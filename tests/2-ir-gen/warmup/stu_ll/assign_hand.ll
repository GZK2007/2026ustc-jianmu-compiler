define i32 @main(){
    %a =  alloca [10 x i32]
    %a0 = getelementptr [10 x i32], [10 x i32]* %a, i32 0, i32 0
    store i32 10, i32* %a0
    %a1 = getelementptr [10 x i32], [10 x i32]* %a, i32 0, i32 1
    %a0value = load i32, i32* %a0
    %a02 = mul i32 %a0value, 2
    store i32 %a02, i32* %a1
    %a1value = load i32, i32* %a1
    ret i32 %a1value
}