.global main
main:
        // R1 = DISPCNT
        mov     r1, #0x04000000
        // enable BG2
        mov     r0, #0x0400
        // enable Mod3
        orr     r0, #3
        strh    r0, [r1, #0]

        // R0 = Red color
        mov     r0, #0x001f

        mov     r1, #0x06000000
        add     r1, #38400 // (bytes_per_pixel=2) * (y=80) * (display_width=240)
        mov     r2, #0
loop:
        strh    r0, [r1, #0]
        add     r1, #2
        add     r2, #1
        cmp     r2, #240
        bne     loop

die:
        b       die
