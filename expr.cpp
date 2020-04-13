#include "expr.hpp"
#include "value.hpp"
#include "env.hpp"
#include "step.hpp"
#include "cont.hpp"

NumExpr::NumExpr(int _rep) {
  rep = _rep;
}

bool NumExpr::equals(PTR(Expr) e) {
  PTR(NumExpr) n = CAST(NumExpr)(e);
  if (n == NULL)
    return false;
  else
    return rep == n->rep;
}

PTR(Val) NumExpr::interp(PTR(Env) env) {
  return NEW(NumVal)(rep);
}

PTR(Expr) NumExpr::subst(std::string var, PTR(Val) new_val) {
  return NEW(NumExpr)(rep);
}

bool NumExpr::containsVar() {
    return false;
}

PTR(Expr) NumExpr::optimize() {
    return NEW(NumExpr)(rep);
}

std::string NumExpr::to_string() {
    return std::to_string(rep);
}

void NumExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(NumVal)(rep);
}


//=====================================================

AddExpr::AddExpr(PTR(Expr) _lhs, PTR(Expr) _rhs) {
  lhs = _lhs;
  rhs = _rhs;
}

bool AddExpr::equals(PTR(Expr) e) {
  PTR(AddExpr) a = CAST(AddExpr)(e);
  if (a == NULL)
    return false;
  else
    return (lhs->equals(a->lhs)
            && rhs->equals(a->rhs));
}

PTR(Val) AddExpr::interp(PTR(Env) env) {
//    return lhs->interp(env)->add_to(rhs->interp(env));
    
    PTR(Val) lhs_val = lhs -> interp(env);
    PTR(Val) rhs_val = rhs -> interp(env);
    return lhs_val -> add_to(rhs_val);
}



PTR(Expr) AddExpr::subst(std::string var, PTR(Val) new_val) {
    return NEW(AddExpr)(lhs->subst(var, new_val),
                                rhs->subst(var, new_val));
}

bool AddExpr::containsVar() {
    return lhs -> containsVar() || rhs -> containsVar();
}

PTR(Expr) AddExpr::optimize() {
    PTR(Expr) temp_lhs = lhs -> optimize();
    PTR(Expr) temp_rhs = rhs -> optimize();
    if (!temp_lhs -> containsVar() && !temp_rhs -> containsVar()) {
        return NEW(NumExpr)((CAST(NumExpr)(temp_lhs) -> rep )
                            + (CAST(NumExpr)(temp_rhs) -> rep));
    }
    return NEW(AddExpr)(temp_lhs, temp_rhs);
}


std::string AddExpr::to_string() {
    return lhs -> to_string() + " + " + rhs -> to_string();
}


void AddExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env;
    
    Step::cont = NEW(RightThenAddCont)(rhs, Step::env, Step::cont);
}


//=====================================================

MultExpr::MultExpr(PTR(Expr) _lhs, PTR(Expr) _rhs) {
  lhs = _lhs;
  rhs = _rhs;
}

bool MultExpr::equals(PTR(Expr) e) {
  PTR(MultExpr)m = CAST(MultExpr)(e);
  if (m == NULL)
    return false;
  else
    return (lhs->equals(m->lhs)
            && rhs->equals(m->rhs));
}

PTR(Val) MultExpr::interp(PTR(Env) env) {
  return lhs->interp(env)->mult_with(rhs->interp(env));
}

PTR(Expr) MultExpr::subst(std::string var, PTR(Val) new_val)
{
    return NEW(MultExpr)(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

bool MultExpr::containsVar() {
    return lhs -> containsVar() || rhs -> containsVar();
}


PTR(Expr) MultExpr::optimize() {
    PTR(Expr) temp_lhs = lhs -> optimize();
    PTR(Expr) temp_rhs = rhs -> optimize();
    if (!temp_lhs -> containsVar() && !temp_rhs -> containsVar()) {
        return NEW(NumExpr)((CAST(NumExpr)(temp_lhs) -> rep )
                            * (CAST(NumExpr)(temp_rhs) -> rep));
    }
    return NEW(MultExpr)(temp_lhs, temp_rhs);
}


std::string MultExpr::to_string() {
    return lhs -> to_string() + " * " + rhs -> to_string();
}

void MultExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env;
    
    Step::cont = NEW(RightThenMultCont)(rhs, Step::env, Step::cont);
    
}

//=====================================================


VarExpr::VarExpr(std::string _name) {
  name = _name;
}

bool VarExpr::equals(PTR(Expr) e) {
  PTR(VarExpr) v = CAST(VarExpr)(e);
  if (v == NULL)
    return false;
  else
    return name == v->name;
}

PTR(Val) VarExpr::interp(PTR(Env) env) {
    return env -> lookup(name);
//  throw std::runtime_error("can not interpret variable");
}

PTR(Expr) VarExpr::subst(std::string var, PTR(Val) new_val) {
  if (name == var)
    return new_val->to_expr();
  else
    return NEW(VarExpr)(name);
}

bool VarExpr::containsVar() {
    return true;
}


PTR(Expr) VarExpr::optimize() {
    return NEW(VarExpr)(name);
}


std::string VarExpr::to_string() {
    return name;
}

void VarExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = Step::env -> lookup(name);
}


//=====================================================

BoolExpr::BoolExpr(bool _rep) {
  rep = _rep;
}

bool BoolExpr::equals(PTR(Expr) e) {
  PTR(BoolExpr) b = CAST(BoolExpr)(e);
  if (b == NULL)
    return false;
  else
    return rep == b->rep;
}

PTR(Val) BoolExpr::interp(PTR(Env) env) {
  return NEW(BoolVal)(rep);
}

PTR(Expr) BoolExpr::subst(std::string var, PTR(Val) new_val) {
  return NEW(BoolExpr)(rep);
}

bool BoolExpr::containsVar() {
    return true;
}

PTR(Expr) BoolExpr::optimize() {
    return NEW(BoolExpr)(rep);
}


std::string BoolExpr::to_string() {
    return rep ? "_true" : "_false";
}


void BoolExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(BoolVal)(rep);
}

//=====================================================

LetExpr::LetExpr(std::string _varStr, PTR(Expr) _rhs, PTR(Expr) _body) {
    varStr = _varStr;
    rhs = _rhs;
    body = _body;
}

bool LetExpr::equals(PTR(Expr) e) {
    PTR(LetExpr) l = CAST(LetExpr)(e);
    if (l == NULL)
        return false;
    else return (l->varStr == varStr && l->rhs -> equals(rhs)) && l->body -> equals(body);
}

PTR(Expr) LetExpr::subst(std::string var, PTR(Val) val) {
    if (var == varStr)
        return NEW(LetExpr)(var, rhs, body);
    else return NEW(LetExpr)(varStr, rhs-> subst(var, val), body -> subst(var, val));
}



PTR(Val) LetExpr::interp(PTR(Env) env) {
    PTR(Val) rhs_val = rhs -> interp(env);
    PTR(Env) new_env = NEW(ExtendedEnv) (env, varStr, rhs_val);
    return body -> interp(new_env);
}


PTR(Expr) LetExpr::optimize() {
    PTR(Expr) temp_rhs = rhs -> optimize();
    PTR(Expr) temp_body = body -> optimize();
    if (!temp_rhs -> containsVar()) {
        PTR(Val) rhs_val = NEW(NumVal)(CAST(NumExpr)(temp_rhs)->rep);
        return temp_body -> subst(varStr, rhs_val) -> optimize();
    }
    return NEW(LetExpr)(varStr, temp_rhs, temp_body);
}

bool LetExpr::containsVar() {
    return NEW(LetExpr)(varStr, rhs, body) -> optimize() -> containsVar();
}


std::string LetExpr::to_string() {
    return "_let " + varStr + " = " + rhs -> to_string() + " _in " + body -> to_string();
}


void LetExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = Step::env;
    
    Step::cont = NEW(LetBodyCont)(varStr, body, Step::env, Step::cont);
}



//==========================================================================


IfExpr::IfExpr(PTR(Expr) _condition, PTR(Expr) _then_part, PTR(Expr) _else_part) {
    condition = _condition;
    then_part = _then_part;
    else_part = _else_part;
}


bool IfExpr::equals(PTR(Expr) e) {
    PTR(IfExpr) lhs = CAST(IfExpr)(e);
    if (lhs == NULL)
        return false;
    else return (lhs->condition->equals(condition) && lhs->then_part -> equals(then_part)) && lhs->else_part -> equals(else_part);
    
}

PTR(Val) IfExpr::interp(PTR(Env) env) {
    if (condition -> interp(env) -> is_true())
        return then_part -> interp(env);
    else
        return else_part -> interp(env);
}


PTR(Expr) IfExpr::subst(std::string _var, PTR(Val) val) {
    return NEW(IfExpr)(condition->subst(_var, val), then_part->subst(_var, val), else_part->subst(_var, val));
}


bool IfExpr::containsVar() {
    return true;
}


PTR(Expr) IfExpr::optimize() {
    PTR(BoolExpr) temp_condition = CAST(BoolExpr)(condition -> optimize());
    if (temp_condition == NULL) {
        return NEW(IfExpr)(condition, then_part, else_part);
    }
    else return (temp_condition -> rep) ? then_part->optimize() : else_part->optimize();
}


std::string IfExpr::to_string() {
    return "_if " + condition->to_string() + " _then " + then_part -> to_string() + " _else " + else_part -> to_string();
}


void IfExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = condition;
    Step::env = Step::env;
    
    Step::cont = NEW(IfBranchCont)(then_part, else_part, Step::env, Step::cont);
    
}


//============================================================

CompareExpr::CompareExpr(PTR(Expr) _lhs, PTR(Expr) _rhs) {
    lhs = _lhs;
    rhs = _rhs;
}

bool CompareExpr::equals(PTR(Expr) e) {
    PTR(CompareExpr) ce = CAST(CompareExpr)(e);
    if (ce == NULL) {
        return false;
    }
    else
        return lhs->equals(ce->lhs) && rhs->equals(ce->rhs);
}

PTR(Val) CompareExpr::interp(PTR(Env) env) {
    
    if (lhs->interp(env)->equals(rhs->interp(env)))
        return NEW(BoolVal)(true);
    else return NEW(BoolVal)(false);
}

PTR(Expr) CompareExpr::subst(std::string var, PTR(Val) val) {
    return NEW(CompareExpr)(lhs->subst(var, val), rhs->subst(var, val));
}

bool CompareExpr::containsVar() {
    return true;
}

PTR(Expr) CompareExpr::optimize() {
    PTR(Expr) temp_lhs = lhs->optimize();
    PTR(Expr) temp_rhs = rhs->optimize();
    
    if (!temp_lhs -> containsVar() && !temp_rhs -> containsVar()) {
        if (temp_lhs -> equals(temp_rhs))
            return NEW(BoolExpr)(true);
        else return NEW(BoolExpr)(false);
        
    }
    return NEW(CompareExpr)(temp_lhs, temp_rhs);
}

std::string CompareExpr::to_string() {
    return lhs->to_string() + " == " + rhs->to_string();
}


void CompareExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env;
    
    Step::cont = NEW(RightThenCompCont)(rhs, Step::env, Step::cont);
}


//=============================================================

FunExpr::FunExpr(std::string _formal_arg, PTR(Expr) _body) {
    formal_arg = _formal_arg;
    body = _body;
}

bool FunExpr::equals(PTR(Expr) e) {
    PTR(FunExpr) l = CAST(FunExpr)(e);
    if (l == NULL)
        return false;
    return l->formal_arg == formal_arg && l->body->equals(body);
}

PTR(Val) FunExpr::interp(PTR(Env) env) {
    return NEW(FunVal)(formal_arg, body, env);
}

PTR(Expr) FunExpr::subst(std::string var, PTR(Val) val) {
    if (var == formal_arg)
        return NEW(FunExpr)(formal_arg, body);
    else return NEW(FunExpr)(formal_arg, body->subst(var, val));
}

bool FunExpr::containsVar() {
    return true;
}

PTR(Expr) FunExpr::optimize() {
    body = body->optimize();
    return NEW(FunExpr)(formal_arg, body);
}

std::string FunExpr::to_string() {
    return "_fun (" + formal_arg + ") " + body->to_string();
}


void FunExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(FunVal)(formal_arg, body, Step::env);
    
}


//============================================================



CallExpr::CallExpr(PTR(Expr) _to_be_called, PTR(Expr) _actual_arg) {
    to_be_called = _to_be_called;
    actual_arg = _actual_arg;
}

bool CallExpr::equals(PTR(Expr) e) {
    PTR(CallExpr) l = CAST(CallExpr)(e);
    if (l == NULL)
        return false;
    return l->to_be_called->equals(to_be_called) && l->actual_arg->equals(actual_arg);
}

PTR(Val) CallExpr::interp(PTR(Env) env) {
    return to_be_called->interp(env)->call(actual_arg->interp(env));
}

PTR(Expr) CallExpr::subst(std::string var, PTR(Val) val) {
    return NEW(CallExpr)(to_be_called->subst(var, val), actual_arg->subst(var, val));
}

bool CallExpr::containsVar() {
    return to_be_called->containsVar();
}

PTR(Expr) CallExpr::optimize() {
//    to_be_called = to_be_called->optimize();
//    actual_arg = actual_arg->optimize();
    return NEW(CallExpr)(to_be_called, actual_arg);
}

std::string CallExpr::to_string() {
    return to_be_called->to_string() + "(" + actual_arg->to_string() + ")";
}


void CallExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = to_be_called;
    Step::env = Step::env;
    
    Step::cont = NEW(ArgThenCallCont)(actual_arg, Step::env, Step::cont);
    
}

//=============================================================

