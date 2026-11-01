define i32 @main() {
label_entry:
  %op0 = alloca i32
  store i32 0, i32* %op0
  %op1 = alloca [10 x i32]
  %op2 = getelementptr [10 x i32], [10 x i32]* %op1, i32 0, i32 0
  store i32 10, i32* %op2
  %op3 = getelementptr [10 x i32], [10 x i32]* %op1, i32 0, i32 1
  %op4 = load i32, i32* %op2
  %op5 = mul i32 %op4, 2
  store i32 %op5, i32* %op3
  %op6 = load i32, i32* %op3
  store i32 %op6, i32* %op0
  %op7 = load i32, i32* %op0
  ret i32 %op7
}
