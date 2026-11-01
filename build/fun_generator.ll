define i32 @callee(i32 %arg0) {
label_entry:
  %op1 = alloca i32
  %op2 = alloca i32
  store i32 %arg0, i32* %op2
  %op3 = load i32, i32* %op2
  %op4 = mul i32 %op3, 2
  store i32 %op4, i32* %op1
  %op5 = load i32, i32* %op1
  ret i32 %op5
}
define i32 @main() {
label_entry:
  %op0 = alloca i32
  store i32 0, i32* %op0
  %op1 = call i32 @callee(i32 110)
  store i32 %op1, i32* %op0
  %op2 = load i32, i32* %op0
  ret i32 %op2
}
