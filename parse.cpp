#include <iostream>
#include <sstream>
#include "parse.hpp"
#include "catch.hpp"
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
static std::string interp(std::string s);
static std::string stepInterp(std::string s);
static std::string optimize(std::string s);
static bool equals(std::string s1, std::string s2);

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


static std::string interp(std::string s) {
    PTR(Expr) e = parse_str(s);
    return e -> interp(Env::empty) -> to_string();
}

static std::string stepInterp(std::string s) {
    PTR(Expr) e = parse_str(s);
    return Step::interp_by_steps(e) -> to_string();
}

static std::string optimize(std::string s) {
    PTR(Expr) e = parse_str(s);
    return e -> optimize() -> to_string();
}

static bool equals(std::string s1, std::string s2) {
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

TEST_CASE( "See if this works" ) {
  PTR(Expr) ten_plus_one = NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr)(1));
  
  CHECK ( parse_str_error(" ( 1 ") == "expected a close parenthesis" );

  CHECK( parse_str("10")->equals(NEW(NumExpr)(10)) );
  CHECK( parse_str("(10)")->equals(NEW(NumExpr)(10)) );
  CHECK( parse_str("10+1")->equals(ten_plus_one) );
  CHECK( parse_str("(10+1)")->equals(ten_plus_one) );
  CHECK( parse_str("(10)+1")->equals(ten_plus_one) );
  CHECK( parse_str("10+(1)")->equals(ten_plus_one) );
  CHECK( parse_str("1+2*3")->equals(NEW(AddExpr)(NEW(NumExpr)(1),
                                            NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
  CHECK( parse_str("1*2+3")->equals(NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)),
                                            NEW(NumExpr)(3))) );
  CHECK( parse_str("4*2*3")->equals(NEW(MultExpr)(NEW(NumExpr)(4),
                                             NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
  CHECK( parse_str("4+2+3")->equals(NEW(AddExpr)(NEW(NumExpr)(4),
                                            NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
  CHECK( parse_str("4*(2+3)")->equals(NEW(MultExpr)(NEW(NumExpr)(4),
                                               NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
  CHECK( parse_str("(2+3)*4")->equals(NEW(MultExpr)(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)),
                                               NEW(NumExpr)(4))) );
  CHECK( parse_str("xyz")->equals(NEW(VarExpr)("xyz")) );
  CHECK( parse_str("xyz+1")->equals(NEW(AddExpr)(NEW(VarExpr)("xyz"), NEW(NumExpr)(1))) );

  CHECK ( parse_str_error("!") == "expected a digit or open parenthesis at !" );
  CHECK ( parse_str_error("(1") == "expected a close parenthesis" );

  CHECK( parse_str(" 10 ")->equals(NEW(NumExpr)(10)) );
  CHECK( parse_str(" (  10 ) ")->equals(NEW(NumExpr)(10)) );
  CHECK( parse_str(" 10  + 1")->equals(ten_plus_one) );
  CHECK( parse_str(" ( 10 + 1 ) ")->equals(ten_plus_one) );
  CHECK( parse_str(" 11 * ( 10 + 1 ) ")->equals(NEW(MultExpr)(NEW(NumExpr)(11),
                                                         ten_plus_one)) );
  CHECK( parse_str(" ( 11 * 10 ) + 1 ")
        ->equals(NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(11), NEW(NumExpr)(10)),
                         NEW(NumExpr) (1))) );
  CHECK( parse_str(" 1 + 2 * 3 ")
        ->equals(NEW(AddExpr)(NEW(NumExpr)(1),
                         NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );

  CHECK( parse_str_error(" ! ") == "expected a digit or open parenthesis at !" );
  CHECK( parse_str_error(" ( 1 ") == "expected a close parenthesis" );
  CHECK( parse_str_error(" 1 )") == "expected end of file at )" );
  
  CHECK( parse_str("_true")->equals(NEW(BoolExpr)(true)) );
  CHECK( parse_str("_false")->equals(NEW(BoolExpr)(false)) );
  
  CHECK( parse_str_error("_maybe ") == "unexpected keyword _maybe" );
    
    
    CHECK( (parse_str("3 + (2 * 4)") -> optimize() -> to_string()) == "11" );
    CHECK( (parse_str("x + (2 * 4)") -> optimize() -> to_string()) == "x + 8" );
    CHECK( (parse_str("x + y + z") -> optimize() -> to_string()) == "x + y + z" );
    CHECK( (parse_str("_let x = 2 _in x + 8")) -> optimize() -> equals( NEW(NumExpr)(10)) );
    CHECK( (parse_str("_let x = 2 _in x + 8")) -> optimize() -> to_string() == "10" );
    CHECK( (parse_str("_let x = y + 4 _in x + 8")) -> optimize() -> to_string() == "_let x = y + 4 _in x + 8" );
    CHECK( (parse_str("_let y = z + 2 _in x + y + (2 * 3)")) -> optimize() -> to_string() == "_let y = z + 2 _in x + y + 6" );
    
    CHECK( (parse_str("_let x = 5 _in _let y = z + 2 _in x + y + (2 * 3)")) -> optimize() -> to_string()
          == "_let y = z + 2 _in 5 + y + 6");
    
    PTR(EmptyEnv) emptyenv = NEW(EmptyEnv)();
    
    CHECK( (parse_str("_let x = 2 _in x + 8")) -> interp(emptyenv) -> to_string() == "10" );
    
    
    CHECK( (parse_str("_if _true _then 1 _else 2")) -> interp(emptyenv) -> equals(NEW(NumVal)(1)) );
    
    CHECK( (parse_str("_if 1 == 1 _then 1 _else 2")) -> interp(emptyenv) -> equals(NEW(NumVal)(1)) );
    CHECK( (parse_str("_if 1 == 9 _then 1 _else 2")) -> interp(emptyenv) -> equals(NEW(NumVal)(2)) );
    CHECK( (parse_str("_if 1 == (2 + -1) _then 1 _else 2")) -> interp(emptyenv) -> equals(NEW(NumVal)(1)) );
    
    CHECK( (parse_str("_fun (x) x + 1")) -> equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)))));
    CHECK( (parse_str("(_fun (x) x + 1)(10)")) -> interp(emptyenv) -> equals(NEW(NumVal)(11)));
    CHECK( (parse_str("_let f = _fun (x) x + 1 _in f (10)")) -> equals(NEW(LetExpr)("f", NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))), NEW(CallExpr)(NEW(VarExpr)("f"), NEW(NumExpr)(10)))));
    
    PTR(Expr) e = parse_str("_let f = _fun (x) x + 1 _in f (10)");
    PTR(LetExpr) le = CAST(LetExpr)(e);
    
    CHECK( le -> body -> subst(le -> varStr, le -> rhs -> interp(emptyenv)) ->optimize() -> equals(NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))), NEW(NumExpr)(10))));
    
    CHECK( (parse_str("_let f = _fun (x) x + 1 _in f (10)")) -> interp(emptyenv) -> equals(NEW(NumVal)(11)));
    
    CHECK( parse_str("_let f = _fun (x) _if x == 0 _then 0 _else _if x == 2 + -1 _then 1 _else 3 _in f (0)")->interp(emptyenv) -> to_string() == "0" );
    
    
    CHECK( (parse_str("_let f = _fun (x) x * x _in f (2)")) -> interp(emptyenv) -> equals(NEW(NumVal)(4)));
    
    // two arguments test case
    CHECK( (parse_str("_let y = 8 _in _let f = _fun (x) x * y _in f (2)")) -> interp(emptyenv)-> to_string() == "16");
    
    
    // partial application test case
    CHECK( (parse_str("_let add =  _fun (x) _fun(y) x + y _in _let addFive = add (5) _in addFive (10)")) -> interp(emptyenv) -> equals(NEW(NumVal)(15)) );
    
    CHECK(parse_str("f(10)(1)")->equals(NEW(CallExpr)(NEW(CallExpr)(NEW(VarExpr)("f"),NEW(NumExpr)(10)),NEW(NumExpr)(1))));
    
    
    // recursive function test case
    CHECK( (parse_str("_let fac = _fun (fac) _fun (x) _if x == 1 _then 1 _else x * fac (fac) (x + -1) _in fac (fac) (5)")) -> interp(emptyenv) -> to_string() == "120" );
    
    std::string streseTest = "_let fib = _fun (fib) _fun (x) _if x == 0 _then 1 _else _if x == 1 _then 1 _else fib(fib)(x + -1) + fib(fib)(x + -2) _in fib(fib)(10)";
    
    CHECK( (parse_str(streseTest)) -> interp(emptyenv) -> to_string() == "89" );
    
    CHECK( (parse_str(streseTest)) -> optimize() -> to_string() == streseTest );
    
    CHECK( Step::interp_by_steps((parse_str("_let x = 2 _in x + 8"))) -> equals(NEW(NumVal)(10)) );
    CHECK( Step::interp_by_steps((parse_str("_if 1 == 9 _then 1 _else 2"))) -> equals(NEW(NumVal)(2)) );
    
    
    CHECK( Step::interp_by_steps((parse_str("(_fun (x) x + 1)(10)"))) -> equals(NEW(NumVal)(11)) );
    CHECK( Step::interp_by_steps((parse_str("_let f = _fun (x) x + 1 _in f (10)"))) -> equals(NEW(NumVal)(11)) );
    CHECK( Step::interp_by_steps((parse_str("_let y = 8 _in _let f = _fun (x) x * y _in f (2)"))) -> equals(NEW(NumVal)(16)) );
    CHECK( Step::interp_by_steps((parse_str("_let fac = _fun (fac) _fun (x) _if x == 1 _then 1 _else x * fac (fac) (x + -1) _in fac (fac) (5)"))) -> equals(NEW(NumVal)(120)) );
    
    // TEST FOR LOOP TO SEE IF THE INTERPRETER WILL NERVER CRASH BY EXHAUSTIGN THE C STACK
    std::string countdownTest = "_let countdown = _fun (countdown) _fun (n) _if n == 0 _then 0 _else countdown (countdown) (n + -1) _in countdown (countdown) (1000000)";
    CHECK( Step::interp_by_steps((parse_str(countdownTest))) -> equals(NEW(NumVal)(0)) );
    
    std::string countTest = "_let count = _fun (count) _fun (n) _if n == 0 _then 0 _else 1 + count (count) (n + -1) _in count (count) (10000)";
    
    CHECK( Step::interp_by_steps((parse_str(countTest))) -> equals(NEW(NumVal)(10000)) );
    
    
}
