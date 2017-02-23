#ifndef _CR_H
#define _CR_H

#ifdef __ASSEMBLER__

#else /* !__ASSEMBLER__ */

#endif /* !__ASSEMBLER__ */

#define CR0_PE	0x00000001	// Protection Enable
#define CR0_MP	0x00000002	// Monitor coProcessor
#define CR0_EM	0x00000004	// Emulation
#define CR0_TS	0x00000008	// Task Switched
#define CR0_ET	0x00000010	// Extension Type
#define CR0_NE	0x00000020	// Numeric Errror
#define CR0_WP	0x00010000	// Write Protect
#define CR0_AM	0x00040000	// Alignment Mask
#define CR0_NW	0x20000000	// Not Writethrough
#define CR0_CD	0x40000000	// Cache Disable
#define CR0_PG	0x80000000	// Paging

#define CR4_PSE	0x00000010	// Page size extension

#endif /* !_CR_H */