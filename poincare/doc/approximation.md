# Approximation API

## Entry point

`Approximation::To` is the main approximation entry point.
This method takes a `const Tree *` input, `Parameters` and an optional `Context`.
The return type is a customization point of the template method.
Available return types are:
- `Tree*`
- `float` / `double`
- `std::complex<float>` / `std::complex<double>`
- `Coordinate2D<float>` / `Coordinate2D<float>`
- `PointOrRealScalar<float>` / `PointOrRealScalar<double>`
- `BooleanOrUndefined`

Types which have both a `float` and a `double` version automatically set the float precision of the approximation algorithm to `FloatPrecision::Single` or `FloatPrecision::Double` respectively.
For types which do not have a float type included (`Tree*` and `BooleanOrUndefined`), the `Approximation::To` overload with the `FloatPrecision` parameter must be used to specify the desired float precision.

Another overload of `Approximation::To` allows to pass an abscissa (as a `float` or a `double` value). Var0 in the expression will be replaced by the value passed in this "abscissa" parameter. The float precision is automatically deduced from the "abscissa" float type.

### Parameters

#### projectLocalVariables

To use when the tree has not been projected and may have parametered nodes.

Indeed, these nodes' parameter (the local variables) needs to be projected to `Var` nodes for the approximation to work.

#### prepare

Only used on Trees that have been projected beforehand.

An additional step will be taken to prepare the expression for a better approximation.

For example, `x * y^(-1) * ln(x) * ln(10)^(-1)` will be replaced with `x * log(x) / y`.

This also greatly improve integral approximations.

#### optimise

In addition to `prepare` parameter, and only used for the `Tree*` return type of `Approximation::To`.

The resulting Tree will have been optimized for successive approximations (for Grapher for example).

In practice this means `ApproximateAndReplaceEveryScalar` will be applied on the tree.

### Context

Context passes all the necessary information for the approximation :
- Random nodes memoization
- Local variables values
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
