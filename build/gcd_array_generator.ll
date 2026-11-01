@x = global [1 x i32] zeroinitializer
@y = global [1 x i32] zeroinitializer
define i32 @gcd(i32 %arg0, i32 %arg1) {
label_entry:
  %op2 = alloca i32
  %op3 = alloca i32
  %op4 = alloca i32
  store i32 %arg0, i32* %op3
  store i32 %arg1, i32* %op4
  %op5 = load i32, i32* %op4
  %op6 = icmp eq i32 %op5, 0
  br i1 %op6, label %label_trueBB, label %label_falseBB
label_trueBB:                                                ; preds = %label_entry
  %op7 = load i32, i32* %op3
  store i32 %op7, i32* %op2
  br label %label14
label_falseBB:                                                ; preds = %label_entry
  %op8 = load i32, i32* %op3
  %op9 = load i32, i32* %op4
  %op10 = sdiv i32 %op8, %op9
  %op11 = mul i32 %op10, %op9
  %op12 = sub i32 %op8, %op11
  %op13 = call i32 @gcd(i32 %op9, i32 %op12)
  store i32 %op13, i32* %op2
  br label %label14
label14:                                                ; preds = %label_trueBB, %label_falseBB
  %op15 = load i32, i32* %op2
  ret i32 %op15
}
define i32 @funArray(i32* %arg0, i32* %arg1) {
label_entry:
  %op2 = alloca i32*
  %op3 = alloca i32*
  %op4 = alloca i32
  %op5 = alloca i32
  %op6 = alloca i32
  store i32* %arg0, i32** %op2
  store i32* %arg1, i32** %op3
  %op7 = load i32*, i32** %op2
  %op8 = getelementptr i32, i32* %op7, i32 0
  %op9 = load i32, i32* %op8
  store i32 %op9, i32* %op4
  %op10 = load i32*, i32** %op3
  %op11 = getelementptr i32, i32* %op10, i32 0
  %op12 = load i32, i32* %op11
  store i32 %op12, i32* %op5
  %op13 = load i32, i32* %op4
  %op14 = load i32, i32* %op5
  %op15 = icmp slt i32 %op13, %op14
  br i1 %op15, label %label_trueBB, label %label_falseBB
label_trueBB:                                                ; preds = %label_entry
  store i32 %op13, i32* %op6
  store i32 %op14, i32* %op4
  %op16 = load i32, i32* %op6
  store i32 %op16, i32* %op5
  br label %label_falseBB
label_falseBB:                                                ; preds = %label_entry, %label_trueBB
  %op17 = load i32, i32* %op4
  %op18 = load i32, i32* %op5
  %op19 = call i32 @gcd(i32 %op17, i32 %op18)
  ret i32 %op19
}
define i32 @main() {
label_entry:
  %op0 = alloca i32
  store i32 0, i32* %op0
  %op1 = getelementptr [1 x i32], [1 x i32]* @x, i32 0, i32 0
  store i32 90, i32* %op1
  %op2 = getelementptr [1 x i32], [1 x i32]* @y, i32 0, i32 0
  store i32 18, i32* %op2
  %op3 = getelementptr [1 x i32], [1 x i32]* @x, i32 0, i32 0
  %op4 = getelementptr [1 x i32], [1 x i32]* @y, i32 0, i32 0
  %op5 = call i32 @funArray(i32* %op3, i32* %op4)
  ret i32 %op5
}
