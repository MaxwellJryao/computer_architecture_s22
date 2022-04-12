# EXPECTED OUTPUT: 0123456789

.text
main:
        addi    sp, sp, -32
        sw      ra, 12(sp)
        sw      s0, 24(sp)
        li      t1, 0
        li      t2, 10
loop: 
        mv      a0, t1
        li a7, 1
        ecall 
        addi    t1, t1, 1
        blt     t1, t2, loop
        li a7, 10
        ecall
        addi    sp,sp,16
        jr      ra
