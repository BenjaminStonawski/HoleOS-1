/*
 * Interrupt handler
 *
 * This file is part of HoleOS.
 *
 * HoleOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HoleOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HoleOS.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <common/spinlock.h>
#include <common/x86.h>
#include <core/mmu.h>
#include <core/proc.h>
#include <core/traps.h>
#include <defs.h>
#include <driver/pci/pci.h>
#include <memlayout.h>
#include <param.h>

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern unsigned int vectors[]; // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
unsigned int ticks;

void tvinit(void) {
	int i;

	for (i = 0; i < 256; i++)
		SETGATE(idt[i], 0, SEG_KCODE << 3, vectors[i], 0);
	SETGATE(idt[T_SYSCALL], 1, SEG_KCODE << 3, vectors[T_SYSCALL], DPL_USER);

	initlock(&tickslock, "time");
}

void idtinit(void) {
	lidt(idt, sizeof(idt));
}

// PAGEBREAK: 41
void trap(struct trapframe* tf) {
	if (tf->trapno == T_SYSCALL) {
		if (myproc()->killed)
			exit();
		myproc()->tf = tf;
		syscall();
		if (myproc()->killed)
			exit();
		return;
	}

	switch (tf->trapno) {
	case T_IRQ0 + IRQ_TIMER:
		if (cpuid() == 0) {
			acquire(&tickslock);
			ticks++;
			wakeup(&ticks);
			release(&tickslock);
		}
		lapiceoi();
		break;
	case T_IRQ0 + IRQ_IDE:
		lapiceoi();
		break;
	case T_IRQ0 + IRQ_IDE + 1:
		// Bochs generates spurious IDE1 interrupts.
		break;
	case T_IRQ0 + IRQ_KBD:
		kbdintr();
		lapiceoi();
		break;
	case T_IRQ0 + IRQ_COM1:
		uartintr();
		lapiceoi();
		break;
	case T_IRQ0 + 10:
		pci_interrupt(10);
		lapiceoi();
		break;
	case T_IRQ0 + 11:
		pci_interrupt(11);
		lapiceoi();
		break;
	case T_IRQ0 + 7:
	case T_IRQ0 + IRQ_SPURIOUS:
		cprintf("cpu%d: spurious interrupt at %x:%x\n", cpuid(), tf->cs, tf->eip);
		lapiceoi();
		break;

	// PAGEBREAK: 13
	default:
		if (myproc() == 0 || (tf->cs & 3) == 0) {
			// In kernel, it must be our mistake.
			cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n", tf->trapno,
					cpuid(), tf->eip, rcr2());
			panic("trap");
		}
		// In user space, assume process misbehaved.
		cprintf("pid %d %s: trap %d err %d on cpu %d "
				"eip 0x%x addr 0x%x--kill proc\n",
				myproc()->pid, myproc()->name, tf->trapno, tf->err, cpuid(), tf->eip,
				rcr2());
		myproc()->killed = 1;
	}

	// Force process exit if it has been killed and is in user space.
	// (If it is still executing in the kernel, let it keep running
	// until it gets to the regular system call return.)
	if (myproc() && myproc()->killed && (tf->cs & 3) == DPL_USER)
		exit();

	// Force process to give up CPU on clock tick.
	// If interrupts were on while locks held, would need to check nlock.
	if (myproc() && myproc()->state == RUNNING && tf->trapno == T_IRQ0 + IRQ_TIMER)
		yield();

	// Check if the process has been killed since we yielded
	if (myproc() && myproc()->killed && (tf->cs & 3) == DPL_USER)
		exit();
}
