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
  Functions for translating the program graph into C code. The C code can then
  be run through a C compiler to generate a working program binary.
*/

#pragma once

#include <memory>
#include <string>
#include "graph.h"

/*
  These functions convert the given graph entity into valid C code. Child
  entities will be recursively visited in order to output proper program logic
  (a function's parameters and statements, an expression's sub-expressions,
  etc.).
*/
std::string translate(std::shared_ptr<Package> package);
std::string translate(std::shared_ptr<Function> function);
std::string translate(std::shared_ptr<Expression> expression);