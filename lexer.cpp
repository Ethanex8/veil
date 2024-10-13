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

#include "lexer.h"
#include <map>

// Current state of the lexer
enum class LexerState {
  // Either CR or CRLF, both of which indicate a single newline
  cr_or_crlf,
  // Either / (division), // (single-line comment), or /* (multi line comment)
  divide_or_comment,
  // String of [A-Za-z_][A-Za-z0-9_]*. Either an identifier or keyword.
  identifier_or_keyword,
  // Either - (minus) or -> (arrow)
  minus_or_arrow,
  // Currently in a multi-line comment
  multi_line_comment,
  // Either CR or CRLF, but inside a multi-line comment
  multi_line_comment_cr_or_crlf,
  // Encountered *, will end multi-line comment if next char is /
  multi_line_comment_maybe_end,
  // Currently in a single-line comment, terminated on newline
  single_line_comment,
  // Start of a new lexeme
  start,
};

Lexer::Lexer(std::string source):
  source_{std::move(source)},
  state_{LexerState::start},
  index_{0},
  line_number_{1},
  column_number_{1},
  columns_per_tab_{2}
{}

/*
  If the given lexeme corresponds to a keyword, returns the TokenType of the
  keyword. Otherwise, returns TokenType::identifier.
*/
TokenType get_keyword_token_type(const std::string& lexeme) {
  static const std::map<std::string, TokenType> keyword_to_token_type {
    {"func", TokenType::func_keyword},
    {"return", TokenType::return_keyword},
  };
  const auto iter = keyword_to_token_type.find(lexeme);
  if (iter != keyword_to_token_type.cend()) return iter->second;
  return TokenType::identifier;
}

/*
  The lexer is implemented as a state machine, with the following additional
  properties:
    - An index into the source code string is maintained, indicating the current
      character. The index will only ever be incremented.
    - When entering the start state, the current index will be saved to indicate
      the start of a new lexeme. This saved index will be referenced later in
      order to capture the entire lexeme.
    - The line and column numbers in the source string will be tracked along
      with the index. These will be associated with generated tokens.
    - Generated tokens will be appended to the end of the result list. This list
      will only ever be appended to.
*/
std::vector<Token> Lexer::run()
{
  while (true) {
    switch (state_) {
      case LexerState::cr_or_crlf:
        if (current_char() == '\n') {
          advance_char();
        }
        advance_line();
        state_ = LexerState::start;
        break;
      case LexerState::divide_or_comment:
        switch (current_char()) {
          case '/':
            advance_char();
            state_ = LexerState::single_line_comment;
            break;
          case '*':
            advance_char();
            state_ = LexerState::multi_line_comment;
            break;
          default:
            add_token(TokenType::divide);
            state_ = LexerState::start;
            break;
        }
        break;
      case LexerState::identifier_or_keyword:
        if (isalnum(current_char()) || current_char() == '_') {
          advance_char();
        } else {
          Token& token = add_token(TokenType::identifier);
          token.type = get_keyword_token_type(token.lexeme);
          state_ = LexerState::start;
        }
        break;
      case LexerState::minus_or_arrow:
        if (current_char() == '>') {
          advance_char();
          add_token(TokenType::arrow);
        } else {
          add_token(TokenType::minus);
        }
        state_ = LexerState::start;
        break;
      case LexerState::multi_line_comment:
        switch (current_char()) {
          case '\n':
            advance_char();
            advance_line();
            break;
          case '\r':
            advance_char();
            state_ = LexerState::multi_line_comment_cr_or_crlf;
            break;
          case '*':
            advance_char();
            state_ = LexerState::multi_line_comment_maybe_end;
            break;
          default:
            advance_char();
            break;
        }
        break;
      case LexerState::multi_line_comment_cr_or_crlf:
        if (current_char() == '\n') {
          advance_char();
        }
        advance_line();
        state_ = LexerState::multi_line_comment;
        break;
      case LexerState::multi_line_comment_maybe_end:
        if (current_char() == '/') {
          advance_char();
          state_ = LexerState::start;
        } else {
          state_ = LexerState::multi_line_comment;
        }
        break;
      case LexerState::single_line_comment:
        switch (current_char()) {
          case '\n':
            advance_char();
            advance_line();
            state_ = LexerState::start;
            break;
          case '\r':
            advance_char();
            state_ = LexerState::cr_or_crlf;
            break;
          default:
            advance_char();
            break;
        }
        break;
      case LexerState::start:
        start_lexeme();
        if (isalpha(current_char()) || current_char() == '_') {
          advance_char();
          state_ = LexerState::identifier_or_keyword;
          break;
        }
        switch (current_char()) {
          case '+':
            advance_char();
            add_token(TokenType::plus);
            state_ = LexerState::start;
            break;
          case '*':
            advance_char();
            add_token(TokenType::multiply);
            state_ = LexerState::start;
            break;
          case '%':
            advance_char();
            add_token(TokenType::modulo);
            state_ = LexerState::start;
            break;
          case ',':
            advance_char();
            add_token(TokenType::comma);
            state_ = LexerState::start;
            break;
          case ';':
            advance_char();
            add_token(TokenType::semicolon);
            state_ = LexerState::start;
            break;
          case '{':
            advance_char();
            add_token(TokenType::left_curly);
            state_ = LexerState::start;
            break;
          case '}':
            advance_char();
            add_token(TokenType::right_curly);
            state_ = LexerState::start;
            break;
          case '(':
            advance_char();
            add_token(TokenType::left_paren);
            state_ = LexerState::start;
            break;
          case ')':
            advance_char();
            add_token(TokenType::right_paren);
            state_ = LexerState::start;
            break;
          case '\n':
            advance_char();
            advance_line();
            break;
          case '\r':
            advance_char();
            state_ = LexerState::cr_or_crlf;
            break;
          case '\t':
            advance_char();
            advance_tab();
            break;
          case ' ':
            advance_char();
            break;
          case '/':
            advance_char();
            state_ = LexerState::divide_or_comment;
            break;
          case '-':
            advance_char();
            state_ = LexerState::minus_or_arrow; 
            break;
          case '\0':
            add_token(TokenType::end);
            return std::move(tokens_);
        }
        break;
    }
  }
}

// Begins a new lexeme, saving the current index/column/line
void Lexer::start_lexeme() {
  start_index_ = index_;
  start_line_number_ = line_number_;
  start_column_number_ = column_number_;
}

// Advance to the next input character, incrementing the index/column/line
void Lexer::advance_char() {
  if (index_ == source_.size() - 1) return;
  ++index_;
  ++column_number_;
}

// Advance to the next line, resetting to column 1
void Lexer::advance_line() {
  ++line_number_;
  column_number_ = 1;
}

// Advance one tab worth of columns, taking into account partial tabs
void Lexer::advance_tab() {
  column_number_ =
    (column_number_ + columns_per_tab_) / columns_per_tab_ * columns_per_tab_;
}

// Returns the lexeme indicated by the saved index and the current index
std::string Lexer::get_lexeme() {
  std::size_t size = index_ - start_index_;
  if (size == 0) return std::string{};
  return std::string{source_, start_index_, size};
}

// Appends a new token of the given type to the result list
Token& Lexer::add_token(TokenType token_type) {
  return tokens_.emplace_back(
    Token{token_type, get_lexeme(), start_line_number_, start_column_number_});
}