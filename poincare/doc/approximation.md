# Approximation API

## Main entry points

These are methods taking an input `const Tree *`, `Parameters` and an optional `Context`.

They return the approximated input under the requested dimension :
- `ToTree` : Return a Tree.
- `To` : Return a `double` or `float`, it is also overloaded to take an abscissa as parameter and replace Var0 with this value.
- `ToComplex` :  Return a `std::complex`.
- `ToPointOrScalar` : Return a `PointOrScalar`, also overloaded to take an abscissa.
- `ToPoint` : Return a `Point`.
- `ToBoolean` : Return a `bool`.

### Parameters

#### isRootAndCanHaveRandom

To use when the input tree is a root and may have random nodes.

This sets up a context for the approximation of various random nodes, handling random seeding and the `RandIntNoRep` tree.

#### projectLocalVariables

To use when the tree has not been projected and may have parametered nodes.

Indeed, these nodes' parameter (the local variables) needs to be projected to `Var` nodes for the approximation to work.

#### prepare

Only used on Trees that have been projected beforehand.

An additional step will be taken to prepare the expression for a better approximation.

For example, `x * y^(-1) * ln(x) * ln(10)^(-1)` will be replaced with `x * log(x) / y`.

This also greatly improve integral approximations.

#### optimise

In addition to `prepare` parameter, and only used in `ToTree`.

The resulting Tree will have been optimized for successive approximations (for Grapher fro example).

In practice this means `ApproximateAndReplaceEveryScalar` has been called on it.

### Context

Context passes all the necessary informations for the approximation :
- Random nodes memoization
- Local variable values
- Global symbol context
- List element or Point element that is being approximated
- Angle unit and Complex format

All of them have a default value, that can be used when the information doesn't matter.

A set of assertions across the approximation will ensure the right context has been provided.

For example, approximating `cos(3)` with default angle unit `AngleUnit::None` will raise, but approximating `Trig(3,1)` will not.

## Other entry points

TODO: Some of them may be made private.

### ApproximateAndReplaceEveryScalar

Replace as much as possible all subtrees of the expression into floats.

For instance : `(1+2+i)*(x+ln(12+3))` -> `(3.0+1.0*i)*(x+2.708)`
