# Poincare simplification architecture

## A three-tiered architecture

We formalize the simplification of an Expression the following way:
1. Find an interesting pattern in an Expression
2. Build a new Expression based on that pattern.

So in other words, a simplification is the association of a pattern and a gene-
ration rule. We formalize this approach with three classes : a `Simplification`
is made out of an `ExpressionSelector` (whose job is to detect a pattern in an
`Expression`) and of an `ExpressionBuilder` (which will build a new expression).

To give more details, it goes this way :
```
Expression * inputExpression;
ExpressionSelector * selector
ExpressionBuilder * builder;

ExpressionMatch * match = selector->match(inputExpression);
Expression * simplifiedExpression = builder->build(match);
```

## Rules
```
Addition(Integer(a),Integer(b),c*) -> Addition($Sum(a,b),c*)
```

Addition(Integer(a),Integer(b)) -> Function(Sum,a,b)
Addition(Addition(a*),b*) -> Addition(a*,b*)


- Matches have to be exhaustive i.e : capture all children
 -> We can tell it at compile time (e.g. "Hey, Addition(a,b) can miss children,
you need a wildcard", but "ln(a)" is allright because ln has only one child)

Addition(Integer(0),...) -> Addition(...)
Product(Integer(0),...) -> Integer(0)

Division(Division(a,b),c) -> Division(a,Product(b,c))

Build(type=addition)
 - integer(0)
 - integer(1)
 - and then all the clones of...
 - and then all the clones of...


a*b+a*c -> a*(b+c)


a+(b+c)
