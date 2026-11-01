; ModuleID = 'calculator'
declare void @output(i32)

define i32 @main() {
label_entry:
  %op0 = add i32 8, 4
  %op1 = sub i32 %op0, 1
  %op2 = mul i32 4, %op1
  %op3 = sdiv i32 %op2, 2
  call void @output(i32 %op3)
  ret i32 0
}
