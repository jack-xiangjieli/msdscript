//
//  env.cpp
//  msdscript
//
//  Created by xiangjieli on 3/3/20.
//  Copyright © 2020 xiangjieli. All rights reserved.
//

#include "env.hpp"
#include "expr.hpp"
#include "value.hpp"

PTR(Env) Env::empty = NEW(EmptyEnv)();

//============================================================

EmptyEnv::EmptyEnv() {
    
}

PTR(Val) EmptyEnv::lookup(std::string find_name) {
    throw std::runtime_error("free variable: " + find_name);
}



//============================================================

ExtendedEnv::ExtendedEnv(PTR(Env) _rest, std::string _name, PTR(Val) _val) {
    rest = _rest;
    name = _name;
    val = _val;
    
}


PTR(Val) ExtendedEnv::lookup(std::string find_name) {
    if (find_name == name) {
        return val;
    }
    else {
        PTR(ExtendedEnv) ext_env = CAST(ExtendedEnv)(rest);
        
        if (ext_env == NULL) {
            throw std::runtime_error("free variable: " + find_name);
        }
        else return rest->lookup(find_name);
    }
}

