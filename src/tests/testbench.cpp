// backwither testing framework

#include "includes.h"
#include <iostream>

// from GNU assert.h
#define assert(expr) (static_cast <bool> (expr) ? success () : fail (#expr, __FILE__, __LINE__))

class Testbench {
    bool ok    = true;
    int  tests = 0;
    int  errs  = 0;
public:
    void fail (const char* expr, const char *file, unsigned int line) {
        std::cout << std::endl << "[" <<  file << ":" << line << "] " <<
                  "\033[31mERROR: " << expr << "\033[0m" << std::endl; // red
        ok = false;
        tests++;
        errs++;
    }

    void success () {
        std::cout << ".";
        tests++;
    }

    int run () {
        std::cout << "\033[1mRUNNING TESTS\033[0m" << std::endl; // bold

        #include "asserts.cpp"

        if (ok) {
            std::cout << std::endl << "\033[1;32mALL " << tests << " TESTS OK\033[0m" << std::endl; // bold green
            return 0;
        } else {
            std::cout << std::endl << "\033[1;31m" << errs << " ERROR(S) IN " << tests << " TESTS\033[0m" << std::endl; // bold red
            return errs % 255;
        }
    }
};

int main () {
    Testbench tb = Testbench();
    return tb.run();
}