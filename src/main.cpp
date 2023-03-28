#include "parser.hpp"
#include <iostream>
#include <string>

auto main() -> int {
	nixvm::parser::Parser parser("let x = 1; in x");

	auto expr = parser.parse();

	std::cout << expr->toString() << std::endl;

	return 0;
}
