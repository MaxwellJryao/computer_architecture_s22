.data
# -------------------------------------- TODO: FILL IN YOUR FILE PATH !!! -------------------------------------------
read_path:
        .string "/home/your_user_name/yao-archlab-s22/lab1/sample/turing_before.pixel"
write_path:
        .string "your/path/to/output/file"
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