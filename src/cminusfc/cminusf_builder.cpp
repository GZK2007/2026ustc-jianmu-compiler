#include "cminusf_builder.hpp"

#define CONST_FP(num) ConstantFP::get((float)num, module.get())
#define CONST_INT(num) ConstantInt::get(num, module.get())

// types
Type *VOID_T;
Type *INT1_T;
Type *INT32_T;
Type *INT32PTR_T;
Type *FLOAT_T;
Type *FLOATPTR_T;

/*
 * use CMinusfBuilder::Scope to construct scopes
 * scope.enter: enter a new scope
 * scope.exit: exit current scope
 * scope.push: add a new binding to current scope
 * scope.find: find and return the value bound to the name
 */

Value* CminusfBuilder::visit(ASTProgram &node) {
    VOID_T = module->get_void_type();
    INT1_T = module->get_int1_type();
    INT32_T = module->get_int32_type();
    INT32PTR_T = module->get_int32_ptr_type();
    FLOAT_T = module->get_float_type();
    FLOATPTR_T = module->get_float_ptr_type();

    Value *ret_val = nullptr;
    for (auto &decl : node.declarations) {
        ret_val = decl->accept(*this);
    }
    return ret_val;
}

Value* CminusfBuilder::visit(ASTNum &node) {
    if (node.type == TYPE_INT)
        return CONST_INT(node.i_val);
    return CONST_FP(node.f_val);
}//

Value* CminusfBuilder::visit(ASTVarDeclaration &node) {
    Type *var_type = (node.type == TYPE_INT) ? INT32_T : FLOAT_T;
    Type *alloc_type = var_type;
    if (node.num != nullptr) {
        int array_size = node.num->i_val;
        // 合法程序中数组长度一定大于 0，这里做保护避免崩溃
        if (array_size <= 0)
            array_size = 1;
        alloc_type = ArrayType::get(var_type, array_size);
    }

    if (scope.in_global()) {
        // 全局变量需要初始化为全 0
        auto *global_var =
            GlobalVariable::create(node.id, module.get(), alloc_type, false,
                                   ConstantZero::get(alloc_type, module.get()));
        scope.push(node.id, global_var);
    } else {
        // 局部变量的空间统一分配在函数入口块中，
        // 避免循环体内的声明在每次迭代时重复分配栈空间
        auto *local_var = AllocaInst::create_alloca_begin(
            alloc_type, context.func->get_entry_block());
        scope.push(node.id, local_var);
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTFunDeclaration &node) {
    FunctionType *fun_type;
    Type *ret_type;
    std::vector<Type *> param_types;
    if (node.type == TYPE_INT)
        ret_type = INT32_T;
    else if (node.type == TYPE_FLOAT)
        ret_type = FLOAT_T;
    else
        ret_type = VOID_T;

    for (auto &param : node.params) {
        if (param->type == TYPE_INT)
            param_types.push_back(param->isarray ? INT32PTR_T : INT32_T);
        else if (param->type == TYPE_FLOAT)
            param_types.push_back(param->isarray ? FLOATPTR_T : FLOAT_T);
        else
            param_types.push_back(VOID_T);
    }

    fun_type = FunctionType::get(ret_type, param_types);
    auto func = Function::create(fun_type, node.id, module.get());
    scope.push(node.id, func);
    context.func = func;
    auto funBB = BasicBlock::create(module.get(), "entry", func);
    builder->set_insert_point(funBB);
    scope.enter();
    std::vector<Value *> args;
    for (auto &arg : func->get_args()) {
        args.push_back(&arg);
    }
    for (unsigned int i = 0; i < node.params.size(); ++i) {
        auto &param = node.params[i];
        // 为形参分配栈空间，并把实参存入其中
        auto *arg_alloca = builder->create_alloca(param_types[i]);
        builder->create_store(args[i], arg_alloca);
        scope.push(param->id, arg_alloca);
    }
    node.compound_stmt->accept(*this);
    if (not builder->get_insert_block()->is_terminated()) 
    {
        if (context.func->get_return_type()->is_void_type())
            builder->create_void_ret();
        else if (context.func->get_return_type()->is_float_type())
            builder->create_ret(CONST_FP(0.));
        else
            builder->create_ret(CONST_INT(0));
    }
    scope.exit();
    return nullptr;
}

Value* CminusfBuilder::visit(ASTParam &node) {
    // 形参在 ASTFunDeclaration 中统一处理
    return nullptr;
}

Value* CminusfBuilder::visit(ASTCompoundStmt &node) {
    scope.enter();

    for (auto &decl : node.local_declarations) {
        decl->accept(*this);
    }

    for (auto &stmt : node.statement_list) {
        stmt->accept(*this);
        if (builder->get_insert_block()->is_terminated())
            break;
    }

    scope.exit();
    return nullptr;
}

Value* CminusfBuilder::visit(ASTExpressionStmt &node) {
    if (node.expression != nullptr)
        node.expression->accept(*this);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTSelectionStmt &node) {
    auto *cond = node.expression->accept(*this);
    Value *cond_i1;
    if (cond->get_type()->is_int1_type())
        cond_i1 = cond;
    else if (cond->get_type()->is_float_type())
        cond_i1 = builder->create_fcmp_ne(cond, CONST_FP(0.0));
    else
        cond_i1 = builder->create_icmp_ne(cond, CONST_INT(0));

    auto *true_bb = BasicBlock::create(module.get(), "", context.func);
    auto *false_bb = BasicBlock::create(module.get(), "", context.func);
    BasicBlock *next_bb;
    if (node.else_statement == nullptr) {
        // 没有 else 时，条件为假的分支就是 if 之后的块
        next_bb = false_bb;
    } else {
        next_bb = BasicBlock::create(module.get(), "", context.func);
    }

    builder->create_cond_br(cond_i1, true_bb, false_bb);

    builder->set_insert_point(true_bb);
    node.if_statement->accept(*this);
    if (not builder->get_insert_block()->is_terminated())
        builder->create_br(next_bb);

    if (node.else_statement != nullptr) {
        builder->set_insert_point(false_bb);
        node.else_statement->accept(*this);
        if (not builder->get_insert_block()->is_terminated())
            builder->create_br(next_bb);
    }

    builder->set_insert_point(next_bb);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTIterationStmt &node) {
    auto *cond_bb = BasicBlock::create(module.get(), "", context.func);
    auto *body_bb = BasicBlock::create(module.get(), "", context.func);
    auto *next_bb = BasicBlock::create(module.get(), "", context.func);

    builder->create_br(cond_bb);

    // 循环条件
    builder->set_insert_point(cond_bb);
    auto *cond = node.expression->accept(*this);
    Value *cond_i1;
    if (cond->get_type()->is_int1_type())
        cond_i1 = cond;
    else if (cond->get_type()->is_float_type())
        cond_i1 = builder->create_fcmp_ne(cond, CONST_FP(0.0));
    else
        cond_i1 = builder->create_icmp_ne(cond, CONST_INT(0));
    builder->create_cond_br(cond_i1, body_bb, next_bb);

    // 循环体
    builder->set_insert_point(body_bb);
    node.statement->accept(*this);
    if (not builder->get_insert_block()->is_terminated())
        builder->create_br(cond_bb);

    builder->set_insert_point(next_bb);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTReturnStmt &node) {
    if (node.expression == nullptr) {
        builder->create_void_ret();
        return nullptr;
    }

    auto *ret_val = node.expression->accept(*this);
    // 返回值类型与函数返回类型不一致时做隐式类型转换
    auto *ret_type = context.func->get_return_type();
    if (ret_type->is_float_type() && ret_val->get_type()->is_int32_type())
        ret_val = builder->create_sitofp(ret_val, FLOAT_T);
    else if (ret_type->is_int32_type() && ret_val->get_type()->is_float_type())
        ret_val = builder->create_fptosi(ret_val, INT32_T);

    builder->create_ret(ret_val);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTVar &node) {
    auto *base = scope.find(node.id);
    auto *base_type = base->get_type()->get_pointer_element_type();

    if (node.expression == nullptr) {
        if (base_type->is_array_type()) {
            // 数组整体退化为指向首元素的指针（作为数组实参时使用）
            return builder->create_gep(base, {CONST_INT(0), CONST_INT(0)});
        }
        if (base_type->is_pointer_type()) {
            // 数组形参：alloca 中保存的是指向首元素的指针
            return builder->create_load(base);
        }
        // 普通标量变量：左值语境返回地址，否则取值
        if (context.is_lvalue)
            return base;
        return builder->create_load(base);
    }

    // 下标表达式一定在取值语境下求值
    bool old_is_lvalue = context.is_lvalue;
    context.is_lvalue = false;
    auto *index = node.expression->accept(*this);
    context.is_lvalue = old_is_lvalue;

    // 浮点下标需要转换成整型
    if (index->get_type()->is_float_type())
        index = builder->create_fptosi(index, INT32_T);

    // 负下标会终止程序：调用 neg_idx_except
    auto *is_negative = builder->create_icmp_lt(index, CONST_INT(0));
    auto *neg_bb = BasicBlock::create(module.get(), "", context.func);
    auto *ok_bb = BasicBlock::create(module.get(), "", context.func);
    builder->create_cond_br(is_negative, neg_bb, ok_bb);

    builder->set_insert_point(neg_bb);
    builder->create_call(scope.find("neg_idx_except"), {});
    builder->create_br(ok_bb);

    builder->set_insert_point(ok_bb);

    Value *addr;
    if (base_type->is_array_type()) {
        // 局部/全局数组：alloca 得到的是 [n x T]*
        addr = builder->create_gep(base, {CONST_INT(0), index});
    } else if (base_type->is_pointer_type()) {
        // 数组形参：先取出 T*，再进行下标寻址
        auto *array_ptr = builder->create_load(base);
        addr = builder->create_gep(array_ptr, {index});
    } else {
        // 非法程序（对标量取下标），按指针运算处理
        addr = builder->create_gep(base, {index});
    }

    if (context.is_lvalue)
        return addr;
    return builder->create_load(addr);
}//

Value* CminusfBuilder::visit(ASTAssignExpression &node) {
    // 先求出 var 所代表的地址（左值）
    bool old_is_lvalue = context.is_lvalue;
    context.is_lvalue = true;
    auto *addr = node.var->accept(*this);
    context.is_lvalue = old_is_lvalue;

    // 再对右侧表达式求值，必要时转换成左值的类型
    auto *val = node.expression->accept(*this);
    auto *lhs_type = addr->get_type()->get_pointer_element_type();
    if (lhs_type->is_float_type() && val->get_type()->is_int32_type())
        val = builder->create_sitofp(val, FLOAT_T);
    else if (lhs_type->is_int32_type() && val->get_type()->is_float_type())
        val = builder->create_fptosi(val, INT32_T);

    builder->create_store(val, addr);
    // 赋值表达式的值就是被存入的值
    return val;
}//

Value* CminusfBuilder::visit(ASTSimpleExpression &node) {
    auto *lhs = node.additive_expression_l->accept(*this);
    if (node.additive_expression_r == nullptr)
        return lhs;

    auto *rhs = node.additive_expression_r->accept(*this);

    // 混合类型运算时提升为 float
    bool is_float = lhs->get_type()->is_float_type() ||
                    rhs->get_type()->is_float_type();
    if (is_float) {
        if (lhs->get_type()->is_int32_type())
            lhs = builder->create_sitofp(lhs, FLOAT_T);
        if (rhs->get_type()->is_int32_type())
            rhs = builder->create_sitofp(rhs, FLOAT_T);
    }

    Value *cmp = nullptr;
    switch (node.op) {
    case OP_LE:
        cmp = is_float ? static_cast<Value *>(builder->create_fcmp_le(lhs, rhs))
                       : static_cast<Value *>(builder->create_icmp_le(lhs, rhs));
        break;
    case OP_LT:
        cmp = is_float ? static_cast<Value *>(builder->create_fcmp_lt(lhs, rhs))
                       : static_cast<Value *>(builder->create_icmp_lt(lhs, rhs));
        break;
    case OP_GT:
        cmp = is_float ? static_cast<Value *>(builder->create_fcmp_gt(lhs, rhs))
                       : static_cast<Value *>(builder->create_icmp_gt(lhs, rhs));
        break;
    case OP_GE:
        cmp = is_float ? static_cast<Value *>(builder->create_fcmp_ge(lhs, rhs))
                       : static_cast<Value *>(builder->create_icmp_ge(lhs, rhs));
        break;
    case OP_EQ:
        cmp = is_float ? static_cast<Value *>(builder->create_fcmp_eq(lhs, rhs))
                       : static_cast<Value *>(builder->create_icmp_eq(lhs, rhs));
        break;
    case OP_NEQ:
        cmp = is_float ? static_cast<Value *>(builder->create_fcmp_ne(lhs, rhs))
                       : static_cast<Value *>(builder->create_icmp_ne(lhs, rhs));
        break;
    }

    // 关系运算的结果是整型的 1 或 0
    return builder->create_zext(cmp, INT32_T);
}//

Value* CminusfBuilder::visit(ASTAdditiveExpression &node) {
    if (node.additive_expression == nullptr)
        return node.term->accept(*this);

    auto *lhs = node.additive_expression->accept(*this);
    auto *rhs = node.term->accept(*this);

    bool is_float = lhs->get_type()->is_float_type() ||
                    rhs->get_type()->is_float_type();
    if (is_float) {
        if (lhs->get_type()->is_int32_type())
            lhs = builder->create_sitofp(lhs, FLOAT_T);
        if (rhs->get_type()->is_int32_type())
            rhs = builder->create_sitofp(rhs, FLOAT_T);
    }

    if (node.op == OP_PLUS) {
        return is_float ? static_cast<Value *>(builder->create_fadd(lhs, rhs))
                        : static_cast<Value *>(builder->create_iadd(lhs, rhs));
    }
    return is_float ? static_cast<Value *>(builder->create_fsub(lhs, rhs))
                    : static_cast<Value *>(builder->create_isub(lhs, rhs));
}//

Value* CminusfBuilder::visit(ASTTerm &node) {
    if (node.term == nullptr)
        return node.factor->accept(*this);

    auto *lhs = node.term->accept(*this);
    auto *rhs = node.factor->accept(*this);

    bool is_float = lhs->get_type()->is_float_type() ||
                    rhs->get_type()->is_float_type();
    if (is_float) {
        if (lhs->get_type()->is_int32_type())
            lhs = builder->create_sitofp(lhs, FLOAT_T);
        if (rhs->get_type()->is_int32_type())
            rhs = builder->create_sitofp(rhs, FLOAT_T);
    }

    if (node.op == OP_MUL) {
        return is_float ? static_cast<Value *>(builder->create_fmul(lhs, rhs))
                        : static_cast<Value *>(builder->create_imul(lhs, rhs));
    }
    return is_float ? static_cast<Value *>(builder->create_fdiv(lhs, rhs))
                    : static_cast<Value *>(builder->create_isdiv(lhs, rhs));
}//

Value* CminusfBuilder::visit(ASTCall &node) {
    auto *callee = scope.find(node.id);
    auto *func_type = static_cast<Function *>(callee)->get_function_type();

    std::vector<Value *> args;
    for (unsigned int i = 0; i < node.args.size(); ++i) {
        auto *arg = node.args[i]->accept(*this);
        auto *param_type = func_type->get_param_type(i);
        // 实参类型与形参类型不一致时做隐式类型转换
        if (param_type->is_float_type() && arg->get_type()->is_int32_type())
            arg = builder->create_sitofp(arg, FLOAT_T);
        else if (param_type->is_int32_type() &&
                 arg->get_type()->is_float_type())
            arg = builder->create_fptosi(arg, INT32_T);
        args.push_back(arg);
    }
    return builder->create_call(callee, args);
}//
