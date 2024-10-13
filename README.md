# Veil

An experimental, general-purpose programming language and library.

## Disclaimers

This is not an officially supported Google product. This project is not
eligible for the [Google Open Source Software Vulnerability Rewards
Program](https://bughunters.google.com/open-source-security).

This project is intended for demonstration purposes only. It is not
intended for use in a production environment.

## Contributing

See [`CONTRIBUTING.md`](CONTRIBUTING.md) for details.

## License

Apache 2.0; see [`LICENSE`](LICENSE) for details.

## Dependencies

This project depends on only standard operating system libraries, and the C/C++
programming languages.

## Design Principles

- Correctness and readability are the top priorities for language syntax and
  semantics

- Compile-time type safety

- Accessibility: advanced typing features like templates are built for ease of
  use (simple error messages)

- Usable for all but the most specialized applications, including system
  software and embedded systems

- Platform independent

- Metaprogramming: The compiler is extensible, providing a clean internal
  representation of compilation phases that can be manipulated with the language

- C-like syntax

- Flexible: definitions can be made in any order

## Roadmap

- [x] Version 0.1.0: Compiles a function that returns the sum of two integers,
  and produces valid C code.

- [ ] Version 0.2.0: Compiles a dynamic "list of integers" class.

- [ ] Version 0.3.0: Compiles basic class templates, run-time polymorphism, and
  function overloading.

- [ ] Version 0.4.0: Standard library with dynamic memory allocation, basic
  algorithms, and data structures including balanced trees and hash tables.

- [ ] Version 0.5.0: Compiler is ported from C++ to the new language, rewritten
  entirely in ".v" files and capable of compiling itself. From this point on,
  the C++ version is frozen and releases will consist of both ".v" code and
  generated C code for bootstrapping.

- [ ] Version 0.6.0: New translator is added to directly produce machine code
  with bindings to C libraries, no longer requiring a C compiler to produce
  a working program.

- [ ] Version 0.7.0: Compiler supports optimizations, matching or exceeding the
  performance of a C compiler on the same program.

# Features

This is a list of planned language features.

## Fundamentals

```
// Single line

/*
Multi line
*/

// Conditionals
if (x > y) {
} else {
}

// Loops
while (true) {
}

// Expressions
r = (a + b) * c - d;

// Scope hiding
int x;
if (true) {
  int x;
}

// Initialization safety
int x;
print(x);   // compiler error: cannot use object without initializing

// Aliases
int x = 0;
alias y = x;
y = 1;      // both x and y are now 1
```

## Packages

Packages are the top-level containers of the language. Everything defined inside
a package is isolated from all other packages, unless explicitly imported.

```
// Sets the current package to "a"
package a;

// Imports a public entity from another package.
import b.some_entity;
```

## Functions

- Accept zero or more parameters, passed by value

- Returns zero or one objects by value

- If specified in the function signature, a pointer may be implicitly
  dereferenced after being retured by value

- Return type, parameter types, and name are all part of a function signature
  to support overloading

```
// Return by value
func sum(int a, int b) -> int {
  return a + b;
}

sum(1, 2)  // return value is 3

// Dereference after return
func increment(int* p) *> int {
  *p += 1;
  return p;
}

int x = 0;
increment(&x) += 1;
// x is now 2
```

## Annotations

Most entities can be annotated with additional information, which is used by
subsequent compilation phases.

```
// Specifies memory alignment for class entities
annotation alignment : class {
  int value;
}

@alignment(value=16)
class buffer {
}
```

## Pointers and Arrays

```
int[8] ai;        // array of 8 int
ai[0] = 1;        // set first element to 1
int[rand()] ai2;  // error: array size must be fixed at compile time
some_func(ai);    // error: cannot copy arrays

int i;
int* pi;  // pointer to int
pi = &i;  // points to i

// Pass a pointer to an array as an argument
func accepts_array(int[]* ai) {...}
accepts_array(&ai);

// Increment pointer to next sequential int in memory
++pi;

// Manually construct and deconstruct objects referenced by pointers
some_class p = 0x80000000;  // memory address
p->ctor(16);
p->dtor();
```

## Classes

Supports the following class features:

- Constructors
- Destructors
- Operators
- Methods
- Implicit casts
- Multiple inheritance
- Polymorphism

```
package v;

@builtin
pub class int {
  pub ctor ();  // constructor
  pub ctor (literal<self>);
  pub ctor (self);
  pub dtor ();  // destructor
  pub oper=  (self) *> self;
  pub oper+= (self) *> self;
  pub oper-= (self) *> self;
  pub oper*= (self) *> self;
  pub oper/= (self) *> self;
  pub oper%= (self) *> self;
  pub oper&= (self) *> self;
  pub oper|= (self) *> self;
  pub oper++ ()     *> self;
  pub oper-- ()     *> self;
  pub oper== (self) -> bool;
  pub oper!= (self) -> bool;
  pub oper>  (self) -> bool;
  pub oper>= (self) -> bool;
  pub oper<  (self) -> bool;
  pub oper<= (self) -> bool;
  pub oper+  (self) -> self;
  pub oper-  (self) -> self;
  pub oper*  (self) -> self;
  pub oper/  (self) -> self;
  pub oper%  (self) -> self;
  pub oper&  (self) -> self;
  pub oper|  (self) -> self;
  pub cast   (self) -> longint;     // implicit cast
  pub mthd to_string () -> string;  // method
  pub func default () -> self;      // function (static)
}
```

## Templates

Templates can be defined for both classes and functions. In order to be used as
a template argument, a class must satisfy all required contracts.

```
contract allocator_of<class T> {
  pub mthd alloc(int count) -> T*;
  pub mthd dealloc(T*);
}

class managed {
  pub ctor() {
    ref_count_ = 0;
  }
  pub_to<ref> int ref_count_;
}

template class ref<
  // T must inherit from managed
  class T : managed,
  // Alc must be an allocator of T
  class Alc = default_allocator<T> : allocator_of<T>>
{
  pub template ctor<class Args... : params_of<T.ctor>>(Args args...) {
    pt_ = Alc.alloc(1);
    pt_->ctor(args...);
    ++pt_->ref_count_;
  }
  pub ctor(self rhs) {
    pt_ = rhs.pt_;
    ++pt_->ref_count_;
  }
  pub_to<T> ctor(T* pt) {
    pt_ = pt;
    ++pt_->ref_count_;
  }
  pub dtor() {
    --pt_->ref_count_;
    if (pt_->ref_count_ == 0) Alc.dealloc(pt_);
  }
  pub oper*() *> T {
    return pt_;
  }
  T* pt_;
}

class parser_impl : managed {}
alias parser = ref<parser>;
```

## Data Structures

Common data structures are provided in the standard library.

```
template class list<
  class T,
  class Alc = default_allocator<T> : allocator_of<T>>
{
  pub ctor () {
    _capacity = 8;
    _size = 0;
    _base = Alc.alloc(_capacity);
  }
  public dtor() {
    for (int i = 0; i < _size; ++i) {
      (_base + i)->dtor();
    }
    Alc.dealloc(_base);
  }
  pub template mthd add<
    ctor Ctor : member_of<T>,
    class Args... : params_of<T.ctor>>
    (Args args...)
  {
    if (_size == _capacity) {
      _capacity *= 2;
      T* new_base = Alc.alloc(_capacity);
      copy(_base, new_base, _size);
      Alc.dealloc(_base);
      _base = new_base;
    }
    (_base + _size)->Ctor(args...);
    ++_size;
  }
  pub mthd remove() {
    if (_size == 0) {
      return;
    }
    (_base + _size - 1)->dtor();
    --_size;
  }

  pub oper[](int i) *> T { return _base + i; }
  pub mthd size() -> int { return _size; }

  T*  _base;
  int _capacity;
  int _size;
}
```

## Metaprogramming

Compiler plugins can read and modify the program graph.

The example works as follows:
- A plugin called "precomputed" is invoked on all annotated functions.
- All annotated function definitions are compiled and loaded into the compiler
  binary.
- When an annotated function is invoked in the call graph, the arguments are
  examined to determine if they're known at compile time. If so, the compiled
  function is invoked in the compiler, and the result replaces the annotated
  function call. Otherwise, it is evaluated at runtime as normal.

```
@precomputed
func factorial(int n) -> int {
  if (n <= 1) {
    return 1;
  }
  return factorial(n - 1) * n;
}

factorial(5);   // replaced with 120 by compiler
factorial(x);   // evaluated at runtime
```