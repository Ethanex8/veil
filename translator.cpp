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

#include "translator.h"

std::string translate(std::shared_ptr<Package> package) {
  std::string code;
  for (auto function : package->function_entities()) {
    code += translate(function);
  }
  return code;
}

std::string translate(std::shared_ptr<Function> function) {
  std::string code;
  if (function->return_type() == ReturnType::none) {
    code += "void ";
  } else if (function->return_type() == ReturnType::value) {
    code += function->return_class()->name() + " ";
  }
  code += function->name() + "(";

  std::string params{};
  for (
    auto it = function->object_entities().cbegin();
    it != function->object_entities().cend();
    ++it
  ) {
    auto object = *it;
    params += object->cls()->name() + " " + object->name();
    if (it != function->object_entities().cend() - 1) {
      params += ", ";
    }
  }
  code += params + ") {\n";

  for (auto statement : function->statement_entities()) {
    code += "  ";
    if (auto return_statement =
      std::dynamic_pointer_cast<ReturnStatement>(statement))
    {
      code += "return " + translate(return_statement->expression());
    }
    code += ";\n";
  }
  code += "}\n";
  return code;
}

std::string translate(OperatorType operator_type) {
  switch (operator_type) {
    case OperatorType::plus:
      return "+";
    default:
      return "?";
  }
}

std::string translate(std::shared_ptr<Expression> expression)
{
  std::string code;
  if (auto operator_expression =
    std::dynamic_pointer_cast<OperatorExpression>(expression))
  {
    code += "(";
    for (
      auto it = operator_expression->expression_entities().cbegin();
      it != operator_expression->expression_entities().cend();
      ++it
    ) {
      auto expression = *it;
      code += translate(expression);
      if (it != operator_expression->expression_entities().cend() - 1) {
        code += translate(operator_expression->operator_type());
      }
    }
    code += ")";
  }
  else if (auto object_expression =
    std::dynamic_pointer_cast<ObjectExpression>(expression))
  {
    code += object_expression->object()->name();
  }
  return code;
}