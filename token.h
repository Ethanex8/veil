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

#pragma once

#include <ostream>
#include <string>

enum class TokenType {
  arrow,
  comma,
  divide,
  end,
  func_keyword,
  identifier,
  left_curly,
  left_paren,
  minus,
  modulo,
  multiply,
  plus,
  return_keyword,
  right_curly,
  right_paren,
  semicolon,
};

struct Token {
  TokenType type;
  // String from source file that comprises this token
  std::string lexeme;
  // Line of source file where the token was found
  int line_number;
  // Column of source file where the token was found
  int column_number;
};

// Prints a token to an output stream
std::ostream& operator<<(std::ostream& os, const TokenType& token_type);
std::ostream& operator<<(std::ostream& os, const Token& token);