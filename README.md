# The Hyc language

The goal of this project is to get more familiar with programming compilers and using LLVM. 
## Language features

All the features are still quite buggy...

The basic features are the same as in C, there are structs and unions and functions.
However I added and will add many more features that are not present in C. 
The following is a presentation of what the first version of the language should look like. The feature is not necessarily implemented already. (Although I have implemented many features already).

## Uniform call syntax
Define a function like this:
```C
int add(int* a, int b){
        // ^--- pass the first parameter as a pointer
  return *a + b;
}
```
Then use it like this:
```C
int a = 5;
int b = 5;
a.add(b); // <-- returns 10.
```
If a is a pointer:
```C
a->add(b);
```
### Structs
Structs behave almost the same way as in C

## Unions
Unions in Hyc are special.
They are always tagged and typesafe
A declaration of a union looks like this:
```C
union Foo
{
   int a : 1; // <-- the number after the ":" is the tagged value. The tagged value is of type uint.
   float b; // <-- the tagged value is automatically incremented (starting from the first member) for each added member,
            // but you can specify a value explicitly. Here b has tagged value "2".
    c;  // union members can be untyped. The tagged value is "3".
}
```
Use it like this:
```C
Foo f;
f.a = 10; // <-- makes a active and writes value to it
f!!c;   // <-- makes c active, don't write memory to it. Possible also for untyped members
f.c = 2; // <-- error: cannot write to untyped
f??c; // <-- returns true if c is active. in this case c was set active through f!!c.
uint tagged_value = f as uint; // <-- cast to uint returns the tagged value, in this case "3".
```
Unions can be matched:
```C
match(f)
{
   case a as val // <-- val is a pointer to f.a and can be used in the scope of this particular match stmt.
   { // <-- case stmts need to be opened and closed with { and }
   
   } // <-- there is no implicit fall through to other cases.
   case b <-- defining a pointer can be omitted
   {
   }
   case c // <-- here defining a pointer is forbidden -> Cannot point to untyped value.
   {
   }
}
```
## Named return values
function definitions can look like this:
```C
int[100] ary fill_array(int i)
          ^--- explicitly name the return value.
{
   for(idx : 0..100)
   {
      *ary[idx] = i;
   // ^--- 1. The named return value is implicitly passed to the function as a pointer.
   // 2. the named return value is allocated on the caller stack
   }
   return; // <-- can be omitted, but functions with a named return value must always return void.
}
```
Functions with named return values behave like you would expect:
```C
int[100] val = fill_array(10); // <-- No copy operation is performed here. "val" is implicitly passed to fill_array as a parameter
//        ^--- Use val as you would use any other variable.
int i = fill_array(12)[0]; // <-- Behaves as you would expect. No copy operation is performed. 
                // ^-- An anonymous variable allocated on the caller stack is passed as a pointer
```
## Arrays
Arrays don't decay into pointers. They can be passed as parameters and can be return values.
To declare an array:
```C
int[100] a;
//  ^--- The array specifier is added to the type not to the name!
```
To get the address of an array do this: 
```C
int* a = &ary[0];
```
## Pointers
Pointers should behave the same way as in C.

## Function Pointers
Function Pointers behave the same way as in C, but with a different syntax:
To declare a function pointer:
```C
fptr(int ; int, int) a;
//    ^-- first the return type
//       ^-- ";" marks that now the argument types are specified
//          ^-----^---- argument types
```
To use it:
```C
int add(int a, int b){return a + b;}
void main()
{
  fptr(int;int,int) foo = add(;int,int); 
                      //  ^-- This is a fptr identifier. It references a unique function.
                      //      ^-- note the ";" at the beginning. You don't need to specify a return type.
  foo(5,5); // <-- call a function pointer like any other function
}
```
## Generics
Hyc supports generic structs, unions and functions.
It doesn't support full blown template metaprogramming.
For a call to a generic function: The generic function parameters are implicitly deduced from the arguments.
If a generic parameter cannot be deduced from the arguments, it must be explicitly specified.
Generic types can have default values, they have to come at the end of the generic type declarations e.g.
```C
T2 foo<T,T2=int>(T a, T b){}
             ^-- Default type is int
```
Following algorithm is used to determine the actual type of a generic type parameter in a function call:
```
For each generic type parameter g:
  1. deduce g from the arguments, if that fails...
  2. ... take the current specified generic argument.
     If there is one, look at the next specified generic argument for the next iteration.
     If there is none ...
  3. ... use the specified default type. If there is no default type ...
  4. ... Emit error
```
## Copy operator
Define copy operators and move operators.
Copy operators look like this:
```cpp
operator void =(C* a, C*b) {}
            // ^-- parameters need to be pointers to work.
            // Operators cannot be generic
```
Call it like this:
```C
C foo; 
C bar;
foo = bar; // <-- desugares into =(&foo,&bar)
```
You can delete operators explicitly:
```cpp
operator void =(C* a, C* b) = delete;
```
If every member in a struct defines a copy operator, the compiler will automatically generate a copy operator for the struct itself.
## Move operator
An additional operator to disambiguate between copy and move operations.
Move operators are defined for basic types (int, float, pointers etc.) and behave the same way as copy operators, but can be overloaded for all other types:
```cpp
operator void #(C* a, C* b){}
```
Also if every member in a struct defines a move operator, the compiler will define a move operator for the struct itself
Move operators can be explicitly deleted, just like copy operators.

## Destructors
A special function, that is automatically called for a variable when it gets out of scope. Just like in C++.
When every member of a struct defines a destructor, a destructor will be generated for the struct itself.
Can not be explicitly deleted.
Definition:
```cpp
operator void del(C* c){}
            // ^-- func must be called del!
```

## Out of order definitions:
The order of definition doesn't matter. You can do something like this and it would work:
```C
void foo(){bar();}
void bar(){}
```

## Lambdas
Lambda declaration:
```cpp
[a,b](C* a)->void foo;
// ^-- specifies the captured types
//     ^-- specifies the argument
//          ^-- specifies the return type.
```
Lambda expressions:
```cpp
[int,int](C* a)->void foo = [a,b](C* a){} -> void // <-- in a lambda expression, the return type can be omitted
```
Lambdas can be defined anywhere and have value semantics. 
You can assign a lambda expression to a lambda typed variable.
Call it like any other function. 
They are basically supercharged function pointers because they can also capture variables.

## deferred local type inference
Altough my examples didn't show, but Hyc has *** deferred *** local type inference.
What this means is simple:
```cpp
a := 0; // <-- automatically infers type of a to be int.
auto b = 1; // <-- alternative syntax 
auto c; // <-- declare c, has unknown type
[...] // <-- do something else
c = 5; // <-- if the operator is a "=" or a "#", the type is inferred from the right hand side. Note that "c" cannot be used unless the type is inferred.
```
Also note, that the type of the lhs will be inferred from the type of the rhs.
What this means is, that if you have custom copy/move operations like:
```cpp
void operator=(C* c, B* b){}
[...]
B b;
c := b; // <-- doesn't call operator=(C*,B*)!, instead calls operator=(B* b1, B* b2).
```
# Implemented features
- [X] Structs
- [X] Unions
- [X] Destructors
- [X] Move and Copy operators
- [ ] explicitly delete Move and Copy operators
- [X] Function Pointers
- [ ] Lambdas
- [ ] Initialize all variables in place 
- [X] Named return values
- [X] Uniform Call syntax
- [X] Array types
- [ ] Compile more than one file
- [ ] Compile to library
- [ ] Const
- [ ] Global Variables
- [ ] Compile time code execution: Make everything executable at compile time
- [ ] Support for 128,256 bit integers and 128bit floating point values
- [ ] Strings
- [ ] Stdout
- [ ] String interpolation / String format
- [ ] Language support in vscode
- [ ] Debug Information
- [ ] Overload operators
Features now being worked on: Initialize all variables in place

# Other TODOS
- Support move operator in every aspect (currently no MoveDeclStmt etc.) 
- Optimize destructor calls -> destructor calls that are basically no ops should not be called!
- implement casts
- expose parts of c std lib
- better error messages (show whole line in error message)
- infer amount of array from init list.
- support move operator in array/struct initialization expression
- Optimize generation of array init expression.
