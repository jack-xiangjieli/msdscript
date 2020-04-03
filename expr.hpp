#ifndef expr_hpp
#define expr_hpp

#include <string>

#include "pointer.hpp"
#include <iostream>


class Val;
class Env;

class Expr ENABLE_THIS(Expr){
public :
  virtual bool equals(PTR(Expr)) = 0;
  
  // To compute the number value of an expression,
  // assuming that all variables are 0
  virtual PTR(Val) interp(PTR(Env) env) = 0;
  
  // To substitute a number in place of a variable
  virtual PTR(Expr) subst(std::string var, PTR(Val) val) = 0;
  virtual bool containsVar() = 0;
  virtual PTR(Expr) optimize() = 0;
  virtual std::string to_string() = 0;
    virtual void step_interp() = 0;
};

class NumExpr : public Expr{
public:
    int rep;

    NumExpr(int rep);
    bool equals(PTR(Expr));
  
    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimize();
    std::string to_string();
    
    void step_interp();
};

class AddExpr : public Expr {
public:
  PTR(Expr) lhs;
  PTR(Expr) rhs;

  AddExpr(PTR(Expr) lhs, PTR(Expr) rhs);
  bool equals(PTR(Expr) e);

  PTR(Val) interp(PTR(Env) env);
  PTR(Expr) subst(std::string var, PTR(Val) val);
  bool containsVar();
  PTR(Expr) optimize();
  std::string to_string();
    
  void step_interp();
};

class MultExpr : public Expr {
public:
  PTR(Expr) lhs;
  PTR(Expr) rhs;

  MultExpr(PTR(Expr) lhs, PTR(Expr) rhs);
  bool equals(PTR(Expr) e);

  PTR(Val) interp(PTR(Env) env);
  PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimize();
    std::string to_string();
    
    void step_interp();
};

class VarExpr : public Expr {
public:
  std::string name;

  VarExpr(std::string name);
  bool equals(PTR(Expr) e);

  PTR(Val) interp(PTR(Env) env);
  PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimize();
    std::string to_string();
    
    void step_interp();
};

class BoolExpr : public Expr {
public:
    bool rep;
  
    BoolExpr(bool rep);
    bool equals(PTR(Expr) e);
  
    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimize();
    std::string to_string();
    
    void step_interp();
};

class LetExpr : public Expr {
public:
    std::string varStr;
    PTR(Expr) rhs;
    PTR(Expr) body;
    
    
    LetExpr(std::string varStr, PTR(Expr) rhs, PTR(Expr) body);
    bool equals(PTR(Expr) e);
    
    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimize();
    std::string to_string();
    
    void step_interp();
};


class IfExpr : public Expr {
public:
    PTR(Expr) condition;
    PTR(Expr) then_part;
    PTR(Expr) else_part;
    
    
    IfExpr(PTR(Expr) condition, PTR(Expr) then_part, PTR(Expr) else_part);
    bool equals(PTR(Expr) e);
    
    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimize();
    std::string to_string();
    
    void step_interp();
};



class CompareExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;

    
    CompareExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    bool equals(PTR(Expr) e);

    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimize();
    std::string to_string();
    
    void step_interp();
};


class FunExpr : public Expr {
public:
    std::string formal_arg;
    PTR(Expr) body;
    
    FunExpr(std::string formal_arg, PTR(Expr) body);
    bool equals(PTR(Expr) e);
    
    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimize();
    std::string to_string();
    
    void step_interp();
};

class CallExpr : public Expr {
public:
    PTR(Expr) to_be_called;
    PTR(Expr) actual_arg;
    
    CallExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg);
    bool equals(PTR(Expr) e);
    
    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimize();
    std::string to_string();
    
    void step_interp();
};


#endif /* expr_hpp */
