//
//  cont.cpp
//  msdscript
//
//  Created by xiangjieli on 3/23/20.
//  Copyright © 2020 xiangjieli. All rights reserved.
//

#include "cont.hpp"
#include "step.hpp"
#include "value.hpp"
#include "env.hpp"

PTR(Cont) Cont::done = NEW(DoneCont)();

//=============================================================

DoneCont::DoneCont() { }

void DoneCont::step_continue() {
    throw std::runtime_error("can't continue done");
}

//==============================================================


RightThenAddCont::RightThenAddCont(PTR(Expr) _rhs, PTR(Env) _env, PTR(Cont) _rest) {
    rhs = _rhs;
    env = _env;
    rest = _rest;
    
}

void RightThenAddCont::step_continue() {
    PTR(Val) lhs_val = Step::val;
    
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = Step::env;
    Step::cont = NEW(AddCont)(lhs_val, rest);
}

//==============================================================

AddCont::AddCont(PTR(Val) _lhs_val, PTR(Cont) _rest) {
    lhs_val = _lhs_val;
    rest = _rest;
}

void AddCont::step_continue() {
    PTR(Val) rhs_val = Step::val;
    
    Step::mode = Step::continue_mode;
    Step::val = lhs_val -> add_to(rhs_val);
    Step::cont = rest;
}

//==============================================================


RightThenMultCont::RightThenMultCont(PTR(Expr) _rhs, PTR(Env) _env, PTR(Cont) _rest) {
    rhs = _rhs;
    env = _env;
    rest = _rest;
    
}

void RightThenMultCont::step_continue() {
    PTR(Val) lhs_val = Step::val;
    
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = Step::env;
    Step::cont = NEW(MultCont)(lhs_val, rest);
}


//==============================================================

MultCont::MultCont(PTR(Val) _lhs_val, PTR(Cont) _rest) {
    lhs_val = _lhs_val;
    rest = _rest;
}

void MultCont::step_continue() {
    PTR(Val) rhs_val = Step::val;
    
    Step::mode = Step::continue_mode;
    Step::val = lhs_val -> mult_with(rhs_val);
    Step::cont = rest;
}


//==============================================================

LetBodyCont::LetBodyCont(std::string _varStr, PTR(Expr) _body, PTR(Env) _env, PTR(Cont) _rest) {
    varStr = _varStr;
    body = _body;
    env = _env;
    rest = _rest;
}

void LetBodyCont::step_continue() {
    Step::mode = Step::interp_mode;
    Step::expr = body;
    Step::env = NEW(ExtendedEnv)(env, varStr, Step::val);
    
    Step::cont = rest;
}

//==============================================================

IfBranchCont::IfBranchCont(PTR(Expr) _then_part, PTR(Expr) _else_part, PTR(Env) _env, PTR(Cont) _rest) {
    then_part = _then_part;
    else_part = _else_part;
    env = _env;
    rest = _rest;
}


void IfBranchCont::step_continue() {
    PTR(Val) condition_val = Step::val;
    Step::mode = Step::interp_mode;
    if (condition_val -> is_true()) {
        Step::expr = then_part;
    } else {
        Step::expr = else_part;
    }
    Step::env = env;
    Step::cont = rest;
}

//==============================================================


ArgThenCallCont::ArgThenCallCont(PTR(Expr) _actual_arg, PTR(Env) _env, PTR(Cont) _rest) {
    actual_arg = _actual_arg;
    env = _env;
    rest = _rest;
}

void ArgThenCallCont::step_continue() {
    Step::mode = Step::interp_mode;
    Step::expr = actual_arg;
    Step::env = env;
    
    Step::cont = NEW(CallCont)(Step::val, rest);
}


//==============================================================



CallCont::CallCont(PTR(Val) _to_be_called_val, PTR(Cont) _rest) {
    to_be_called_val = _to_be_called_val;
    rest = _rest;
}

void CallCont::step_continue() {
    to_be_called_val -> call_step(Step::val, rest);
    
}

//==============================================================

RightThenCompCont::RightThenCompCont(PTR(Expr) _rhs, PTR(Env) _env, PTR(Cont) _rest) {
    rhs = _rhs;
    env = _env;
    rest = _rest;
}


void RightThenCompCont::step_continue() {
    PTR(Val) lhs_val = Step::val;
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = env;
    
    Step::cont = NEW(CompCont)(lhs_val, rest);
}

//==============================================================

CompCont::CompCont(PTR(Val) _lhs_val, PTR(Cont) _rest) {
    lhs_val = _lhs_val;
    rest = _rest;
}


void CompCont::step_continue() {
    PTR(Val) rhs_val = Step::val;
    
    Step::mode = Step::continue_mode;
    if (lhs_val -> equals(rhs_val)) {
        Step::val = NEW(BoolVal)(true);
    } else {
        Step::val = NEW(BoolVal)(false);
    }
    
    Step::cont = rest;
    
}

