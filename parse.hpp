#ifndef parse_hpp
#define parse_hpp

#include <iostream>
#include "pointer.hpp"

class Expr;

PTR(Expr) parse(std::istream &in);

#endif /* parse_hpp */
