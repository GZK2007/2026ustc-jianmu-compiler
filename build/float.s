	.text
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -48
.main_label_entry:
# %op0 = alloca float
	addi.d $t0, $fp, -28
	st.d $t0, $fp, -24
# store float 0x40091eb860000000, float* %op0
	ld.d $t0, $fp, -24
	lu12i.w $t1, 0x4048f
	ori $t1, $t1, 0x5c3
	movgr2fr.w $ft0, $t1
	fst.s $ft0, $t0, 0
# %op1 = load float, float* %op0
	ld.d $t0, $fp, -24
	fld.s $ft0, $t0, 0
	fst.s $ft0, $fp, -32
# %op2 = fptosi float %op1 to i32
	fld.s $ft0, $fp, -32
	ftintrz.w.s $ft1, $ft0
	movfr2gr.s $t0, $ft1
	st.w $t0, $fp, -36
# ret i32 %op2
	ld.w $a0, $fp, -36
	b main_exit
main_exit:
	addi.d $sp, $sp, 48
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
