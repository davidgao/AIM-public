#ifndef _SEGMENT_H
#define _SEGMENT_H

#ifdef __ASSEMBLER__

#define SEG_NULL		\
	.word 0, 0;		\
	.byte 0, 0, 0, 0

// The 0xC0 means the limit is in 4096-byte units
// and (for executable segments) 32-bit mode.
#define SEG(type, base, lim)					\
	.word (((lim) >> 12) & 0xffff), ((base) & 0xffff);	\
	.byte (((base) >> 16) & 0xff), (0x90 | (type)),		\
		(0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

#else /* !__ASSEMBLER__ */

#endif /* !__ASSEMBLER__ */

#define STA_X	0x8	// Executable segment
#define STA_E	0x4	// Expand down (non-executable segments)
#define STA_C	0x4	// Conforming code segment (executable only)
#define STA_W	0x2	// Writeable (non-executable segments)
#define STA_R	0x2	// Readable (executable segments)
#define STA_A	0x1	// Accessed

#define SEG_KCODE	1	// kernel code
#define SEG_KDATA	2	// kernel data+stack
#define SEG_KCPU	3	// kernel per-cpu data
#define SEG_UCODE	4	// user code
#define SEG_UDATA	5	// user data+stack
#define SEG_TSS		6	// this process's task state

// cpu->gdt[NSEGS] holds the above segments.
#define NSEGS     7

#endif /* !_SEGMENT_H */
