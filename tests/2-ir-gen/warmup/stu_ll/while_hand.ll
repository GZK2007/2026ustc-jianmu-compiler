define i32 @main(){
    %a = alloca i32
    %i = alloca i32
    store i32 10, i32* %a
    store i32 0, i32* %i
    br label %check

check: 
    %i_val = load i32, i32* %i
    %conorbrk = icmp slt i32 %i_val, 10
    br i1 %conorbrk, label %while, label %end

while:
    %ival = load i32, i32* %i
    %iadd = add i32 %ival, 1
    store i32 %iadd, i32* %i
    %aval = load i32, i32* %a
    %aadd = add i32 %aval, %iadd
    store i32 %aadd, i32* %a
    br label %check

end:
    %a_val = load i32, i32* %a
    ret i32 %a_val
}