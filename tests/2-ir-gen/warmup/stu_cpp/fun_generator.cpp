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

    // 创建 callee
    std::vector<Type *> Ints(1, Int32Type);
    // 返回值类型为 Int32Type，参数类型列表为 Ints
    auto calleeFunTy = FunctionType::get(Int32Type, Ints);
    auto calleeFun = Function::create(calleeFunTy, "callee", module);
    auto bb = BasicBlock::create(module, "entry", calleeFun);
    builder->set_insert_point(bb);
    // 为返回值分配空间
    auto retAlloca = builder->create_alloca(Int32Type);
    // 为 a 分配空间
    auto aAlloca = builder->create_alloca(Int32Type);
    std::vector<Value *> args;
    for (auto &arg : calleeFun->get_args()) {
        args.push_back(&arg);
    }
    builder->create_store(args[0], aAlloca);
    auto aLoad = builder->create_load(aAlloca);
    // 计算 2*a 
    auto mul = builder->create_imul(aLoad, CONST_INT(2));
    // 将计算结果存至为返回值
    builder->create_store(mul, retAlloca);
    auto retLoad = builder->create_load(retAlloca);
    builder->create_ret(retLoad);

    // 创建 main
    auto mainFun =
        Function::create(FunctionType::get(Int32Type, {}), "main", module);
    bb = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(bb);
    auto mainRetAlloca = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(0), mainRetAlloca);
    // 调用 callee 函数，实参为110
    auto call = builder->create_call(calleeFun, {CONST_INT(110)});
    // 将 call 的结果存至为返回值
    builder->create_store(call, mainRetAlloca);
    auto mainRetLoad = builder->create_load(mainRetAlloca);
    builder->create_ret(mainRetLoad);

    std::cout << module->print();
    delete module;
    return 0;
}