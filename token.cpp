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

#include "token.h"

std::ostream& operator<<(std::ostream& os, const TokenType& token_type) {
  switch (token_type) {
    case TokenType::arrow:
      os << "arrow";
      break;
    case TokenType::comma:
      os << "comma";
      break;
    case TokenType::divide:
      os << "divide";
      break;
    case TokenType::end:
      os << "end";
      break;
    case TokenType::func_keyword:
      os << "func_keyword";
      break;
    case TokenType::identifier:
      os << "identifier";
      break;
    case TokenType::left_curly:
      os << "left_curly";
      break;
    case TokenType::left_paren:
      os << "left_paren";
      break;
    case TokenType::minus:
      os << "minus";
      break;
    case TokenType::modulo:
      os << "modulo";
      break;
    case TokenType::multiply:
      os << "multiply";
      break;
    case TokenType::plus:
      os << "plus";
      break;
    case TokenType::return_keyword:
      os << "return_keyword";
      break;
    case TokenType::right_curly:
      os << "right_curly";
      break;
    case TokenType::right_paren:
      os << "right_paren";
      break;
    case TokenType::semicolon:
      os << "semicolon";
      break;
    default:
      os << "unknown";
      break;
  }
  return os;
}

std::ostream& operator<< (std::ostream& os, const Token& token) {
  return os << token.type << " \"" << token.lexeme << "\" "
    << token.line_number << " " << token.column_number;
}