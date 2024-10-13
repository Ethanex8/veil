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
  The lexer is responsible for converting a source code string into a list of
  tokens. The characters in the string are grouped into lexemes, which are
  substrings that make up the "words" of the language. Each lexeme is then
  immediately converted into a token. Unlike strings and lexemes, tokens are
  uniform data structures and more easily handled by subsequent compiler phases.
*/

#pragma once

#include <string>
#include <vector>
#include "token.h"

enum class LexerState;

// Converts source code into a list of tokens
class Lexer {
public:
  /*
    The source code string must be passed on construction, and it must have a
    terminating null character.
  */
  Lexer(std::string source);

  /*
    Sets the number of columns per tab, which affects the column number
    that is associated with each token. If not specified, this defaults to 2.
  */
  void set_columns_per_tab(int columns_per_tab) {
    columns_per_tab_ = columns_per_tab;
  }

  /*
    Runs the lexer, returning a list of tokens. The final token in the returned
    list will be of type TokenType::end.
  */
  std::vector<Token> run();

private:
  void start_lexeme();
  char current_char() const { return source_[index_]; }
  void advance_char();
  void advance_line();
  void advance_tab();
  std::string get_lexeme();
  Token& add_token(TokenType token_type);

  std::string source_;
  LexerState state_;
  std::string::size_type index_;
  std::string::size_type start_index_;
  int line_number_;
  int start_line_number_;
  int column_number_;
  int start_column_number_;
  int columns_per_tab_;
  std::vector<Token> tokens_;
};