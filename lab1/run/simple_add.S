.data
# --------------------------------------- TODO: !!! FILL IN YOUR FILE PATH !!! ---------------------------------------------
read_path:
        .string "/home/your_user_name/yao-archlab-s22/lab1/run/simple_add_data.txt"
# ------------------------------------------------------------------------------------------------------------------

.text
main:
        addi    sp, sp, -32
        sw      ra, 12(sp)
        sw      s0, 24(sp)
        addi    a5, sp, 4
        li      a2, 2
        mv      a1, a5
        la      a0, read_path
        jal     _read_int
        lw      a3,0(a5)
        lw      a4,4(a5)
        add     a0,a3,a4
        jal     _printf_num
        lw      ra,12(sp)
        li      a0,0
        li a7, 10
        ecall
        addi    sp,sp,16
        jr      ra


_printf_num:
# DO NOT MODIFY THIS!!!
        li a7, 1
        ecall 
        ret


_printf_char:
# DO NOT MODIFY THIS!!!
        li a7, 11
        ecall 
        ret


_read_int:
# DO NOT MODIFY THIS!!!
        mv      a5,a1
        mv      a4,a2
        li a7, 1024
        li a1, 0
        ecall
        mv t3, a0
        li a7, 65
        mv a1, a5
        mv a2, a4
        ecall
        li a7, 57
        mv a0, t3
        ecall
        ret