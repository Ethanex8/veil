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
  Functions for printing the program graph in string form, useful for
  visualizing the program graph for debugging or instructive purposes.
*/

#pragma once

#include <memory>
#include <string>
#include "graph.h"

/*
  These functions convert the given graph entity into a textual respresentation,
  recursively converting child entities into the same string. The indenting
  defaults to 0, and is incremented by 2 each time a child is visited.
*/
std::string print(
  std::shared_ptr<Package> package, std::string::size_type indent = 0);
std::string print(
  std::shared_ptr<Function> function, std::string::size_type indent = 0);
std::string print(ReturnType return_type);
std::string print(
  std::shared_ptr<Class> cls, std::string::size_type indent = 0);
std::string print(
  std::shared_ptr<Object> object, std::string::size_type indent = 0);
std::string print(
  std::shared_ptr<Statement> statement, std::string::size_type indent = 0);
std::string print(OperatorType operator_type);
std::string print(
  std::shared_ptr<Expression> expression, std::string::size_type indent = 0);