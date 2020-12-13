#ifndef TAST_UTIL_HPP__
#define TAST_UTIL_HPP__

#include <cstdio>

#define COUT(expr) std::cout << "|| " << #expr << " =~? " << (expr) << std::endl;
#define CODE(statement) printf("// %s\n", #statement); statement
#define DESC(msg, ...) printf("-- "); printf(msg, ## __VA_ARGS__); printf("\n")

#define TAST(fun) \
    std::cout << "## " << #fun << std::endl; \
    fun(); \
    std::cout << std::endl;


#endif /* end of include guard: TAST_UTIL_HPP__ */
