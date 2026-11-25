	.text
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -96
.main_label_entry:
# %op0 = alloca [10 x i32]
	addi.d $t0, $fp, -64
	st.d $t0, $fp, -24
# %op1 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 0
	ld.d $t0, $fp, -24
	st.d $t0, $fp, -72
# store i32 10, i32* %op1
	ld.d $t0, $fp, -72
	addi.w $t1, $zero, 10
	st.w $t1, $t0, 0
# %op2 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 1
	ld.d $t0, $fp, -24
	addi.d $t0, $t0, 4
	st.d $t0, $fp, -80
# %op3 = load i32, i32* %op1
	ld.d $t0, $fp, -72
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -84
# %op4 = mul i32 %op3, 3
	ld.w $t0, $fp, -84
	addi.w $t1, $zero, 3
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -88
# store i32 %op4, i32* %op2
	ld.w $t0, $fp, -88
	ld.d $t1, $fp, -80
	st.w $t0, $t1, 0
	ld.d $t0, $fp, -80
	ld.w $t1, $t0, 0
	st.w $t1, $fp, -92
# ret i32 %op5
	ld.w $a0, $fp, -92
	b main_exit
main_exit:
	addi.d $sp, $sp, 96
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
