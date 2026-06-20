#include <iostream>

#include "executer.hpp"
#include "parser.hpp"

int main() {
  while (true) {
    std::cout << "nsh> " << std::flush;  //added flush to ensure the prompt is displayed immediately(fixed the issue of typing before the prompt appears)
    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) {
      continue;
    }

    std::vector<std::string> tokens = Parser::parseInput(input);
    Executer::execute(tokens);
  }
}
