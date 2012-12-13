//////////////////////////////////////////
// Bootstrap code
//////////////////////////////////////////
// 0x00
boot: 
	// set stack pointer
	mov	$r3 0x1000
	sethi	$r3 0x0
	mov	$sp $r0
	// set interrupt vector
	//// interrupt vector 0
	mov	$r0 #ih // interrupt handler (hardcoded)
	sethi	$r0 0x0
	mov	$iv0 $r0
	//// interrupt vector 1
	mov	$r0 #ih1 // interrupt handler (hardcoded)
	sethi	$r0 0x0
	mov	$iv1 $r0
	clear b32 $r0
	// set fuc_base + 0x800 = 0
	//mov 	$r15 0x0
	//sethi 	$r15 0x20000
	//mov 	$r14 0x0
	//iowr 	I[$r15] $r14
	// jump to the main function
	call	#main
	// set fuc_base + 0x800 = 1
	//mov 	$r15 0x0
	//sethi 	$r15 0x20000
	//mov 	$r14 0x1
	//iowr 	I[$r15] $r14
	// exit the program
	exit
// 0x2f
ih:

        push	$r0
	push	$r1
	push	$r2
	push	$r3
	push	$r4
	push	$r5
	push	$r6
	push	$r7
	push	$r8
	push	$r9
	push	$r10
	push	$r11
	push	$r12
	push	$r13
	push	$r14
	push	$r15
	mov $r10 $flags
        push    $r10
        call	#ihbody
        pop $r10
        mov $flags $r10
	pop	$r15
	pop	$r14
	pop	$r13
	pop	$r12
	pop	$r11
	pop	$r10
	pop	$r9
	pop	$r8
	pop	$r7
	pop	$r6
	pop	$r5
	pop	$r4
	pop	$r3
	pop	$r2
	pop	$r1
	pop	$r0
	bset	$flags $p2
	iret

// 0x2f
ih1:

        push	$r0
	push	$r1
	push	$r2
	push	$r3
	push	$r4
	push	$r5
	push	$r6
	push	$r7
	push	$r8
	push	$r9
	push	$r10
	push	$r11
	push	$r12
	push	$r13
	push	$r14
	push	$r15
	mov $r10 $flags
        push    $r10
        call	#ihbody1
        pop $r10
        mov $flags $r10
	pop	$r15
	pop	$r14
	pop	$r13
	pop	$r12
	pop	$r11
	pop	$r10
	pop	$r9
	pop	$r8
	pop	$r7
	pop	$r6
	pop	$r5
	pop	$r4
	pop	$r3
	pop	$r2
	pop	$r1
	pop	$r0
	bset	$flags $p2
	iret

//////////////////////////////////////////
// User code
///////////////////////////////////////////

