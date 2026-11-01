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

    auto mainFun = Function::create(FunctionType::get(Int32Type, {}), "main", module);
    auto bb = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(bb);
    auto retAlloca = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(0), retAlloca);
    auto aAlloca = builder->create_alloca(Int32Type);
    auto iAlloca = builder->create_alloca(Int32Type);
    // a = 10; i = 0;
    builder->create_store(CONST_INT(10), aAlloca);
    builder->create_store(CONST_INT(0), iAlloca);

    // 条件判断块、循环体块、循环结束块
    auto checkBB = BasicBlock::create(module, "check", mainFun);
    auto whileBB = BasicBlock::create(module, "while", mainFun);
    auto endBB = BasicBlock::create(module, "end", mainFun);
    builder->create_br(checkBB);

    builder->set_insert_point(checkBB);
    auto iVal = builder->create_load(iAlloca);
    // iVal < 10 
    auto icmp = builder->create_icmp_lt(iVal, CONST_INT(10));
    // true 分支跳转至 whileBB，false 分支跳转至 endBB
    builder->create_cond_br(icmp, whileBB, endBB);

    // while 内部
    builder->set_insert_point(whileBB);
    auto iVal2 = builder->create_load(iAlloca);
    // i = i + 1;
    auto iAdd = builder->create_iadd(iVal2, CONST_INT(1));
    builder->create_store(iAdd, iAlloca);
    auto aVal = builder->create_load(aAlloca);
    // a = a + i;
    auto aAdd = builder->create_iadd(aVal, iAdd);
    builder->create_store(aAdd, aAlloca);
    // 跳回判断
    builder->create_br(checkBB);

    builder->set_insert_point(endBB);
    auto aVal2 = builder->create_load(aAlloca);
    // return a;
    builder->create_store(aVal2, retAlloca);
    auto retLoad = builder->create_load(retAlloca);
    builder->create_ret(retLoad);

    std::cout << module->print();
    delete module;
    return 0;
}