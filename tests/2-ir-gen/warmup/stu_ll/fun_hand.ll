define i32 @callee(i32 %a){
    %a2 = mul i32 %a, 2
    ret i32 %a2
}

define i32 @main(){
    %retvalue = call i32 @callee(i32 110)
    ret i32 %retvalue
}