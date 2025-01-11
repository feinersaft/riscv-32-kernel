#include "kernel.h"
#include "common.h"


extern char __bss[], __bss_end[], __stack_top[]; // We can refer to the current location counter from kernel.ld here, because we assigned it to the __bss symbol!

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,long arg5, long fid, long eid) {
	register long a0 __asm__("a0") = arg0;
	register long a1 __asm__("a1") = arg1;
	register long a2 __asm__("a2") = arg2;
	register long a3 __asm__("a3") = arg3;
	register long a4 __asm__("a4") = arg4;
	register long a5 __asm__("a5") = arg5;
	register long a6 __asm__("a6") = fid;
	register long a7 __asm__("a7") = eid;

	__asm__ __volatile__("ecall"
			: "=r"(a0), "=r"(a1)
			: "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
                        : "memory");
	return (struct sbiret){.error = a0, .value = a1};
}

void putchar(char ch) {
	sbi_call(ch, 0, 0, 0, 0, 0, 0, 1 /* Console Putchar */);
}

void kernel_main(void) {
	/*
	* printf("\n\nHello %s\n", "World!");
	* printf("1 + 2 = %d, %x\n", 1 + 2, 0x1234abcd);
	*/
	memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

	PANIC("booted!");
	printf("unreachable here!\n");

	for (;;) {
		__asm__ __volatile__("wfi");
	}
}

__attribute__((section(".text.boot"))) // Since OpenSBI simply jumps to 0x80200000 without knowing the entry point, the boot function needs to be placed at 0x80200000.
__attribute__((naked)) // Tells the compiler to not put code before and after the function body. Otherwise it would alter the function body which should only contain assembler code.
void boot(void) { // Execution starts here as defined in the linker script 
	__asm__ __volatile__(
			"mv sp, %[stack_top]\n" // Set the stack pointer to the end address of the stack are (see linker script). It's important to note that the stack grows towards zero, meaning it is decremented as it is used. Therefore, the end address (not the start address) of the stack area must be set.
			"j kernel_main\n"	// Jump to the kernel main function
			:
			: [stack_top] "r" (__stack_top) // Pass the stack top address as %[stack_top]
	);
}
