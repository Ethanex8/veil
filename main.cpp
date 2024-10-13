/*
  Copyright 2024 Google LLC

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      https://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

/*
  Compiles source code into C code, which can then be fed into a C compiler to
  generate a working binary. The compiler is architected to run through several
  phases in order to produce the final result.

  Phases:
  - Lexer: source code to tokens
  - Parser: tokens to graph
  - Translator: graph to C code
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "printer.h"
#include "token.h"
#include "translator.h"

// Prints the tokens to standard output, one per line
void print_tokens(const std::vector<Token> tokens) {
  for (const Token token : tokens) {
    std::cout << token << "\n";
  }
}

// Reads the file, returning the contents in a null-terminated string
std::string read_file(const std::string& file_name) {
  std::ifstream ifs{file_name};
  std::stringstream ss{};
  ss << ifs.rdbuf() << '\0';
  return ss.str();
}

int main() {
  // Read source file
  std::string source_code = read_file("input.v");
  std::cout << "----------V Code----------\n";
  std::cout << source_code << "\n";

  // Run lexer on source code to get a list of tokens
  std::shared_ptr<Lexer> lexer{std::make_shared<Lexer>(std::move(source_code))};
  std::vector<Token> tokens{lexer->run()};
  std::cout << "----------Tokens----------\n";
  print_tokens(tokens);

  // Run parser on list of tokens to build graph
  std::shared_ptr<Parser> parser{std::make_shared<Parser>(std::move(tokens))};
  std::shared_ptr<Package> package{parser->run()};
  std::cout << "----------Graph ----------\n";
  std::cout << print(package);

  // Translate graph into C code
  std::cout << "----------C Code----------\n";
  std::cout << translate(package);
}
