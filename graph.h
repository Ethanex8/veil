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
  Programs are represented in-memory by a graph. Every program entity (objects,
  functions, etc.) is represented as a graph node. Nodes are linked to each
  other to indicate relationships between entities (functions have parameters,
  packages have functions, etc.).

  A diagram of the entity inheritance hierarchy is given below, with each type
  being a possible graph node.

  Entity
    Package
    Function
    Class
    Object
    Statement
      ReturnStatement
      Expression
        ObjectExpression
        OperatorExpression
*/

#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

template<typename EntityT> class EntityContainer;
class Class;
class Entity;
class Expression;
class Function;
class Object;
class ObjectExpression;
class OperatorExpression;
class Package;
class ReturnStatement;
class Statement;

// Base class for all entities
class Entity : public std::enable_shared_from_this<Entity> {
public:
  // Gets or sets the entity name
  const std::string& name() const { return name_; }
  void set_name(std::string name) { name_ = std::move(name); }

  // Polymorphic
  virtual ~Entity() = default;

  // Not copyable or assignable
  Entity(const Entity&) = delete;
  Entity& operator=(const Entity&) = delete;

protected:
  // Only constructible by derived classes
  Entity() = default;

private:
  // For setting parent-child relationships
  template<typename EntityT> friend class EntityContainer;

  std::string name_;
  std::shared_ptr<Entity> parent_;
};

/*
  Entities that contain other entities of type EntityT must inherit from this
  template. Basic operations such as getting, adding, and removing contained
  entities are provided.
*/
template<typename EntityT> class EntityContainer : public virtual Entity {
public:
  // Returns the contained entity with name, or nullptr if no such entity exists
  std::shared_ptr<EntityT> get(const std::string& name) const;

  // List of all contained entities
  const std::vector<std::shared_ptr<EntityT>>& entities() const {
    return entities_;
  }

  // Adds entity to the end of the list of contained entities
  void add(std::shared_ptr<EntityT> entity);

  // Removes entity from the list of contained entities
  void remove(std::shared_ptr<EntityT> entity);

private:
  std::vector<std::shared_ptr<EntityT>> entities_;
};

/*
  Packages are the top-level entity, containing all other types of entities
  (directly or indirectly). Different packages and their contained entities are
  isolated from each other, unless explicit linkages between packages are
  defined.
*/
class Package :
  public virtual Entity,
  public EntityContainer<Class>,
  public EntityContainer<Function>
{
public:
  // Methods for contained Class entities (see EntityContainer)
  std::shared_ptr<Class> get_class(const std::string& name) const {
    return EntityContainer<Class>::get(name);
  }
  const std::vector<std::shared_ptr<Class>>& class_entities() const {
    return EntityContainer<Class>::entities();
  }
  using EntityContainer<Class>::add;
  using EntityContainer<Function>::add;

  // Methods for contained Function entities (see NodeContainer)
  std::shared_ptr<Function> get_function(const std::string& name) const {
    return EntityContainer<Function>::get(name);
  }
  const std::vector<std::shared_ptr<Function>>& function_entities() const {
    return EntityContainer<Function>::entities();
  }
  using EntityContainer<Class>::remove;
  using EntityContainer<Function>::remove;
};

// Functions may be defined with different types of return semantics
enum class ReturnType {
  // The function returns no objects
  none,
  // The function returns an object by value (a copy is made)
  value,
  // TODO: the function returns an auto-dereferenced pointer
};

/*
  Functions are a grouping of program logic. All logic must be contained inside
  a function. Functions accept input parameters, a list of objects provided by
  the function called. Functions may return an object back to the caller. A
  function's body is composed of a list of statements, which comprise the logic
  of the function.
*/
class Function :
  public virtual Entity,
  public EntityContainer<Object>,
  public EntityContainer<Statement>
{
public:
  // Methods for contained Object entities (see EntityContainer)
  std::shared_ptr<Object> get_object(const std::string& name) const {
    return EntityContainer<Object>::get(name);
  }
  const std::vector<std::shared_ptr<Object>>& object_entities() const {
    return EntityContainer<Object>::entities();
  }
  using EntityContainer<Object>::add;
  using EntityContainer<Object>::remove;

  // Methods for contained Statement entities (see EntityContainer)
  std::shared_ptr<Statement> get_statement(const std::string& name) const {
    return EntityContainer<Statement>::get(name);
  }
  const std::vector<std::shared_ptr<Statement>>& statement_entities() const {
    return EntityContainer<Statement>::entities();
  }
  using EntityContainer<Statement>::add;
  using EntityContainer<Statement>::remove;

  // Gets or sets the return type
  ReturnType return_type() const { return return_type_; }
  void set_return_type(ReturnType return_type) { return_type_ = return_type; }

  /*
    Gets or sets the class of the returned object. Not applicable for
    ReturnType::none.
  */
  std::shared_ptr<Class> return_class() const { return return_class_; }
  void set_return_class(std::shared_ptr<Class> return_class) {
    return_class_ = return_class;
  }

private:
  ReturnType return_type_;
  std::shared_ptr<Class> return_class_;
};

/*
  Classes are the fundamental entity of the typing system. Every object in a
  program must have a class. Classes define the valid operations on an object,
  and any contained objects.
*/
class Class : public virtual Entity {};

/*
  Objects are the fundamental data entity within a program. Every piece of data
  that is stored or operated on belongs to an object.
*/
class Object : public virtual Entity {
public:
  // Gets or sets the class
  std::shared_ptr<Class> cls() const { return cls_; }
  void set_cls(std::shared_ptr<Class> cls) { cls_ = cls; }

private:
  std::shared_ptr<Class> cls_;
};

/*
  Statements are the fundamental program execution unit. There are many types of
  statements, and they have a variety of purposes such as operating on objects
  or controlling the flow of a program.
*/
class Statement : public virtual Entity {};

/*
  A return statement is used to exit a function, returning control back to the
  caller. Return statements may be followed by an expression, with the resulting
  object passed back to the caller.
*/
class ReturnStatement : public Statement {
public:
  // Gets or sets the expression
  std::shared_ptr<Expression> expression() const { return expression_; }
  void set_expression(std::shared_ptr<Expression> expression) { 
    expression_ = expression;
  }

private:
  std::shared_ptr<Expression> expression_;
};

/*
  Expressions are a sequence of operations performed on objects. They are
  recursive, with one expression often being composed of multiple
  sub-expressions, connected with operators.
*/
class Expression : public Statement {};

// Type of operator that appears within an expression
enum class OperatorType {
  // The + binary operator
  plus,
};

/*
  An operator expression combines two or more sub-expressions with a common type
  of operator. For example, a+b is an operator expression of type "plus" and
  sub-expressions "a" and "b".
*/
class OperatorExpression :
  public Expression,
  public EntityContainer<Expression>
{
public:
  // Methods for contained Expression entities (see EntityContainer)
  std::shared_ptr<Expression> get_expression(const std::string& name) const {
    return EntityContainer<Expression>::get(name);
  }
  const std::vector<std::shared_ptr<Expression>>& expression_entities() const {
    return EntityContainer<Expression>::entities();
  }
  using EntityContainer<Expression>::add;
  using EntityContainer<Expression>::remove;

  // Gets or sets the operator type
  OperatorType operator_type() const { return operator_type_; }
  void set_operator_type(OperatorType operator_type) {
    operator_type_ = operator_type;
  }

private:
  OperatorType operator_type_;
};

/*
  An object expression evaluates to a single object. For example, "a" is an
  object expression.
*/
class ObjectExpression : public Expression {
public:
  // Gets or sets the object
  std::shared_ptr<Object> object() const { return object_; }
  void set_object(std::shared_ptr<Object> object) { object_ = object; }
private:
  std::shared_ptr<Object> object_;
};

template<typename EntityT>
std::shared_ptr<EntityT> EntityContainer<EntityT>::get(
  const std::string& name) const
{
  for (auto entity : entities_) {
    if (entity->name() == name) {
      return entity;
    }
  }
  return nullptr;
}

template<typename EntityT>
void EntityContainer<EntityT>::add(std::shared_ptr<EntityT> entity) {
  entities_.push_back(entity);
  entity->parent_ = shared_from_this();
}

template<typename EntityT>
void EntityContainer<EntityT>::remove(std::shared_ptr<EntityT> entity) {
  auto iterator = std::find(entities_.begin(), entities_.end(), entity);
  if (iterator != entities_.end()) {
    entities_.erase(iterator);
    iterator->parent_ = nullptr;
  }
}