/*
 * 	The is teh binary code to be executed as a BootROM extension for Dove X0
 */

	/* Binary Header */

	.word	0xD00D0214	/* SAR Address */
	.word	0x00000000	/* SAR Mask */
	.word	0x00000000	/* SAR Value */
	.word	0x30		/* Source */
	.word	0xC8010030	/* Destination */
	.word	0x0		/* Size (Calculated in doimage) */
	.word	0xC8010030	/* Execution */
	.word	0x3		/* Param1 (DDR3 WL type - 2CS full WL) */
	.word	0x7		/* Param2 (M_RESET WA MPP Number + 1) */
	.word	0x5		/* Param3 (CKE WA MPP Number + 1)*/
	.word	0xFF		/* Param4 (WA Delay in  MS) */	
	.word	0x0000000F	/* 8bit checksum (Calculated in doimage) - 16bit reserved - 8bit param flags */
        
.globl _start
_start:

	stmfd	sp!, {r4 - r12, lr}	/* save registers on stack 	*/
	ldr	r0, [r0]		/* M_RESET WA MPP Number 	*/
	ldr	r1, [r1]		/* CKE WA MPP Number		*/
	ldr	r2, [r2]		/* WA Delay			*/
	ldr	r3, [r3]		/* DDR3 WL type 		*/
	bl	ddr3_wl2cs
	mov	r0, #0x0		/* return value: 0x0 NO_ERR 	*/
	ldmfd	sp!, {r4 - r12, pc}	/* restore regs and return 	*/
