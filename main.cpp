#include <iostream>
#include "parse.hpp"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include "step.hpp"
#include "value.hpp"
#include "env.hpp"



static std::string random_expr(int nested);
static std::string random_addend(int nested);
static std::string random_inner(int nested);
static std::string random_number();
static std::string random_variable();
static std::string random_let(int nested);
//static std::string random_string(int number_of_numbers);

int ITERATION = 100;

int main(int argc, char **argv) {
//    Catch::Session().run(argc, argv);

    

     PTR(Expr) e = parse(std::cin);

    if (argc == 2 && strncmp(argv[1], "--opt", 5) == 0)
        std::cout << "The optimization result is : " << e->optimize()->to_string() << "\n";
    else if (argc == 2 && strncmp(argv[1], "--step_interp", 13) == 0) {
        std::cout << "The interp_by_steps result is : " << Step::interp_by_steps(e) -> to_string() << "\n";
    }
    else
        std::cout << "The interpretation result is : " <<  e->interp(Env::empty)->to_string() << "\n";

    
    
    
    
//    std::string test_string = random_expr(2);
//    std::cout << test_string;
    
//    const char* calculator_cmd[] = {"/usr/bin/parser", NULL};
//    ExecResult calc_res = exec_program(calculator_cmd, test_string);
//    check_success(calc_res);
//    std::cout << calc_res.out;
//
//    std::string test_string1 = random_expr(2);
//    const char* optimizer_cmd[] = {"/usr/bin/parser", "-optimize", NULL};
//    ExecResult opt_res = exec_program(optimizer_cmd, test_string1);
//    check_success(opt_res);
//    std::cout << opt_res.out;

    return 0;
}

//static void check_success(ExecResult &result) {
//    std::cerr << result.err;
//    if (result.exit_code != 0)
//        std::cerr << "non-zero exit: " << result.exit_code << "\n";
//}

// generate random test
std::string random_expr(int nested) {
    if (nested == 0) return "0";
    int number_of_addend = (rand() % 5) + 1;   // 1 to 5
    std::vector<std::string>* addendVec = new std::vector<std::string>;
    for (int i = 0; i < number_of_addend; i++) {
        addendVec -> push_back(random_addend(nested));
    }
    std::string expr = "";
    for (int i = 0; i < number_of_addend; i++) {
        expr += addendVec -> at(i);
        if (i != number_of_addend - 1) expr += " + ";
    }
    return expr;
}

std::string random_addend(int nested) {
    if (nested == 0) return "0";
    int number_of_inner = (rand() % 5) + 1;  // 1 to 5
    std::vector<std::string>* addendVec = new std::vector<std::string>;
    for (int i = 0; i < number_of_inner; i++) {
        addendVec -> push_back(random_inner(nested));
    }
    std::string addend = "";
    for (int i = 0; i < number_of_inner; i++) {
        addend += addendVec -> at(i);
        if (i != number_of_inner - 1) addend += " * ";
    }
    return addend;
}

std::string random_inner(int nested) {
    if (nested == 0) return "0";
    int type_of_inner = ((rand() % 4) + 1);     // 1 to 4
    if (type_of_inner == 1) return random_number();
    else if (type_of_inner == 2) return random_variable();
    else if (type_of_inner == 3) return "(" + random_expr(nested - 1) + ")";
    else if (type_of_inner == 4) return random_let(nested);
    return "";
}

// generate a random number ranging from 0 to 99
std::string random_number() {
    return std::to_string(rand()%100);
}

// generate a random varaible name which only contains lowercase letters
std::string random_variable() {
    return std::string(1, (char) ((rand() % 26) + 'a'));
}

std::string random_let(int nested) {
    std::string var = random_variable();
    std::string rhs = random_expr(nested - 1);
    std::string body = random_expr(nested - 1);
    return "_let " + var + " = " + rhs + " _in " + body;
}
