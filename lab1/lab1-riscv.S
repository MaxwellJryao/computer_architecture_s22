.data
# -------------------------------------- TODO: FILL IN YOUR FILE PATH !!! -------------------------------------------
read_path:
        .string "/home/arch_stu2020011031/yao-archlab-s22/lab1/sample/turing_before.pixel"
write_path:
        .string "/home/arch_stu2020011031/yao-archlab-s22/lab1/sample/output/riscv_result.pixel"
# ------------------------------------------------------------------------------------------------------------------
img:
        .zero   1000000
result_img: 
        .zero   1000000
n:
        .zero   4
m:
        .zero   4
size: 
        .zero   8

.text
main:
        addi    sp,sp,-16
        sw      ra,12(sp)
        jal    image_input
        jal    image_process
        jal    image_output
        jal    image_to_file
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


_write_int:
# DO NOT MODIFY THIS!!! 
        mv a5, a1
        mv a4, a2
        li a7, 1024
        li a1, 1
        ecall
        mv t3, a0
        li a7, 66
        mv a1, a5
        mv a2, a4
        ecall
        li a7, 57
        mv a0, t3
        ecall
        ret 


image_process:
# --------------------------------------- TODO: FILL IN CODE HERE ---------------------------------------
        # initialize and save the callee-save registers
        addi sp, sp, -44 
        sw s0, 40(sp)
        sw s1, 36(sp)
        sw s2, 32(sp)
        sw s3, 28(sp)
        sw s4, 24(sp)
        sw s5, 20(sp)
        sw s6, 16(sp)
        sw s7, 12(sp)
        sw s8, 8(sp)
        sw s9, 4(sp)
        la s1, n 
        lw s1, 0(s1)
        la s2, m 
        lw s2, 0(s2)
        la s3, img 
        la s4, result_img
        addi s6, s1, -1 # n - 1
        addi s7, s2, -1 # m - 1
        add t0, zero, zero # t0 is x
        addi t4, zero, 3 # t4 is 3, i.e. the size of kernel
        add a0, zero, zero # a0 stores the value of x*m
        j Condx
Loopx: 
        add t1, zero, zero # t1 is y
        j Condy 
Loopy:
        # addi t1, t1, 1
        # set the initial value to 0
        # and check the edge case,
        # place the value x*m+y at s5 register
        add s5, a0, t1 # s5 = x*m + y
        slli s5, s5, 2
        add s8, s5, s4 
        sw zero, 0(s8) # result_img[x*m+y] = 0
        add s9, s3, s5 
        lw s9, 0(s9) # s9 is img[x*m+y]
        bne t0, zero, Condy_pre1
        sw s9, 0(s8)
        addi t1, t1, 1
        j Condy
Condy_pre1:
        bne t1, zero, Condy_pre2
        sw s9, 0(s8)
        addi t1, t1, 1
        j Condy
Condy_pre2:
        bne t0, s6, Condy_pre3
        sw s9, 0(s8)
        addi t1, t1, 1
        j Condy
Condy_pre3:
        bne t1, s7, Condy_pre4
        sw s9, 0(s8)
        addi t1, t1, 1
        j Condy
Condy_pre4:
        add t5, zero, zero # t5 is the temporary value storing the mediate value 
        sub a1, zero, s2 # a1 = (i-1)*m
        add t2, zero, zero # t2 is i
        j Condi 
Loopi: 
        add t3, zero, zero # t3 is j
        j Condj
Loopj:
        # update the value iteratively, assume t6 is img[(x+i-1)*m+y+j-1]*K[i][j]
        # add t6, zero, t3 
        addi t6, t3, -1 # j - 1
        add t6, t6, a1 # (i-1)*m + j-1
        slli t6, t6, 2
        add t6, t6, s5
        add t6, s3, t6
        lw t6, 0(t6) # img[(x+i-1)*m+y+j-1]
        addi a3, t2, -1 # i - 1
        addi a4, t3, -1 # j - 1
        sltiu a3, a3, 1 # a3 = 1 iff i == 1
        sltiu a4, a4, 1 # a4 = 1 iff j == 1
        add a2, a3, a4
        sll t6, t6, a2 # a2 = log(K[i][j])
        add t5, t5, t6

        addi t3, t3, 1 # j += 1
Condj:
        bne t3, t4, Loopj 
        add a1, a1, s2 

        addi t2, t2, 1 # i += 1
Condi:
        bne t2, t4, Loopi
        # divide ans by 16, and store the value to result_img
        srli t5, t5, 4
        sw t5, 0(s8)
        addi t1, t1, 1 # y += 1
Condy:
        bne t1, s2, Loopy
        add a0, a0, s2 # a0 += m
        addi t0, t0, 1 # x += 1
Condx: 
        bne t0, s1, Loopx 
        # restore the callee-save registers
        lw s0, 40(sp)
        lw s1, 36(sp)
        lw s2, 32(sp)
        lw s3, 28(sp)
        lw s4, 24(sp)
        lw s5, 20(sp)
        lw s6, 16(sp)
        lw s7, 12(sp)
        lw s8, 8(sp)
        lw s9, 4(sp)
        addi sp, sp, 44
        ret
# -------------------------------------------------------------------------------------------------------

# IMAGE OUTPUT
image_output:
# DO NOT MODIFY THIS!!!
        addi    sp,sp,-32
        sw      ra,28(sp)
        sw      s0,24(sp)
        sw      s1,20(sp)
        sw      s2,16(sp)
        sw      s3,12(sp)
        sw      s4,8(sp)
        sw      s5,4(sp)
        lw s1, n
        mv a0, s1
        jal _printf_num 
        li a0, 32
        jal _printf_char
        lw s2, m
        mv a0, s2
        jal _printf_num  
        li a0, 10
        jal _printf_char

        mul s1, s1, s2 
        la s3, result_img
        addi t1, zero, 0 
        addi s5, zero, 0
        bge t1, s1, L1 
L2:     
        lw a0, 0(s3)
        jal _printf_num
        li a0, 32
        jal _printf_char
        addi s5, s5, 1
        bge s5, s2, PRINT_NEXT_LINE
PRINT_END:
        addi s3, s3, 4
        addi t1, t1, 1
        blt t1, s1, L2
L1:
        lw      ra,28(sp)
        lw      s0,24(sp)
        lw      s1,20(sp)
        lw      s2,16(sp)
        lw      s3,12(sp)
        lw      s4,8(sp)
        lw      s5,4(sp)
        addi    sp,sp,32
        jr      ra
PRINT_NEXT_LINE:
        li a0, 10
        jal _printf_char
        addi s5, zero, 0
        jal PRINT_END
# IMAGE_OUTPUT_END

# IMAGE_TO_FILE
image_to_file:
# DO NOT MODIFY THIS!!!
    addi sp,sp,-4
    sw ra,0(sp)

    la a0, write_path
    li a7, 1024
    li a1, 1
    ecall
    mv t3, a0
    lw t0, n
    lw t1, m
    mul t2, t0, t1

    la a1, size
    li a2, 2
    li a7, 66
    ecall
    la t2, result_img
    addi t4, zero, 0
OUTPUT_ONE_LINE: 
    mv a0, t3
    mv a1, t2
    mv a2, t1
    li a7, 66
    ecall
    slli a3, t1, 2
    add t2, t2, a3
    addi t4, t4, 1
    blt t4, t0, OUTPUT_ONE_LINE

    mv a0, t3
    li a7, 57
    ecall
    lw ra,0(sp)
    addi sp,sp,4
    jr ra
# IMAGE_TO_FILE_END

# IMAGE_INPUT
image_input:
# DO NOT MODIFY THIS!!!
    addi sp,sp,-4
    sw ra, 0(sp)
    li a7, 1024
    li a1, 0
    la a0, read_path
    ecall
    mv t3, a0
    li a7, 65
    la a1, size
    li a2, 2
    ecall

    la a1, size
    la a5, n
    lw t4, 0(a1)
    sw t4, 0(a5)
    la a5, m
    lw t5, 4(a1)
    sw t5, 0(a5)
    mul t2, t4, t5
    mv a0, t3
    la a1, img
    mv a2, t2
    ecall
    mv a0, t3
    li a7, 57
    ecall
    lw ra, 0(sp)
    addi sp, sp, 4
    mv a0, t4
    mv a1, t5
    jr ra
# IMAGE_INPUT_END
