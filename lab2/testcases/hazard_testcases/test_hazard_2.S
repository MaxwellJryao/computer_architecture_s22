# EXPECTED CYCLES: <= 375
# EXPECTED OUTPUT: 0 1 2 3 4 0 1 2 0 1 2 0 1 2 3

.text:
main:
    addi    sp, sp, -32
    sw      ra, 12(sp)
    sw      s0, 24(sp)
    addi    t1, zero, 0
    addi    t2, zero, 5
    addi    a3, zero, 3
    addi    a4, zero, 3
    addi    a5, zero, 0

loop1:
    mv      a0, t1
    jal     _printf_num
    li      a0, 32
    jal     _printf_char
    addi    t1, t1, 1
    blt     t1, t2, loop1

    addi    t3, zero, 0

loop2_1:
    addi    t4, zero, 0
loop2_2:
    mv      a0, t4
    jal     _printf_num
    li      a0, 32
    jal     _printf_char

    addi    t4, t4, 1
    blt     t4, a4, loop2_2
loop2_3:
    addi    t3, t3, 1
    addi    a5, a5, 1
    blt     t3, a3, loop2_1

    mv      a0, a5
    jal     _printf_num

    li a7, 10
    ecall



_printf_num:
    li a7, 1
    ecall
    ret


_printf_char:
    li a7, 11
    ecall
    ret
