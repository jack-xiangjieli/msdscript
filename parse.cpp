#include <iostream>
#include <sstream>
#include "parse.hpp"
#include "env.hpp"
#include "expr.hpp"
#include "value.hpp"
#include "step.hpp"

static PTR(Expr) parse_expr(std::istream &in);
static PTR(Expr) parse_comparg(std::istream &in);
static PTR(Expr) parse_addend(std::istream &in);
static PTR(Expr) parse_multicand(std::istream &in);
static PTR(Expr) parse_inner(std::istream &in);
static PTR(Expr) parse_number(std::istream &in);
static PTR(Expr) parse_negative_number(std::istream &in);
static PTR(Expr) parse_variable(std::istream &in);
static std::string parse_keyword(std::istream &in);
static std::string parse_alphabetic(std::istream &in, std::string prefix);
static PTR(Expr) parse_let(std::istream &in);
static PTR(Expr) parse_if(std::istream &in);
static PTR(Expr) parse_fun(std::istream &in);
static char peek_after_spaces(std::istream &in);

// Take an input stream that contains an expression,
// and returns the parsed representation of that expression.
// Throws `runtime_error` for parse errors.
PTR(Expr) parse(std::istream &in) {
  PTR(Expr) e = parse_expr(in);
  
  // This peek is currently redundant, since we would have
  // consumed whitespace to decide that the expression
  // doesn't continue.
  char c = peek_after_spaces(in);
  if (!in.eof())
    throw std::runtime_error((std::string)"expected end of file at " + c);
  
  return e;
}

// Takes an input stream that starts with an expression,
// consuming the largest initial expression possible.

static PTR(Expr) parse_expr(std::istream &in) {
    PTR(Expr) e = parse_comparg(in);
    
    char c = peek_after_spaces(in);
    if (c == '=') {
        in >> c;
        char c1 = peek_after_spaces(in);
        if (c1 == '=') {
            in >> c1;
            PTR(Expr) rhs = parse_expr(in);
            e = NEW(CompareExpr)(e, rhs);
        }
    }
    
    return e;
}


static PTR(Expr) parse_comparg(std::istream &in) {
  PTR(Expr) e = parse_addend(in);
  
  char c = peek_after_spaces(in);
  if (c == '+') {
    in >> c;
    PTR(Expr)rhs = parse_comparg(in);
    e = NEW(AddExpr)(e, rhs);
  }
  
  return e;
}

// Takes an input stream that starts with an addend,
// consuming the largest initial addend possible, where
// an addend is an expression that does not have `+`
// except within nested expressions (like parentheses).
static PTR(Expr) parse_addend(std::istream &in) {
  PTR(Expr) e = parse_multicand(in);
  
  char c = peek_after_spaces(in);
  if (c == '*') {
    c = in.get();
    PTR(Expr) rhs = parse_addend(in);
    e = NEW(MultExpr)(MultExpr(e, rhs));
  }
  
  return e;
}


static PTR(Expr) parse_multicand(std::istream &in) {
    PTR(Expr) e = parse_inner(in);
    while (peek_after_spaces(in) == '(') {
        PTR(Expr) rhs = parse_inner(in);
        e = NEW(CallExpr)(CallExpr(e, rhs));
    }
    
    return e;
}

// Parses something with no immediate `+` or `*` from `in`.
static PTR(Expr) parse_inner(std::istream &in) {
  PTR(Expr) e;

  char c = peek_after_spaces(in);
  
  if (c == '(') {
    c = in.get();
    e = parse_expr(in);
    c = peek_after_spaces(in);
    if (c == ')')
      c = in.get();
    else
      throw std::runtime_error("expected a close parenthesis");
  } else if (c == '-') {
      e = parse_negative_number(in);
  } else if (isdigit(c)) {
    e = parse_number(in);
  } else if (isalpha(c)) {
    e = parse_variable(in);
  } else if (c == '_') {
    std::string keyword = parse_keyword(in);
    if (keyword == "_true")
      return NEW(BoolExpr)(true);
    else if (keyword == "_false")
      return NEW(BoolExpr)(false);
    else if (keyword == "_let")
        e = parse_let(in);
    else if (keyword == "_if")
        e = parse_if(in);
    else if (keyword == "_fun")
        e = parse_fun(in);
    else
      throw std::runtime_error((std::string)"unexpected keyword " + keyword);
  } else {
    throw std::runtime_error((std::string)"expected a digit or open parenthesis at " + c);
  }
  
  return e;
}

// Parses a number, assuming that `in` starts with a digit.
static PTR(Expr) parse_number(std::istream &in) {
  int num = 0;
  in >> num;
  return NEW(NumExpr)(num);
}

static PTR(Expr) parse_negative_number(std::istream &in) {
    in.get();
    int num = 0;
    in >> num;
    return NEW(NumExpr)(-num);
}

// Parses an expression, assuming that `in` starts with a
// letter.
static PTR(Expr) parse_variable(std::istream &in) {
  return NEW(VarExpr)(parse_alphabetic(in, ""));
}

// Parses an expression, assuming that `in` starts with a
// letter.
static std::string parse_keyword(std::istream &in) {
  in.get(); // consume `_`
  return parse_alphabetic(in, "_");
}

// Parses an expression, assuming that `in` starts with a
// letter.
static std::string parse_alphabetic(std::istream &in, std::string prefix) {
  std::string name = prefix;
  while (1) {
    char c = in.peek();
    if (!isalpha(c))
      break;
    name += in.get();
  }
  return name;
}

static PTR(Expr) parse_let(std::istream &in) {
    PTR(Expr) rhs;
    PTR(Expr) body;
    
    // get variable string
    peek_after_spaces(in);  // skip the blank space
    std::string varStr = parse_alphabetic(in, "");
    
    // get rhs expression
    peek_after_spaces(in);   // skip the blank space
    in.get();   //consume
    rhs = parse_expr(in);
    
    // get body expression
    peek_after_spaces(in);
    in.get();   //consume
    std::string inString = parse_alphabetic(in, "_");
    if (inString == "_in")
        body = parse_expr(in);
    else
        throw std::runtime_error((std::string)"expect a _in in this expression ");
    
    return NEW(LetExpr)(varStr, rhs, body);
}

static PTR(Expr) parse_if(std::istream &in) {
    PTR(Expr) condition;
    PTR(Expr) then_part;
    PTR(Expr) else_part;
    
    // parse the condition
    peek_after_spaces(in);  // skip the blank space
    condition = parse_expr(in);
    
    // parse the then_part
    peek_after_spaces(in);
    in.get();   //consume
    std::string thenString = parse_alphabetic(in, "_");
    if (thenString == "_then")
        then_part = parse_expr(in);
    else
        throw std::runtime_error((std::string)"expect _then in this expression ");
    
    // parse the else_part
    peek_after_spaces(in);
    in.get();   //consume
    std::string elseString = parse_alphabetic(in, "_");
    if (elseString == "_else")
        else_part = parse_expr(in);
    else
        throw std::runtime_error((std::string)"expect _then in this expression ");
    
    return NEW(IfExpr)(IfExpr(condition, then_part, else_part));
}

static PTR(Expr) parse_fun(std::istream &in) {
    std::string formal_arg;
    PTR(Expr) body;
    
    peek_after_spaces(in);  // skip the blank space
    formal_arg = parse_expr(in)->to_string();
    
    peek_after_spaces(in);
    body = parse_expr(in);
    return NEW(FunExpr)(FunExpr(formal_arg, body));
}



// Like in.peek(), but consume an whitespace at the
// start of `in`
static char peek_after_spaces(std::istream &in) {
  char c;
  while (1) {
    c = in.peek();
    if (!isspace(c))
      break;
    c = in.get();
  }
  return c;
}

/* for tests */
static PTR(Expr) parse_str(std::string s) {
  std::istringstream in(s);
  return parse(in);
}


std::string interp(std::string s) {
    PTR(Expr) e = parse_str(s);
    return e -> interp(Env::empty) -> to_string();
}

std::string stepInterp(std::string s) {
    PTR(Expr) e = parse_str(s);
    return Step::interp_by_steps(e) -> to_string();
}

std::string optimize(std::string s) {
    PTR(Expr) e = parse_str(s);
    return e -> optimize() -> to_string();
}

bool equals(std::string s1, std::string s2) {
    return parse_str(s1) -> equals(parse_str(s2));
}


/* for tests */
static std::string parse_str_error(std::string s) {
  std::istringstream in(s);
  try {
    (void)parse(in);
    return "";
  } catch (std::runtime_error exn) {
    return exn.what();
  }
}
