//
//  step.hpp
//  msdscript
//
//  Created by xiangjieli on 3/23/20.
//  Copyright © 2020 xiangjieli. All rights reserved.
//

#ifndef step_hpp
#define step_hpp


#include "pointer.hpp"
#include <stdio.h>
#include "expr.hpp"


class Expr;
class Env;
class Cont;
class Val;

class Step {
public:
    
    typedef enum {
        interp_mode,
        continue_mode
    } mode_t;
    
    static mode_t mode;       /* choose mode */

    static PTR(Expr) expr;    /* for interp_mode */
    static PTR(Env) env;      /* for interp_mode */
    
    static PTR(Val) val;      /* for continue_mode */
    
    static PTR(Cont) cont;    /* for all modes */
    
    static PTR(Val) interp_by_steps(PTR(Expr) e);
    
};

#endif /* step_hpp */
