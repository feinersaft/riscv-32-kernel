typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[]; // We can refer to the current location counter from kernel.ld here, because we assigned it to the __bss symbol!

void *memset(void *buf, char c, size_t n) {
	uint8_t *p = (uint8_t *) buf;
	while (n--)
		*p++ = c;
	return buf;
}

void kernel_main(void) {
	memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

	for (;;);
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
