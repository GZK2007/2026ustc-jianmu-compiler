	.text
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -32
.main_label_entry:
# %op0 = fcmp ugt float 0x4016000000000000, 0x3ff0000000000000
	lu12i.w $t0, 0x40b00
	movgr2fr.w $ft0, $t0
	lu12i.w $t1, 0x3f800
	movgr2fr.w $ft1, $t1
	fcmp.slt.s $fcc0, $ft1, $ft0
	addi.w $t0, $zero, 1
	bcnez $fcc0, .main_fcmp_end
	addi.w $t0, $zero, 0
.main_fcmp_end:
	st.b $t0, $fp, -17
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
