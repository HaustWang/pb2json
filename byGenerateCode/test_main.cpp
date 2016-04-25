#include "generator.hpp"
#include <iostream>

int main() {
    Generator gen;
    gen.Generate("test.proto", "test.hpp");
    gen.Generate("test.proto");
    std::cout<<gen.ErrString()<<std::endl;
    return 0;
}
