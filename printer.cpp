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

#include "printer.h"

std::string print(
  std::shared_ptr<Package> package, std::string::size_type indent)
{
  std::string text(indent, ' ');
  text += "Package:" + package->name() + "\n";
  for (auto function : package->function_entities()) {
    text += print(function, indent + 2);
  }
  return text;
}

std::string print(
  std::shared_ptr<Function> function, std::string::size_type indent)
{
  std::string text(indent, ' ');
  text += "Function:" + print(function->return_type()) + "\n";
  if (function->return_type() == ReturnType::value) {
    text += print(function->return_class(), indent + 2);
  }
  for (auto object : function->object_entities()) {
    text += print(object, indent + 2);
  }
  for (auto statement : function->statement_entities()) {
    text += print(statement, indent + 2);
  }
  return text;
}

std::string print(ReturnType return_type) {
  switch (return_type) {
    case ReturnType::none:
      return std::string{"none"};
    case ReturnType::value:
      return std::string{"value"};
    default:
      return std::string{"unknown"};
  }
}

std::string print(
  std::shared_ptr<Class> cls, std::string::size_type indent)
{
  std::string text(indent, ' ');
  text += "Class:" + cls->name() + "\n";
  return text;
}

std::string print(
  std::shared_ptr<Object> object, std::string::size_type indent)
{
  std::string text(indent, ' ');
  text += "Object:" + object->name() + "\n";
  text += print(object->cls(), indent + 2);
  return text;
}

std::string print(
  std::shared_ptr<Statement> statement, std::string::size_type indent)
{
  std::string text(indent, ' ');
  if (auto return_statement =
    std::dynamic_pointer_cast<ReturnStatement>(statement))
  {
    text += "ReturnStatement\n";
    text += print(return_statement->expression(), indent + 2);
  }
  return text;
}

std::string print(OperatorType operator_type) {
  switch (operator_type) {
    case OperatorType::plus:
      return std::string{"plus"};
    default:
      return std::string{"unknown"};
  }
}

std::string print(
  std::shared_ptr<Expression> expression, std::string::size_type indent)
{
  std::string text(indent, ' ');
  if (auto operator_expression =
    std::dynamic_pointer_cast<OperatorExpression>(expression))
  {
    text += "OperatorExpression:" + print(operator_expression->operator_type())
      + "\n";
    for (auto expression : operator_expression->expression_entities()) {
      text += print(expression, indent + 2);
    }
  }
  else if (auto object_expression =
    std::dynamic_pointer_cast<ObjectExpression>(expression))
  {
    text += "ObjectExpression\n";
    text += print(object_expression->object(), indent + 2);
  }
  return text;
}