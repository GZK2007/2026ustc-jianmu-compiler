	.text
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -32
.main_label_entry:
# %op0 = icmp sgt i32 5, 1
	addi.w $t0, $zero, 5
	addi.w $t1, $zero, 1
	slt $t2, $t1, $t0
	st.b $t2, $fp, -17
# %op1 = zext i1 %op0 to i32
	ld.b $t0, $fp, -17
	st.w $t0, $fp, -21
# %op2 = icmp ne i32 %op1, 0
	ld.w $t0, $fp, -21
	sltu $t1, $zero, $t0
	st.b $t1, $fp, -22
# br i1 %op2, label %label3, label %label4
	ld.b $t0, $fp, -22
	bnez $t0, .main_label3
	b .main_label4
.main_label3:
# ret i32 233
	addi.w $a0, $zero, 233
	b main_exit
.main_label4:
# ret i32 0
	addi.w $a0, $zero, 0
	b main_exit
main_exit:
	addi.d $sp, $sp, 32
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
