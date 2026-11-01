#include "BasicBlock.hpp"
#include "Constant.hpp"
#include "Function.hpp"
#include "IRBuilder.hpp"
#include "Module.hpp"
#include "Type.hpp"

#include <iostream>
#include <memory>

// 定义一个从常数值获取/创建 ConstantInt 类实例化的宏，方便多次调用
#define CONST_INT(num) ConstantInt::get(num, module)

// 定义一个从常数值获取/创建 ConstantFP 类实例化的宏，方便多次调用
#define CONST_FP(num) ConstantFP::get(num, module)

int main() {
    auto module = new Module();
    auto builder = new IRBuilder(nullptr, module);
    Type *Int32Type = module->get_int32_type();
    auto *arrayType = ArrayType::get(Int32Type, 10);
    auto mainFun = Function::create(FunctionType::get(Int32Type, {}), "main", module);
    auto bb = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(bb);
    auto retAlloca = builder->create_alloca(Int32Type);
    // main 函数默认 ret 0
    builder->create_store(CONST_INT(0), retAlloca);
    // 为 a 分配空间，类型为 [10 x i32]
    auto aAlloca = builder->create_alloca(arrayType);
    // 取 a[0] 的地址
    auto a0GEP = builder->create_gep(aAlloca, {CONST_INT(0), CONST_INT(0)});
    // a[0] <- 10
    builder->create_store(CONST_INT(10), a0GEP);
    // 取 a[1] 的地址
    auto a1GEP = builder->create_gep(aAlloca, {CONST_INT(0), CONST_INT(1)});
    // a[1] <- a[0] * 2
    auto a0Load = builder->create_load(a0GEP);
    auto mul = builder->create_imul(a0Load, CONST_INT(2));
    builder->create_store(mul, a1GEP);
    // 将 a[1] 的值取出来
    auto a1Load = builder->create_load(a1GEP);
    builder->create_store(a1Load, retAlloca);
    auto retLoad = builder->create_load(retAlloca);
    builder->create_ret(retLoad);
    std::cout << module->print();
    delete module;
    return 0;
}