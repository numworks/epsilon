---
title: Poincaré
---
# Poincaré

## Structure

Poincare takes text input such as `1+2*3` and turns it into a tree structure, that can be simplified, approximated and pretty-printed.

Each node of a tree represents either an operator or a value. All nodes have a type (`Type::Addition`, `Type::Multiplication`...) and some also store a value (ie `Type::Rational`).

![A example expression tree of Epsilon software](<%= p "expression-tree.svg" %>){:class="img-right"}
According to their types, expressions are childless (`Type::Rational`) or store pointers to their children (we call those children operands). To ease tree traversal, each node also keeps a pointer to its parent: that information is somewhat redundant but makes dealing with the expression tree much easier. `Multiplication` and `Addition` are the only type that can hold an infinite number of operands. Other expressions have a fixed number of operands: for instance, an `AbsoluteValue` will only ever have one child.

## RTTI: Run-time type information

The type of a C++ object is used by the compiler to generate a vtable. A vtable is a lookup table that tells which function to call for a given object class, hence creating polymorphism. Once the vtable has been built, the compiler completely discards the type information of a given object.

The problem with vtables is that they allow polyphormism based on a single class only: you can have different code called on a Node depending on whether it's an addition or a multiplication. But vtables can't handle dynamic behavior based on two parameters. For example, if you want to call a function depending on the type of two parameters, vtables can't do that.

That case happens quite often in Poincare: for example, if an expression contains the addition of another addition, we can merge both nodes in a single one ($$1+(\pi+x)$$ is $$1+\pi+x$$), see figure below). And we want to implement this behavior only if both nodes are additions.

The C++ standard has support for keeping type information at runtime, a behavior known as RTTI. However that feature is quite comprehensive and a bit overkill for what we needed, so we decided to do an equivalent solution manually: each expression subclass implements a `type()` function to give its type.

![Addition associative rule on a expression tree](<%= p "rtti.svg" %>){:class="img-responsive"}

## Expression parsing

Lexing and parsing are done by homemade lexer and parser available [here](https://github.com/numworks/epsilon/tree/master/poincare/src/parsing).

## Simplification

![Simplify an expression tree](<%= p "simplify.svg" %>){:class="img-right"}
Expression simplification is done in-place and modifies directly the expression. Simplifying is a two-step process: first the expression is reduced, then it is beautified. So far, we excluded matrices from the simplification process to avoid increasing complexity due to the non-commutativity of matrix multiplication.

### Ordering of operands

To simplify an expression one needs to find relevant patterns. Searching for a given pattern can be extremely long if done the wrong way. To make pattern searching much more efficient, we need to sort operands of commutative operations.

To sort those operands, we defined an order on expressions with the following features:

* The order is total on types and values: `Rational(-2/3)` < `Rational(0)` < `...` < `Multiplication` < `Power` < `Addition` < `...`
* The order relationship is depth-first recursive: if two expressions are equal in type and values, we compare their operands starting with the last.
* To compare two expressions, we first sort their commutative children to ensure the unicity of expression representations. This guarantees that the order is total on expressions.

![Order relationship on expressions](<%= p "order.svg" %>){:class="img-responsive"}
In the example, both root nodes are r so we compare their last operands. Both are equal to $$\pi$$ so we compare the next operands. As 3 > 2, we can conclude on the order relation between the expressions.

Moreover, the simplification order has a few additional rules:

* Within an `Addition` or a `Multiplication`, any `Rational` is always the first operand
* Comparing an `Addition` a with an `Expression` e is equivalent to comparing a with an `Addition` whose single operand is e. Same goes for the `Multiplication`.
* To compare a `Power` p with an `Expression` e, we compare $$p$$ with $$e^1$$.

Thanks to these rules, the order groups similar terms together and thus avoid quadratic complexity when factorizing. For example, it groups expressions with same bases together (ie $$\pi$$ and $$\pi^3$$) and terms with same non-rational factors together (ie $$\pi$$ and $$2*\pi$$).

Last but not least, as this order is total, it makes checking if two expressions are identical very easy.

### Reduction

The reduction phase is the most important part of simplification. It happens recursively and bottom-up: we first reduce the operands of an expression before reducing the expression itself. That way, when reducing itself, an expression can assert that its operands are reduced (and thus have some very useful knowledge such as "there is no `Division` or `Subtraction` among my operands"). Every type of `Expression` has its own reduction rules.

To decrease the set of possible expression types in reduced expressions, we turn `Subtraction` into `Addition`, `Division` and `Root` into `Power` and so on:

* $$a-b \rightarrow a+(-1)*b$$
* $$-a \rightarrow (-1)*a$$
* $$\frac{a}{b} \rightarrow a*b^{-1}$$
* $$\sqrt{x} \rightarrow x^{\frac{1}{2}}$$
* $$\sqrt[y]{x} \rightarrow x^{\frac{1}{y}}$$
* $$\ln(x) \rightarrow log_{e}(x)$$

![Reduce expression tree](<%= p "reduce.svg" %>){:class="img-responsive"}

Here is a short tour of the reduction rules for the main `Expression` subclasses:

#### `Additions` are reduced by common applying mathematics rules

* Associativity: $$(a+b)+c \rightarrow a+b+c$$
* Commutativity: $$a+b \rightarrow b+a$$ which enables to sort operands and group like-terms together
* Factorization: $$a+5*a \rightarrow 6*a$$
* $$a+0 \rightarrow a$$
* Reducing addition to the same denominator

#### `Multiplications` apply the following rules

* Associativity: $$(a*b)*c \rightarrow a*b*c$$
* Commutativity: $$a*b \rightarrow b*a$$ (which is true because we do no reduce matrices yet)
* Factorization: $$a*a^5 \rightarrow a^6$$
* $$a*0 \rightarrow 0$$
* $$\frac{sine}{cosine} \rightarrow tangent$$
* Distribution: $$a*(b+c) \rightarrow a*b+a*c$$

#### `Powers` apply the following rules

* We get rid of square roots at denominator and of sum of 2 square roots at denominator
* $$x^0 \rightarrow 1\;if\;x \neq 0$$
* $$x^1 \rightarrow x$$
* $$0^x \rightarrow 0 \;if\; x > 0$$
* $$1^x \rightarrow 1$$
* $$(a^b)^c \rightarrow a^{b*c} \;if\; a > 0 \;or\; c \in \mathbb{Z}$$
* $$(a*b*c*...)^n \rightarrow a^n*b^n*c^n*... \;if\; n \in \mathbb{Z}$$
* $$(a*b*c*...)^r \rightarrow \mid a\mid^r*(sign(a)*b*c*...)^r \;if\; a \in \mathbb{Q}$$
* $$a^{b+c} \rightarrow (a^b)*b^c \;if\; a, b \in \mathbb{Z}$$
* $$a^{b+c} \rightarrow (a^b)*b^c \;if\; a, b \in \mathbb{Z}$$
* $$r^s\;with\; r, s \in \mathbb{Q}$$ can be simplified using the factorisation in primes of $$r$$ (ie, $$8^{\frac{1}{2}} \rightarrow 2*2^{\frac{1}{2}}$$)
* $$i^{\frac{p}{q}} \rightarrow e^{\frac{i*\pi*p}{2*q}} \;with\; p, q \in \mathbb{Z}$$
* $$e^{\frac{i*\pi*p}{q}} \rightarrow cos(\frac{\pi*p}{q})+i*sin(\frac{\pi*p}{q}) \;with\; p, q \in \mathbb{Z}$$
* $$x^{log(y,x)} \rightarrow y \;if\;y > 0$$
* $$10^{log(x)} \rightarrow x \;if\; x > 0$$

To avoid infinite loops, reduction is contextualized on the parent expression. This forces to reduce an expression only once it has been attached to its parent expression.

### Beautify

![Beautify expression tree](<%= p "beautify.svg" %>){:class="img-left"}
This phase turns expressions in a more readable way. Divisions, subtractions, Naperian logarithms reappear at this step. Parentheses are also added to be able to print the tree in infix notation without any ambiguity. This phase is also recursive and top-down: we first beautify the node expression and then beautify its operands.

## Approximation

Expressions can be approximate thanks to the method `approximate()` which return another (dynamically allocated) expression that can be either:

* A complex
* A matrix of complexes

To approximate an expression, we first approximate its operands (which are ensured to be either complex or matrix of complexes) and then approximate the expression depending on its type (an `Addition` add its operand approximations for example).

# Pretty print

Poincare is responsible for laying out expressions in 2D as in a text book. The `ExpressionLayout` class represents the layout on screen of an `Expression`, and can be derived from an `Expression` by calling the function `createLayout()` . `ExpressionLayout` is also a tree structure, although the layout tree does not exactly follow the expression tree

![Expression layout tree](<%= p "layout.svg" %>){:class="img-responsive"}

![Expression layout baseline](<%= p "baseline.svg" %>){:class="img-right"}
The `baseline()` of `ExpressionLayout` is useful to align several layouts relatively to each other.
