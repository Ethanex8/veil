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

#include "parser.h"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

// Current state of the parser
enum class ParserState {
  // Inside an expression, expecting an operator
  expression_operator,
  // Inside an expression, expecting a value
  expression_value,
  // Expecting a function parameter list
  func_params_start,
  // Expecting a function name
  func_name,
  // Expecting a function return clause or function body
  func_return_clause,
  // Expecting a function body
  func_body,
  // Expecting a function parameter, starting with its class
  func_param,
  // Expecting a function parameter name
  func_param_name,
  // Expecting a function parameter or an empty parameter list
  func_param_or_end,
  // Expecting a class in the function return clause
  func_return_type,
  // Expecting another function parameter, or the end of the parameter list
  func_params_next_or_end,
  // Start of a new program entity
  start,
  // Expecting a statement or end of block
  statement,
};

Parser::Parser(std::vector<Token> tokens):
  tokens_{std::move(tokens)},
  iterator_{tokens_.cbegin()},
  state_{ParserState::start}
{}

/*
  The parser is implemented as a state machine, with the following additional
  properties:
    - An iterator into the token list is maintained, indicating the current
      token. The index will only ever be incremented.
    - If an unexpected token type is encounted, the compiler will terminate and
      an error message will be printed.
    - As the graph is being constructed, references to specific graph entities
      are maintained in order to keep track of where new entities should be
      inserted. This includes the current package, current function, etc.
*/
std::shared_ptr<Package> Parser::run() {
  package_ = std::make_shared<Package>();
  package_->set_name("default");

  std::shared_ptr<Class> int_class = std::make_shared<Class>();
  int_class->set_name("int");
  package_->add(int_class);

  while (true) {
    switch (state_) {
      case ParserState::start:
        switch (current_token().type) {
          case TokenType::end:
            return package_;
          case TokenType::func_keyword:
            function_ = std::make_shared<Function>();
            function_->set_return_type(ReturnType::none);
            package_->add(function_);
            state_ = ParserState::func_name;
            advance_token();
            break;
          default:
            fail();
        }
        break;
      case ParserState::func_name:
        switch (current_token().type) {
          case TokenType::identifier:
            function_->set_name(current_token().lexeme);
            state_ = ParserState::func_params_start;
            advance_token();
            break;
          default:
            fail();
        }
        break;
      case ParserState::func_params_start:
        switch (current_token().type) {
          case TokenType::left_paren:
            state_ = ParserState::func_param_or_end;
            advance_token();
            break;
          default:
            fail();
        }
        break;
      case ParserState::func_param_or_end:
        switch (current_token().type) {
          case TokenType::right_paren:
            state_ = ParserState::func_return_clause;
            advance_token();
            break;
          default:
            state_ = ParserState::func_param;
            break;
        }
        break;
      case ParserState::func_param:
        switch (current_token().type) {
          case TokenType::identifier:
            cls_ = package_->get_class(current_token().lexeme);
            if (!cls_) fail();
            object_ = std::make_shared<Object>();
            object_->set_cls(cls_);
            function_->add(object_);
            state_ = ParserState::func_param_name;
            advance_token();
            break;
          default:
            fail();
        }
        break;
      case ParserState::func_param_name:
        switch (current_token().type) {
          case TokenType::identifier:
            object_->set_name(current_token().lexeme);
            state_ = ParserState::func_params_next_or_end;
            advance_token();
            break;
          default:
            fail();
        }
        break;
      case ParserState::func_params_next_or_end:
        switch (current_token().type) {
          case TokenType::comma:
            state_ = ParserState::func_param;
            advance_token();
            break;
          case TokenType::right_paren:
            state_ = ParserState::func_return_clause;
            advance_token();
            break;
          default:
            fail();
        }
        break;
      case ParserState::func_return_clause:
        switch (current_token().type) {
          case TokenType::arrow:
            state_ = ParserState::func_return_type;
            advance_token();
            break;
          default:
            state_ = ParserState::func_body;
            break;
        }
        break;
      case ParserState::func_return_type:
        switch (current_token().type) {
          case TokenType::identifier:
            cls_ = package_->get_class(current_token().lexeme);
            if (!cls_) fail();
            function_->set_return_type(ReturnType::value);
            function_->set_return_class(cls_);
            state_ = ParserState::func_body;
            advance_token();
            break;
          default:
            fail();
        }
        break;
      case ParserState::func_body:
        switch (current_token().type) {
          case TokenType::left_curly:
            state_ = ParserState::statement;
            advance_token();
            break;
          default:
            fail();
        }
        break;
      case ParserState::statement:
        switch (current_token().type) {
          case TokenType::right_curly:
            state_ = ParserState::start;
            advance_token();
            break;
          case TokenType::return_keyword:
            return_statement_ = std::make_shared<ReturnStatement>();
            function_->add(return_statement_);
            state_ = ParserState::expression_value;
            advance_token();
            break;
          default:
            fail();
        }
        break;
      case ParserState::expression_value:
        switch (current_token().type) {
          case TokenType::identifier:
            object_ = function_->get_object(current_token().lexeme);
            if (!object_) fail();
            object_expression_ = std::make_shared<ObjectExpression>();
            object_expression_->set_object(object_);
            state_ = ParserState::expression_operator;
            advance_token();
            break;
          default:
            fail();
        }
        break;
      case ParserState::expression_operator:
        switch (current_token().type) {
          case TokenType::semicolon:
            if (operator_expression_) {
              operator_expression_->add(object_expression_);
              return_statement_->set_expression(operator_expression_);
              operator_expression_.reset();
            } else {
              return_statement_->set_expression(object_expression_);
            }
            object_expression_.reset();
            state_ = ParserState::statement;
            advance_token();
            break;
          case TokenType::plus: {
            std::shared_ptr<OperatorExpression> operator_expression =
              std::make_shared<OperatorExpression>();
            operator_expression->set_operator_type(OperatorType::plus);
            if (operator_expression_) {
              operator_expression_->add(object_expression_);
              operator_expression->add(operator_expression_);
            } else {
              operator_expression->add(object_expression_);
            }
            operator_expression_ = operator_expression;
            object_expression_.reset();
            state_ = ParserState::expression_value;
            advance_token();
            break;
          }
          default:
            fail();
        }
      break;
    }
  }
}

// Advanced the iterator to the next token
void Parser::advance_token() {
  if (current_token().type == TokenType::end) { return; }
  ++iterator_;
}

// Prints an error message referencing the current token, and exits
void Parser::fail() {
  std::cerr << "error: unexpected token " << current_token() << std::endl;
  std::exit(EXIT_FAILURE);
}