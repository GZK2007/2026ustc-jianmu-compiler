	.text
	.globl callee
	.type callee, @function
callee:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -48
	st.w $a0, $fp, -20
.callee_label_entry:
# %op1 = alloca i32
	addi.d $t0, $fp, -32
	st.d $t0, $fp, -28
# store i32 %arg0, i32* %op1
	ld.d $t0, $fp, -28
	ld.w $t1, $fp, -20
	st.w $t1, $t0, 0
# %op2 = load i32, i32* %op1
	ld.d $t0, $fp, -28
	ld.w $t1, $t0, 0
	st.w $t1, $fp, -36
# %op3 = mul i32 3, %op2
	ld.w $t0, $fp, -36
	addi.w $t1, $zero, 3
	mul.w $t0, $t0, $t1
	st.w $t0, $fp, -40
# ret i32 %op3
	ld.w $a0, $fp, -40
	b callee_exit
callee_exit:
	addi.d $sp, $sp, 48
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -32
.main_label_entry:
# %op0 = call i32 @callee(i32 110)
	addi.w $a0, $zero, 110
	bl callee
	st.w $a0, $fp, -20
# ret i32 %op0
	ld.w $a0, $fp, -20
	b main_exit
main_exit:
	addi.d $sp, $sp, 32
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
