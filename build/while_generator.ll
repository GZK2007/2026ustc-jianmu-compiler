define i32 @main() {
label_entry:
  %op0 = alloca i32
  store i32 0, i32* %op0
  %op1 = alloca i32
  %op2 = alloca i32
  store i32 10, i32* %op1
  store i32 0, i32* %op2
  br label %label_check
label_check:                                                ; preds = %label_entry, %label_while
  %op3 = load i32, i32* %op2
  %op4 = icmp slt i32 %op3, 10
  br i1 %op4, label %label_while, label %label_end
label_while:                                                ; preds = %label_check
  %op5 = load i32, i32* %op2
  %op6 = add i32 %op5, 1
  store i32 %op6, i32* %op2
  %op7 = load i32, i32* %op1
  %op8 = add i32 %op7, %op6
  store i32 %op8, i32* %op1
  br label %label_check
label_end:                                                ; preds = %label_check
  %op9 = load i32, i32* %op1
  store i32 %op9, i32* %op0
  %op10 = load i32, i32* %op0
  ret i32 %op10
}
