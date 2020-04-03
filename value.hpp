#ifndef value_hpp
#define value_hpp

#include "pointer.hpp"
#include <string>






/* A forward declaration, so `Val` can refer to `Expr`, while
   `Expr` still needs to refer to `Val`. */
class Expr;
class Env;
class Cont;

class Val ENABLE_THIS(Val) {
public :
  virtual bool equals(PTR(Val) val) = 0;
  virtual PTR(Val) add_to(PTR(Val) other_val) = 0;
  virtual PTR(Val) mult_with(PTR(Val) other_val) = 0;
  virtual PTR(Expr) to_expr() = 0;
  virtual std::string to_string() = 0;
  virtual bool is_true() = 0;
  virtual PTR(Val) call(PTR(Val) actual_arg) = 0;
  virtual void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) = 0;
};

class NumVal : public Val {
public:
  int rep;
  NumVal(int rep);
  bool equals(PTR(Val) val);

  PTR(Val) add_to(PTR(Val) other_val);
  PTR(Val) mult_with(PTR(Val) other_val);
  PTR(Expr) to_expr();
  std::string to_string();
  bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
};

class BoolVal : public Val {
public:
  bool rep;
  BoolVal(bool rep);
  bool equals(PTR(Val) val);

  PTR(Val) add_to(PTR(Val) other_val);
  PTR(Val) mult_with(PTR(Val) other_val);
  PTR(Expr) to_expr();
  std::string to_string();
  bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
};

class FunVal : public Val {
public:
    std::string formal_arg;
    PTR(Expr) body;
    PTR(Env) env;
    FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env);
    bool equals(PTR(Val) val);
    
    PTR(Val) add_to(PTR(Val) other_val);
    PTR(Val) mult_with(PTR(Val) other_val);
    PTR(Expr) to_expr();
    std::string to_string();
    bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
};

#endif /* value_hpp */
