define i32 @main(){
    %a = alloca float
    store float 0x40163851e0000000, float* %a
    %aval = load float, float* %a
    %torf = fcmp ugt float %aval, 1.0
    br i1 %torf, label %tbr, label %fbr

tbr:
    ret i32 233

fbr:
    ret i32 0
}