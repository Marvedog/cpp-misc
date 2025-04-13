#include <iostream>

int main() {
    auto hello = []() {
        std::cout << "Helo, lambda world!" << std::endl;
    };

    hello();
}