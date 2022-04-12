# EXPECTED CYCLES: <= 73
# EXPECTED OUTPUT: 3 4 5

.text:
main:
    addi    sp, sp, -32
    sw      ra, 12(sp)
    sw      s0, 24(sp)

    addi    t1, zero, 3
    mv      a0, t1
    jal     _printf_num

    li      a0, 32
    jal     _printf_char

    addi    t2, zero, 4
    addi    a1, zero, 0
    mv      a0, t2
    jal     _printf_num

    li      a0, 32
    jal     _printf_char

    addi    t3, zero, 5
    addi    a1, zero, 0
    addi    a2, zero, 0
    mv      a0, t3
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
