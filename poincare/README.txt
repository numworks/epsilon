# Poincare

Expressions have a structure of tree, storing:
- Pointers to the operand expressions (ie children)
- A pointer to the parent expression
Expressions are typed (ie Type::Addition, Type::Multiplication...) and some expressions also hold values (ie Type::Rational).
Multiplication and Addition are the only type that can hold an infinite number of operands. Other expressions have a fixed number of operands (ie AbsoluteValue).

## Simplification

Expression simplification is done in-place and modify directly the expression.
Simplify is a two-phase method:
- It first reduces the expression
- Then, it beautify the expression
So far, we excluded matrices from the simplificaiton process to avoid increasing complexity due to non commutativity in multiplications.

### Order

We define an simplification order on expressions with the following features:
- The order is total on types and values:
    Rational(-2/3) < Rational(0) < ... < Multiplication < Power < Addition < ...
- The order relationship is depth-first recursive: if two expressions are equal in type (and values), we compare their operands starting with the last.
To compare two expressions, we first sort their commutative children to ensure the unicity of expression representations. This guarantee that the order is total on expressions.

Moreover, the simplification order have some other specificities for some types:
- for addition and multiplication, any rational operand is always the first operands after sorting (by commutativity).
- Comparing an addition expression with an expression of different type called e is equivalent to comparing the addition with an addition with a single operand e.
- Idem for multiplications.
- To compare a power expression with an expression of different type called e, we compare the power with e^1.
The order groups like terms together to avoid quadratic complexity when factorizing addition or multiplication. For example, it groups terms with same bases together (ie Pi, Pi^3)  and with same non-rational factors together (ie Pi, 2*Pi).

Last comment; as the order is total, we can easily check if two expressions are identical by using this order.


### Reduce

The reducing phase is the most important part of simplification.
It happens recursively and bottom-up: we first reduce the operands of an expression before reducing the expression itself. That way, when reducing itself, an expression can assert that its operands are reduced (and thus have some properties explained in the following).
Every type of expression has its own rules of reducing.

To decrease the pool of possible expression types in reduced expressions, we converte subtraction to addition, division to power etc ... at reduction:
a-b -> a+(-1)*b
a/b -> a*b^(-1)
-a -> (-1)*a
sqrt(x) -> x^(1/2)
root(x,y) -> x^(1/y)
ln(x) -> log(x,e)

Here is a short tour of shallow reduction for the main types:

1. Additions are reduced by applying mathematics rules, ie:
- Associativity: (a+b)+c -> a+b+c
- Commutativity: a+b -> b+c which enables to sort operands and group like-terms together
- Factorization: a+5a -> 6a
- a+0->a
- We also reduce addition to same denominator.

2. Multiplications apply the following rules:
- Associativity
- Commutativity (which is true because we do no reduce matrices yet)
- a*0 -> 0
- Factorization
- sin/cos -> tan
- Distribution: a*(b+c) -> ab+ac

3. Powers apply the following rules:
- We get rid of square roots at denominator and of sum of 2 square roots at denominator
- x^0-> 1 if x != 0
- x^1 -> x
- 0^x -> 0 if x>0, undefined otherwise
- 1^x
- (a^b)^c -> a^(b+c) if a > 0 or c is integer
- (a*b*c*...)^n = a^n*b^n*c^n*... if n integer
- (a*b*...)^r -> |a|^r*(sign(a)*b*...)^r if a rational
- a^(b+c) -> (a^b)*a^c with a and b rational
- r^s with r, s rationals can be simplified using the factorisation in primes of r (ie, 8^(1/2) -> 2*2^(1/2))
- i^(p/q)-> exp^(i*Pi*p/(2q)) with p, q integers
- exp^(i*Pi*p/q) -> cos(Pi*p/q)+i*sin(Pi*p/q) with p, q integers
- x^log(y,x)->y if y > 0
- 10^log(y)

To avoid infinite loop, reduction is contextualized on the parent expression (ie, reducing addition to same denominator). This forces to reduce an expression only once it is included in a expression.

### Beautify

This phase turns expressions in a more readable way. Division, subtraction, Naperian logarithm reappear at this step. Parenthesis are also added to be able to print the tree in infix notation without any ambiguity.
This phase is also recursive and top-down: we first beautify the node expression and the beautify its operands.

## Approximation

Expressions can be approximate which return another (dynamically allocated) expression that can be either:
- a complex
- a matrix of complex

To approximate an expression, we first approximate its operands (which are ensured to be either complex or matrix of complex) and then approximate the expression using the operand approximations depending on its type.
