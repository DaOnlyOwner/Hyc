#pragma once

// Expressions
enum class ExprPrecedence : int
{
    ident_expr = 10,
    integer_lit = 10,
    float_lit = 10,
    unary_op = 6,
    productGroup = 4,
    sumGroup= 3,
};