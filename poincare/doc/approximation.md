#  Approximation API

## Public approximation API

When you are working from the project applications (i.e. Grapher, Calculation etc.), the public approximation API should be used (except if no Pool is available for your project).

### Entry points

The public approximation API is exposed by the typed Expressions (`UserExpression`, `SystemExpression`, `PreparedFunction`):
Several methods are available, depending on the Expression type, the approximation return type and other operations performed alongside approximation.
Non-exhaustive list:
`UserExpression`:
- `approximateToRealScalar`: returns a float
- `cloneAndApproximate`: returns a (`UserExpression`) clone of the expression on which approximation has been applied
- `cloneAndSimplifyAndApproximate`: create two (`UserExpression`) clones of the expression, one with the simplified expression and one with the approximated expression

`SystemExpression`:
- `approximateSystemToRealScalar`: returns a float
- `approximateSystemToTree`: returns a (`SystemExpression`) clone of the expression on which approximation has been applied
- `cloneAndBeautifyAndApproximate`: create two (`UserExpression`) clones of the expression, one with the beautified expression and one with the approximated expression

`PreparedFunction`:
- `approximateToRealScalarWithValue`: returns a float
- `approximateToPoint`: returns a `Coordinate2D`
- `approximateToPointOrRealScalarWithValue`: returns a `PointOrRealScalar`

### ApproximationPolicy

Most approximation entry points allow to pass an `ApproximationPolicy` as a function parameter.
If it is not the case, it is either because passing an `ApproximationPolicy` is not relevant for this entrypoint, either because `ApproximationPolicy` has not been ported yet to this entrypoint.
The parameters in `ApproximationPolicy` are:

#### `onlyReplaceNumerical`

This parameter can only be used if the approximation method returns an Expression (not a float or a Point for instance).

The numerical parts of the expression are approximated, but randoms, variables, functions, etc. are left unchanged.

For example:
- `pi*x + 1/3` -> `3.14*x + 0.33`.
- `(1+2+i)*(x+ln(12+3))` -> `(3.0+1.0*i)*(x+2.708)`

This is useful to optimize an expression for successive approximations (for Grapher for example).

In practice this means `ApproximateAndReplaceEveryScalar` is applied on the tree.

####  `optimize`

Skip some validation steps for a shorter computing time. These steps will be asserted instead. This is the case for the expression dimension check which is quite costly. This means that if you call `cloneAndApproximate` with `optimize=true`, you need to be sure that the expression you are approximating has a valid dimension.

## Internal approximation API

When working inside `Poincare::Internal`, it is frequent that an approximation needs to to be performed directly on a `Tree`. In this case, the internal approximation API should be used. Projects that do not have a Pool also need to use this internal approximation API.

### Entry point

`Approximation::To` is the main approximation entry point.
This method takes a `const Tree *` input and some `Parameters`.
The return type is a customization point of the template method.
Available return types are:
- `Tree*`
- `float` / `double`
- `std::complex<float>` / `std::complex<double>`
- `Coordinate2D<float>` / `Coordinate2D<double>`
- `PointOrRealScalar<float>` / `PointOrRealScalar<double>`
- `BooleanOrUndefined`

Types which have both a `float` and a `double` version automatically set the float precision of the approximation algorithm to `FloatPrecision::Single` or `FloatPrecision::Double` respectively.
For types which do not have a float type included (`Tree*` and `BooleanOrUndefined`), the `Approximation::To` overload with the `FloatPrecision` parameter must be used to specify the desired float precision.

Another overload of `Approximation::To` allows to pass an abscissa (as a `float` or a `double` value). Var0 in the expression will be replaced by the value passed in this "abscissa" parameter. The float precision is automatically deduced from the "abscissa" float type.

`Approximate` is the in-place version of `To` (for `Tree*`).

### Parameters

Approximation parameters are split into `ApproximationPolicy` (see the `ApproximationPolicy` section above) and `ExpressionVariantParameters`, which are parameters that depend on the expression type. `ExpressionVariantParameters` provide 4 alternatives:

- `User`: if the expression is a `UserExpression`, `UserSettings` (complex format, angle unit, symbol context) need to be passed. This type of expression will be automatically projected at the beginning of approximation. This is useful to project local variables of parametric nodes to `Var` nodes (required for the approximation to work).
- `Projected`: if the expression is a `SystemExpression`, it can optionally be "prepared". This is done by passing a `ProjectedPolicy{.prepare=true}` to the `expressionParameters`. This will trigger additional processing of the expression for a more accurate approximation. For example, `x * y^(-1) * ln(x) * ln(10)^(-1)` will be replaced with `x * log(x) / y`. This also greatly improve integral approximations.
- `Prepared`: if the expression is a `PreparedFunction`, it is already projected and prepared. No additional processing will be done before approximating, this allows for a faster approximation.
- `Unspecified` (default): sometimes a `Tree` is in an hybrid state and is neither a `User`, `Projected` or `Prepared` expression. In this case, `Unspecified` needs to be passed to `expressionParameters`. The expression will not be projected nor prepared before its approximation.

For debugging purposes, we recommend that you specify the expression type if it known instead of relying on the default `Unspecified` parameter.
