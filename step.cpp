//
//  step.cpp
//  msdscript
//
//  Created by xiangjieli on 3/23/20.
//  Copyright © 2020 xiangjieli. All rights reserved.
//

#include "step.hpp"
#include "expr.hpp"
#include "env.hpp"
#include "cont.hpp"


Step::mode_t Step::mode;

PTR(Cont) Step::cont;

PTR(Expr) Step::expr; /* only for Step::interp_mode */
PTR(Env) Step::env; /* only for Step::interp_mode */

PTR(Val) Step::val;        /* only for Step::continue_mode */





PTR(Val) Step::interp_by_steps(PTR(Expr) e) {
    Step::mode = Step::interp_mode;
    Step::val = nullptr;
    Step::env = Env::empty;
    Step::expr = e;
    Step::cont = Cont::done;
    
    while (1) {
        if (Step::mode == interp_mode) {
            Step::expr -> step_interp();
        }
        else {
            if (Step::cont == Cont::done) {
                return Step::val;
            } else {
                Step::cont -> step_continue();
            }
        }
    }
}



