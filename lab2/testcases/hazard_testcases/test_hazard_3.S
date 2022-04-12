# EXPECTED CYCLES: <= 42
# EXPECTED OUTPUT: 123

.text:
main:
    addi    sp, sp, -32
    sw      ra, 12(sp)
    sw      s0, 24(sp)

    addi    sp, sp, -32
    mv      a1, sp
    addi    a1, a1, 28
    addi    a2, zero, 123
    sw      a2, 0(a1)
    lw      a0, 28(sp)
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
