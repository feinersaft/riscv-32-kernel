ENTRY(boot) /* defines the first executable instruction in an output file */


SECTIONS {
	/* Assignments to the . means "move the location counter to the position <value>. In this case, the location counter is moved to the position 0x80200000 */
	. = 0x80200000;

	/* We define output sections: */
	.text : { /* Name of the output section */
		KEEP(*(.text.boot)); /* these two lines define the content of the output section */
		*(.text , .text.*); /* we put the .text and .text.* section of an input file in this output section; we put a comma between the sections just for comprehensiveness */
	}

	.rodata : ALIGN(4) { /* The ALIGN(4) directive ensures that the current address is adjusted to a 4-byte boundary */
		*(.rodata , .rodata.*); /* the * at the beginning means: "search in all input files for the sections referenced here */
	}

	.data : ALIGN(4) {
		*(.data , .data.*);
	}

	.bss : ALIGN(4) {											
	       	__bss = .;
		*(.bss , .bss.* , .sbss , .sbss.*);
	       	__bss_end = .;
 	}

	. = ALIGN(4);
	. += 128 * 1024; /* 128KB */
	__stack_top = .; /* Defines the end address of the stack area. */
}
