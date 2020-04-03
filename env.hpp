//
//  env.hpp
//  msdscript
//
//  Created by xiangjieli on 3/3/20.
//  Copyright © 2020 xiangjieli. All rights reserved.
//

#ifndef env_hpp
#define env_hpp

#include <stdio.h>
#include <string>

#include "pointer.hpp"

class Val;

class Env ENABLE_THIS(Env){
public:
    static PTR(Env) empty;
    virtual PTR(Val) lookup(std::string find_name) = 0;
};

class EmptyEnv : public Env {
public:
    EmptyEnv();
    PTR(Val) lookup(std::string find_name);
};

class ExtendedEnv : public Env {
public:
    std::string name;
    PTR(Val) val;
    PTR(Env) rest;
    
    ExtendedEnv(PTR(Env) rest, std::string name, PTR(Val) val);
    PTR(Val) lookup(std::string find_name);
};


#endif /* env_hpp */
