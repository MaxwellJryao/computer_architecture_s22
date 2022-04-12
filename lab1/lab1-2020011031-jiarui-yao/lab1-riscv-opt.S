.data
# -------------------------------------- TODO: FILL IN YOUR FILE PATH !!! -------------------------------------------
read_path:
        .string "/home/arch_stu2020011031/yao-archlab-s22/lab1/sample/turing_before.pixel"
write_path:
        .string "/home/arch_stu2020011031/yao-archlab-s22/lab1/sample/riscv_result.pixel"
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

        slli a7, s7, 2
        j Condx_pre
Loopx_pre:
        slli a5, a0, 2
        add a6, s4, a5 # a6 = &result_img[x*m]
        add a5, s3, a5 
        add t1, a7, a5 
        lw a5, 0(a5) # a5 = img[x*m]
        sw a5, 0(a6)
        add a6, a6, a7 # a6 = &result_img[x*m+m-1]
        lw t1, 0(t1) # t1 = img[x*m+m-1]
        sw t1, 0(a6)
        add a0, a0, s2 
        addi t0, t0, 1
Condx_pre:
        blt t0, s1, Loopx_pre 
        addi t1, zero, 1 # y = 1
        j Condy_pre 
Loopy_pre:
        slli a5, t1, 2
        add a6, s4, a5 # a6 = &result_img[y] 
        add a5, a5, s3 
        lw a5, 0(a5) # a5 = img[y]
        sw a5, 0(a6)
        add a5, t1, a0 
        sub a5, a5, s2 
        slli a5, a5, 2
        add a6, s4, a5
        add a5, a5, s3 
        lw a5, 0(a5) # a5 = img[(n-1)*m+y]
        sw a5, 0(a6)
        addi t1, t1, 1
Condy_pre:
        blt t1, s7, Loopy_pre 

        addi t0, zero, 1 # x = 1
        add a0, zero, s2 # reset a0 to m
        j Condx
Loopx: 
        addi t1, zero, 1 # t1 is y
        # At the beginning of each row, preload the initial 9 pixels
        add s5, a0, t1 # s5 = x*m + y
        slli s5, s5, 2
        add s8, s5, s4 # s8 = &result_img[x*m+y]
        # sw zero, 0(s8) # result_img[x*m+y] = 0
        add s9, s3, s5 # s9 = &img[x*m+y]
        addi t2, s9, -4 # t2 = &img[x*m+y-1]
        addi t3, s9, 4 # t3 = &img[x*m+y+1]
        slli t4, s2, 2 # t4 = 4m
        add t5, s9, t4 # t5 = &img[(x+1)m+y]
        sub t6, s9, t4 # t6 = &img[(x-1)m+y]
        addi t4, t5, 4 # t4 = &img[(x+1)m+y+1] 
        addi a1, t5, -4 # a1 = &img[(x+1)m+y-1]
        addi a2, t6, 4 # a2 = &img[(x-1)m+y+1]
        addi a3, t6, -4 # a3 = &img[(x-1)m+y-1] 

        # load the 9 pixels' values
        lw s9, 0(s9)
        lw t2, 0(t2)
        lw t3, 0(t3)
        lw t4, 0(t4)
        lw t5, 0(t5)
        lw t6, 0(t6)
        lw a1, 0(a1)
        lw a2, 0(a2)
        lw a3, 0(a3)

        add a4, a2, a3
        add a4, a4, a1
        add a4, a4, t4 # a4 = the sum of 4 corner pixels
        slli a5, t6, 1
        add a4, a5, a4 
        slli a5, t5, 1
        add a4, a5, a4 
        slli a5, t2, 1
        add a4, a5, a4 
        slli a5, t3, 1
        add a4, a5, a4 
        slli a5, s9, 2
        add a4, a5, a4 
        srli a4, a4, 4
        sw a4, 0(s8)

        addi t1, t1, 1
        j Condy 
Loopy:
        # addi t1, t1, 1
        # set the initial value to 0
        # and check the edge case,
        # place the value x*m+y at s5 register
        
        # move the kernel window 1 pixel right 
        add a3, zero, t6
        add t2, zero, s9 
        add a1, zero, t5        
        add t6, zero, a2 
        add s9, zero, t3 
        add t5, zero, t4 
        add s5, a0, t1 # s5 = xm+y
        slli s5, s5, 2
        add s8, s5, s4 # s8 = &result_img[xm+y]
        add s5, s3, s5 # s5 = &img[xm+y]
        addi s5, s5, 4 # s5 = &img[xm+y+1]
        slli a4, s2, 2 # a4 = 4m 
        sub a2, s5, a4 
        add t4, s5, a4 
        
        lw t3, 0(s5)
        lw a2, 0(a2)
        lw t4, 0(t4)

        add a4, a2, a3
        add a4, a4, a1
        add a4, a4, t4 # a4 = the sum of 4 corner pixels
        slli a5, t6, 1
        add a4, a5, a4 
        slli a5, t5, 1
        add a4, a5, a4 
        slli a5, t2, 1
        add a4, a5, a4 
        slli a5, t3, 1
        add a4, a5, a4 
        slli a5, s9, 2
        add a4, a5, a4 
        srli a4, a4, 4
        sw a4, 0(s8)

        # lw s9, 0(s9) # s9 is img[x*m+y]
        # add t5, zero, zero # t5 is the temporary value storing the mediate value 
        # sub a1, zero, s2 # a1 = (i-1)*m

        # divide ans by 16, and store the value to result_img
        # srli t5, t5, 4
        # sw t5, 0(s8)
        addi t1, t1, 1 # y += 1
Condy:
        blt t1, s7, Loopy
        add a0, a0, s2 # a0 += m
        addi t0, t0, 1 # x += 1
Condx: 
        blt t0, s6, Loopx 
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
