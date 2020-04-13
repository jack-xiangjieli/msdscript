#include "value.hpp"

#include <stdexcept>

#include "expr.hpp"
#include "env.hpp"
#include "cont.hpp"
#include "step.hpp"


NumVal::NumVal(int _rep) {
  rep = _rep;
}

bool NumVal::equals(PTR(Val) other_val) {
  PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
  if (other_num_val == nullptr)
    return false;
  else
    return rep == other_num_val->rep;
}

PTR(Val) NumVal::add_to(PTR(Val) other_val) {
  PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
  if (other_num_val == nullptr)
    throw std::runtime_error("not a number");
  else
      return NEW(NumVal)((unsigned) rep + (unsigned) other_num_val->rep);
}

PTR(Val) NumVal::mult_with(PTR(Val) other_val) {
  PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
  if (other_num_val == nullptr)
    throw std::runtime_error("not a number");
  else
      return NEW(NumVal)(rep * other_num_val->rep);
}

PTR(Expr) NumVal::to_expr() {
  return NEW(NumExpr)(rep);
}

std::string NumVal::to_string() {
  return std::to_string(rep);
}

bool NumVal::is_true() {
    if (rep == 0) return false;
    else return true;
}

PTR(Val) NumVal::call(PTR(Val) actual_arg) {
    return NULL;
}

void NumVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    
}

//=======================================================================



BoolVal::BoolVal(bool _rep) {
  rep = _rep;
}

bool BoolVal::equals(PTR(Val) other_val) {
    
  PTR(BoolVal) other_bool_val = CAST(BoolVal)(other_val);
  if (other_bool_val == nullptr)
    return false;
  else
    return rep == other_bool_val->rep;
}

PTR(Val) BoolVal::add_to(PTR(Val) other_val) {
  throw std::runtime_error("no adding booleans");
}

PTR(Val) BoolVal::mult_with(PTR(Val) other_val) {
  throw std::runtime_error("no multiplying booleans");
}

PTR(Expr) BoolVal::to_expr() {
  return NEW(BoolExpr)(rep);
}

std::string BoolVal::to_string() {
    return rep ? "_true" : "_false";
}

bool BoolVal::is_true() {
    return rep;
}

PTR(Val) BoolVal::call(PTR(Val) actual_arg) {
    return NULL;
}

void BoolVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    
}

//============================================================


FunVal::FunVal(std::string _formal_arg, PTR(Expr) _body, PTR(Env) _env) {
    formal_arg = _formal_arg;
    body = _body;
    env = _env;
}

bool FunVal::equals(PTR(Val) val) {
    PTR(FunVal) fv = CAST(FunVal)(val);
    if (fv == NULL)
        return false;
    return fv->formal_arg == formal_arg && fv->body->equals(body);
}

PTR(Val) FunVal::add_to(PTR(Val) other_val) {
    return body->interp(env)->add_to(other_val);
}

PTR(Val) FunVal::mult_with(PTR(Val) other_val) {
    return body->interp(env)->mult_with(other_val);
}

PTR(Expr) FunVal::to_expr() {
    return NEW(FunExpr)(formal_arg, body);
}

std::string FunVal::to_string() {
    return "_fun (" + formal_arg + ") " + body->to_string();
}

bool FunVal::is_true() {
    return false;
}

PTR(Val) FunVal::call(PTR(Val) actual_arg) {
    return body -> interp(NEW(ExtendedEnv)(env, formal_arg, actual_arg));
}


void FunVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    Step::mode = Step::interp_mode;
    Step::expr = body;
    Step::env = NEW(ExtendedEnv)(env, formal_arg, actual_arg_val);
    Step::cont = rest;
}







