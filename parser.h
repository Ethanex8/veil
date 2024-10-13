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
  The parser is responsible for converting a list of tokens into a program
  graph. The tokens are grouped into program entities, entity properties, and
  relationships between entities. Unlike tokens, which come in a flat list, the
  graph is a highly structured representation of the program.
*/

#pragma once

#include <memory>
#include <vector>
#include "graph.h"
#include "token.h"

enum class ParserState;

// Converts a list of tokens into a program graph
class Parser {
public:
  /*
    The token list must be passed on construction, and it must include a
    terminating "end" token.
  */
  Parser(std::vector<Token> tokens);

  // Runs the parser, returning the top-level entity of the program graph
  std::shared_ptr<Package> run();

private:
  std::vector<Token> tokens_;
  std::vector<Token>::const_iterator iterator_;
  ParserState state_;

  std::shared_ptr<Package> package_;
  std::shared_ptr<Function> function_;
  std::shared_ptr<Object> object_;
  std::shared_ptr<Class> cls_;
  std::shared_ptr<ReturnStatement> return_statement_;
  std::shared_ptr<ObjectExpression> object_expression_;
  std::shared_ptr<OperatorExpression> operator_expression_;

  const Token& current_token() const { return *iterator_; }
  void advance_token();
  void fail();
};