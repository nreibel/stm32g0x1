.syntax unified
.cpu cortex-m0plus
.fpu softvfp

.global reset_handler
.type reset_handler, %function

reset_handler:
    /* set stack pointer */
    ldr r0, =_estack
    mov sp, r0

    /* run boot code and jump to main */
    bl init
    bl main

    /* infinite loop */
loop:
    b loop

.size reset_handler, .-reset_handler
