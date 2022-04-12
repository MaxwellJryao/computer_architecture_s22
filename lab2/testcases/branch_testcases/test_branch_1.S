# EXPECTED OUTPUT: 10

.text
main:
        addi    sp, sp, -32
        sw      ra, 12(sp)
        sw      s0, 24(sp)
        li      t1, 0
        li      t2, 10
loop: 
        addi    a1, a0, 1
        addi    t1, t1, 1
        bge     t1, t2, endloop
        addi    a4, zero, 1
        addi    a5, zero, 1
        addi    a0, zero, 1
        jal     loop
endloop:
        mv      a0, t1
        jal     _printf_num
        li a7, 10
        ecall
        addi    sp,sp,16
        jr      ra


_printf_num:
        li a7, 1
        ecall 
        ret


_printf_char:
        li a7, 11
        ecall 
        ret