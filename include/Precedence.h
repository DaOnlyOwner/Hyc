#pragma once

// Expressions
enum class ExprPrecedence : int
{
    Group18 = 0, // ((
    Group17 = 10, // ,
    Group16 = 20, // ternary op, =, #, compound assignment, compound move
    Group15 = 30, // ||
    Group14 = 40, // &&
    Group13 = 50, // |
    Group12 = 60, // ^
    Group11 = 70, //  &
    Group10 = 80, // == !=
    Group9 = 90, // Relational ops
    Group8 = 100, // threeway (not implemented right now)
    Group7 = 110, // left and right shift
    Group6 = 120, // + - 
    Group5 = 130, // * / %
    Group4 = 140, // Pointer to member (not implemented right now)
    Group3 = 150, // unary + -, ! ~, as, *a, &a
    Group2 = 160, // postfix ++, --, Func call, Subscript, . ->
    Group1 = 170, // :: 
    Group0 = 180 // Ident, Numbers etc.
};