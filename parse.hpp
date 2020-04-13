#ifndef parse_hpp
#define parse_hpp

#include <iostream>
#include "pointer.hpp"

class Expr;

PTR(Expr) parse(std::istream &in);

std::string interp(std::string s);
std::string stepInterp(std::string s);
std::string optimize(std::string s);
bool equals(std::string s1, std::string s2);


#endif /* parse_hpp */
