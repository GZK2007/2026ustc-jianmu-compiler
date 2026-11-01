define i32 @main() {
label_entry:
  %op0 = alloca i32
  store i32 0, i32* %op0
  %op1 = alloca float
  store float 0x40163851e0000000, float* %op1
  %op2 = load float, float* %op1
  %op3 = fcmp ugt float %op2, 0x3ff0000000000000
  br i1 %op3, label %label_trueBB, label %label_falseBB
label_trueBB:                                                ; preds = %label_entry
  store i32 233, i32* %op0
  br label %label_retBB
label_falseBB:                                                ; preds = %label_entry
  store i32 0, i32* %op0
  br label %label_retBB
label_retBB:                                                ; preds = %label_trueBB, %label_falseBB
  %op4 = load i32, i32* %op0
  ret i32 %op4
}
