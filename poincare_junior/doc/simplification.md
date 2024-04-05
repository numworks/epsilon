# Simplification algorithm

## General view

From a parsed user input expression, the simplification algorithm does:
1. Ensure the expression has a valid dimension (units, matrices, lists ...)
2. Adjust the approximation strategy if the expression's dimension requires it (units)
3. Seed the random nodes
4. Project the expression, approximate depending on the strategy
5. Replace all user symbols with variables
6. Apply systematic reduction
7. Bubble up lists, applying systematic reduction
8. Apply advanced reduction
9. Simplify Dependencies
10. Approximate again, depending on the strategy
11. Beautify expression
12. Replace User symbols and functions stored in context and start over at step 1.

## 1 - Dimension check


Dimension covers units, matrix size, and list size (handled in a different functions in a similar way).

This is done first here so that all following steps can assume the dimension is correct, removing the need for many checks.

Some issues such as Unreal, division by zero or other undefinitions can still arise later.

## 2 - Approximation strategy

The simplification algorithm handle three simplification strategies :
 - `Default`: Default strategy.
 - `NumbersToFloat`: All numbers are approximated to floats (TODO: This is currently not used and a bit deprecated due to float propagation. We should either remove it or clarify its use.)
 - `ApproximateToFloat`: Everything that can be approximated to a float is approximated (everything but variables, random, expressions having children that cannot be approximated).

Starting from `Default`, each strategy is less and less demanding in term of tree size, but the quality of the simplification will downgrade (example of $1-0.3-0.7$).

Approximation strategy is checked here and later in the simplification algorithm (some steps may unlock new possible approximations).

At this step :

### If we detected units in the expression

Most units (except angle ones) enforce an approximation of the expression. There is no need to simplify with `Default` strategy in that case (since strategy will be accounted for later as well).

To ensure a constant strategy throughout the Simplification process, we simply raise a `RelaxContext` exception, restarting the simplification with a downgraded strategy, unless it is already at `ApproximateToFloat`.

### If strategy is `ApproximateToFloat`

Before projection (that could reduce approximation precision) and random nodes seeding (that is not yet relevant), apply the approximation to reduce the expression as much as possible.

For example:
$$ln(cos(x))^{ln(cos(1))} = ln(cos(x))^{-0.615626}$$

## 3 - Random nodes seeding

Since the next steps may duplicate parts of the expression, we need to seed each random node. a duplicated random node should evaluate to the same random number.

For example, with this projection, both random should never approximate to different values.
$$sinh(random())=\frac{e^{random()}-e^{-random()}}{2}$$

Therefore, we seed each random in this step with an id. On approximation, random nodes with a same id will be approximated to the same value.

## 4 - Projection

It's expected to:
- Reduce the number of equivalent representations of an expression (Div(A,B) -> Mult(A, Pow(B, -1)))
- Un-contextualize the expression (remove unit, complex format and angle units considerations from reduction algorithm)
- Do nothing if applied a second time

### Effects

<details>

| Match | Replace |
|---|---|
| unit | 1 |
| decimal{n}(A) | 10^(-n)×A |
| cos(A) | trig(A×RadToAngleUnit, 0) |
| sin(A) | trig(A×RadToAngleUnit, 1) |
| tan(A) | tanRad(A×RadToAngleUnit) |
| acos(A) | atrig(A, 0)×RadToAngleUnit |
| asin(A) | atrig(A, 1)×RadToAngleUnit |
| atan(A) | atanRad(A)×RadToAngleUnit |
| sqrt(A) | A^0.5 |
| e^A | exp(A) |
| A^B (with A matrix) | powerMatrix(A, B) |
| A^B (with real complex format) | powerReal(A, B) |
| ceil(A) | -floor(-A) |
| frac(A) | A - floor(A) |
| e | exp(1) |
| conj(A) | re(A)-i×re(A) |
| - A | (-1)×A |
| A - B | A + (-1)×B |
| A / B | A×B^-1 |
| log(A, e) | ln(e) |
| log(A) | ln(A)×ln(10)^(-1) |
| log(A, B) | ln(A)×ln(B)^(-1) |
| ln(A) (with real complex format) | lnReal(A) |
| sec(A) | 1/cos(A) |
| csc(A) | 1/sin(A) |
| cot(A) | cos(A)/sin(A) |
| arcsec(A) | acos(1/A) |
| arccsc(A) | asin(1/A) |
| arccot(A) | acos(0) - atan(A) |
| cosh(A) | (exp(A)+exp(-A))×1/2 |
| sinh(A) | (exp(A)-exp(-A))×1/2 |
| tanh(A) | (exp(2A)-1)/(exp(2A)+1) |
| arcosh(A) | ln(A + sqrt(A - 1)×sqrt(A + 1)) |
| arsinh(A) | ln(A + sqrt(A^2 + 1)) |
| artanh(A) | (ln(1+A)-ln(1-A))×1/2 |

</details>

## 5 - User symbols

We list all global user symbols, sorted in the alphabetical order.

All symbols (global and local) are then projected to an id, based on if they are global or local, and how nested they are in the local contexts, using Bruijn indices.

For example, under the expression are the projected id of the user symbols. Some of the local user symbols are preserved for beautification (since we don't list them with the global user-symbols).

This step converts the `UserVariable` trees (containing the variable name) into a `Variable` (containing the id only).

```
a+b+diff(a+c+x+sum(k+a+x, k, 1, b), x, b)+a
0 1      1 3 0     0 2 1  k  1  2   x  1  0
```

In other words, within a parametered expression, `0` is the local parameter.

When nested inside a parametered expression, all indexes are incremented.

This variable id has to be accounted for when comparing trees, or manipulating them in and out of parametric expressions, using `Variables::LeaveScope` and `Variables::EnterScope`.

## 6 - Systematic reduction

It's expected to:
- Be efficient and simple
- Apply obvious and definitive changes
- Do nothing if applied a second time
- Ignore second term of dependencies

### Effects

<details>

| Match | Replace |
|---|---|
| A+(B+C) | A+B+C |
| A×(B×C) | A×B×C |
| A+Dep(B, C) | Dep(A+B, C) |
| 1^x | 1 |
| 0^B (with re(B) <= 0) | undef |
| 0^B (with re(B) > 0) | 0 |
| 0^B | Dep(0, 0^B) |
| A^B (with B not an integer) | exp(B×ln(A)) |
| A^0 (with A != 0) | 1 |
| A^0 | Dep(1, A^0) |
| A^1 | A |
| i^n | 1, i, -1 or -i |
| (w^p)^n | w^(p×n) |
| (w1×...×wk)^n | w1^n×...×wk^n |
| exp(a)^b | exp(a×b) |
| +(A) | A |
| +() | 0 |
| B + A | A + B |
| 0 + A + B | A + B |
| 2 + 4.1 | 6.1 |
| 2×a + 4.1×a | 6.1×a |
| ×(A) | A |
| ×() | 1 |
| B×A | A×B |
| 2×4.1 | 8.2 |
| 0×A | 0 |
| 1×A×B | A×B |
| t^m×t^n | t^(m+n) |
| powerReal(A, B) (with A complex or positive, or B integer) | A^B |
| powerReal(A, B) (with A negative, B negative rational p/q, q even) | unreal |
| powerReal(A, B) (with A negative, B rational p/q, q odd) | ±|A|^B |
| abs(abs(x)) | abs(x) |
| abs(x) (when x is a number) | ±x |
| trigDiff({1,1,0,0}, {1,0,1,0}) | {0, 1, 3, 0} |
| trig(-x,y) | ±trig(-x,y) |
| trig(πn/120, B) (with some values of n) | exact value |
| trig(atrig(A,B), B) | A |
| trig(atrig(A,B), C) | sqrt(1-A^2) |
| atrig(trig(π×y, i), j) | π/2 - atrig(trig(π×y, i), i) |
| atrig(trig(π×y, 0), 0) (with ⌊y + π/2⌋ even) | π×(y - ⌊y + π/2⌋) |
| atrig(trig(π×y, 0), 0) (with ⌊y + π/2⌋ odd) | π×(⌊y + π/2⌋ - y) |
| atrig(trig(π×y, 1), 1) (with ⌊y⌋ even) | π×(y - ⌊y⌋) |
| atrig(trig(π×y, 1), 1) (with ⌊y⌋ odd) | π×(y - ⌊y⌋ + 1) |
| atrig(A,B) (with A one of the exact values) | exact value |
| arcsin(-x) | -arcsin(x) |
| arccos(-x) | π - arccos(x) |
| atan({-1, 0, 1}) | {-π/4, 0, π/4} |
| diff(A) (with all n children of A having a known partial derivative) | diff(child(A, 0))×partialDiff(A, 0) + ... + diff(child(A, n))×partialDiff(A, n) |
| partialDiff(A×B×C×D, 2) | A×B×D |
| partialDiff(A + B + C + D, 2) | 1 |
| partialDiff(exp(x), 0) | exp(x) |
| partialDiff(ln(x), 0) | 1/x |
| partialDiff(Trig(x, n), 0) | Trig(x, n - 1) |
| partialDiff(Trig(x, n), 1) | 0 |
| partialDiff(x^n, 0) | n×x^(n - 1) |
| partialDiff(x^n, 1) | 0 |
| lnReal(x) (with x > 0) | ln(x) |
| lnReal(x) (with x <= 0 or complex) | nonreal |
| lnReal(x) | Dep(ln(x), lnReal(x)) |
| ln(exp(x)) | x |
| ln(-1) | iπ |
| ln(1) | 0 |
| exp(ln(x)) | x |
| exp(0) | 1 |
| exp(B×ln(A)) (with B an integer) | A^B |
| arg(0) | undef |
| arg(x) (with re(x) = 0)| π/2 if im(x) > 0, -π/2 if im(x) < 0 |
| arg(x) (with re(x) > 0) | arctan(im(x)/re(x)) |
| arg(x) (with re(x) < 0 and im(x) >= 0) | arctan(im(x)/re(x)) + π |
| arg(x) (with re(x) < 0 and im(x) < 0) | arctan(im(x)/re(x)) - π |
| im(x) (with re(x) = 0) | -ix |
| im(x) (with im(x) = 0) | 0 |
| re(x) (with re(x) = 0) | 0 |
| re(x) (with im(x) = 0) | x |
| sum(k, k, m, n) | n(n + 1)/2 - (m - 1)m/2 |
| sum(k^2, k, m, n) | n(n + 1)(2n + 1)/6 - (m - 1)(m)(2m - 1)/6 |
| sum(f, k, m, n) (with f independent of k or random nodes) | f×(1 + n - m) |
| prod(f, k, m, n) (with f independent of k or random nodes) | f^(1 + n - m) |
| gcd(B, gcd(C, A)) | gcd(A, B, C) |
| lcm(B, lcm(C, A)) | lcm(A, B, C) |
| gcd(A) | A |
| lcm(A) | A |
| gcd(A, B) (with A, B integers) | exact value if A, B integers, undef otherwise |
| lcm(A, B) (with A, B integers) | exact value if A, B integers, undef otherwise |
| rem(A, 0) | undef |
| quo(A, 0) | undef |
| rem(A, B) (with A, B integers) | exact value |
| quo(A, B) (with A, B integers) | exact value |
| A! (with A positive integer) | Prod(k, 1, A, k) |
| binomial(n,k) (with valid n, k) | (n - 0)/(k - 0) × ... × (n - j)/(k - j) × ... × (n - k - 1)/(k - k + 1) |
| permute(n, k) (with valid n, k) | n!/(n-k)! |
| sign(A) | 0 / 1 / -1 if A sign is known |
| ⌊A⌋ (with A rational) | exact value |
| round(A, B) (with valid A, B) | floor(A×10^B + 1/2)×10^-B |
| listSort(L) | Apply sort |
| median(L) | result |
| dim(A) | result |
| L(n) | result |
| mean(L) | result |
| stddev(L) | result |
| variance(L) | result |
| sampleStdDev(L) | result |
| minimum(L) | result |
| maximum(L) | result |
| sum(L) | result |
| prod(L) | result |
| diff(dep(x, {ln(x), z}), x, y) | dep(diff(x, x, y), {diff(ln(x), x, y), z}) |

</details>

## 7 - List operators

TO COMPLETE

## 9 - Advanced Reduction

It's expected to:
- Reduce any reducible expression if given enough ressources
- Do its best with reduced ressources
- Be deterministic
- Ignore second term of dependencies

### Effects

<details>

| Match | Replace |
|---|---|
| A?×\|B\|×\|C\|×D? | A×\|BC\|×D |
| \|A×B?\| | \|A\|×\|B\| |
| \|A\| | exp(ln(re(A)^2+im(A)^2)/2) |
| exp(A?×i×B?) | cos(A×B) + i×sin(A×B) |
| exp(A + B?) | exp(A)×exp(B) |
| A?×exp(B)×exp(C)×D? | A×exp(B + C)×D |
| A? + cos(B) + C? + i×sin(B) + D? | A + C + D + exp(i×B) |
| A?×(B + C?)×D? | A×B×D + A×C×D |
| A? + B?×C×D? + E? + F?×C×G? + H? | A + C×(B×D + F×G) + E + H |
| (A? + B)^2 | (A^2 + 2×A×B + B^2) |
| A×ln(B) (with A integer) | ln(B^A) + i×(A×arg(B) - arg(B^A)) |
| A? + ln(B) + C? + ln(D) + E? | A + C + ln(BD) + E + i×(arg(B) + arg(D) - arg(BD)) |
| ln(12/7) | 2×ln(2) + ln(3) - ln(7) |
| ln(A×B?) | ln(A) + ln(B) - i×(arg(A) + arg(B) - arg(AB)) |
| ln(A^B) | B×ln(A) - i×( B×arg(A) - arg(A^B)) |
| i×(B×arg(A) - arg(A^B)) | i×k×2π (when k can be found) |
| i×(arg(A) + arg(B) - arg(A×B)) | i×k×2π (when k can be found) |
| A? + cos(B)^2 + C? + sin(D)^2 + E? | 1 + A + C + E |
| A?×Trig(B, C)×D?×Trig(E, F)×G? | 0.5×A×D×(Trig(B - E, TrigDiff(C, F)) + Trig(B + E, C + F))×G |
| Trig(A? + B, C) | Trig(A, 0)×Trig(B, C) + Trig(A, 1)×Trig(B, C-1) |
| sum(f + g, k, a, b) | sum(f, k, a, b) + sum(g, k, a, b) |
| sum(x_k, k, 0, n) | x_0 + ... + x_n |
| prod(f×g, k, a, b) | prod(f, k, a, b)×prod(g, k, a, b) |
| prod(x_k, k, 0, n) | x_0×...×x_n |
| Prod(u(k), k, a, b) / Prod(u(k), k, a, c) (with c < b) | Prod(u(k), k, c+1, b) |
| binomial(n, k) | n! / (k!(n - k)!) |
| permute(n, k) | n! / (n - k)! |
| tan(A) | sin(A) / cos(A) |
| atan(A) | asin(A / Sqrt(1 + A^2)) |
| im(x + y) | im(x) + im(z) |
| re(x + y) | re(x) + re(z) |
| im(x×y) | im(x)re(y) + re(x)im(y) |
| re(x×y) | re(x)re(y) - im(x)im(y) |
| A? + B?×im(C)×D? + E? | A - i×BCD + i×B×re(C)×D + E |
| A? + B?×re(C)×D? + E? | A + BCD - i×B×im(C)×D + E |

</details>

### Examples

<details>

Unsuccessful advanced reduction on simple tree $a+b$.

`_` represent the node that is being examined.

```mermaid
graph TD;
  A["_+(a,b)"]-->|NextNode|B["+(_a,b)"]
  B-->|NextNode|C["+(a,_b)"]
  C-->|NextNode|I["Impossible, compute metric"]
  A-->|Contract|AD["Impossible"]
  A-->|Expand|AE["Impossible"]
  B-->|Contract|BD["Impossible"]
  B-->|Expand|BE["Impossible"]
  C-->|Contract|CD["Impossible"]
  C-->|Expand|CE["Impossible"]
```

Successful advanced reduction on $|a||b|-|ab|=0$.

For clarity, Impossible paths are removed and nextNode are concatenated.

```mermaid
graph TD;
  A["_|a||b|-|ab|"]-->|8 NextNode|E["|a||b|-_|ab|"]
  E-->|3 NextNode|F["End of tree (20)"]
  E-->|Expand|J["_0"]
  J-->|1 NextNode|K["End of tree (1)"]
  A-->|Contract|L["_0"]
  L-->M["Already explored"]
```

Successful advanced reduction on
$$-a^2-b^2+(a+b)^2+a(c+d)^2=a(2b+(c+d)^2)$$
NextNode explorations are hidden for clarity.

```mermaid
graph TD;
  Root["-a^2-b^2+(a+b)^2+a(c+d)^2"]-->X1["36 VS 36"]
  Root-->|Expand|A["-a^2-b^2+(a+b)^2+a(c^2+2cd+d^2)"]
    A-->X2["45 VS 36"]
    A-->|Expand|AA["-a^2-b^2+(a+b)^2+ac^2+a(2cd+d^2)"]
      AA-->X3["49 VS 36"]
      AA-->|Expand|AAA["-a^2-b^2+(a+b)^2+ac^2+2acd+ad^2"]
        AAA-->X4["49 VS 36"]
        AAA-->|Expand|AAAA["2ab+ac^2+2acd+ad^2"]
          AAAA-->X5["Full path, 34 VS 36"]
        AAA-->|Contract|AAAB["(a+b)^2-(a^2+b^2)+ac^2+2acd+ad^2"]
          AAAB-->X6["Full path, 48 VS 34"]
          AAAB-->|Contract|AAABA["(a+b)^2-(a^2+b^2)+ad^2+a(c^2+2cd)"]
            AAABA-->X7["Full path, 48 VS 34"]
      AA-->|Expand|AAB["2ab+ac^2+a(2cd+d^2)"]
        AAB-->X8["34 VS 34"]
        AAB-->|Contract|AABA["a(2b+c^2)+a(2cd+d^2)"]
          AABA-->X9["Full path, 34 VS 34"]
          AABA-->|Contract|AABAA["a(2b+c^2+2cd+d^2)"]
            AABAA-->X10["Full path, 26 VS 34"]
      AA-->|Contract|AAC["(a+b)^2-(a^2+b^2)+ac^2+a(2cd+d^2)"]
        AAC-->X11["48 VS 26"]
        AAC-->|Expand|AACA["a^2+2ab+b^2-(a^2+b^2)+ac^2+a(2cd+d^2)"]
          AACA-->X12["Full path, 55 VS 26"]
          AACA-->|Contract|AACAA["a^2+b^2-(a^2+b^2)+a(2b+c^2)+a(2cd+d^2)"]
            AACAA-->X13["Full path, 55 VS 26"]
        AAC-->|Contract|AACB["(a+b)^2-(a^2+b^2)+a(c^2+2cd+d^2)"]
          AACB-->X14["44 VS 26"]
          AACB-->|Expand|AACBA["a^2+2ab+b^2-(a^2+b^2)+a(c^2+2cd+d^2)"]
            AACBA-->X15["Full path, 51 VS 26"]
    A-->|Expand|AB["2ab+a(c^2+2cd+d^2)"]
      AB-->X16["30 VS 26"]
      AB-->|Contract|ABA["a(2b+c^2+2cd+d^2)"]
        ABA-->X17["26 VS 26"]
        ABA-->|Contract|ABAA["a(c^2+d^2+2(b+cd))"]
          ABAA-->X18["Full path, 27 VS 26"]
          ABAA-->|Expand|ABAAA["ac^2+a(d^2+2(b+cd))"]
            ABAAA-->X19["Full path, 33 VS 26"]
    A-->|Contract|AC["(a+b)^2-(a^2+b^2)+a(c^2+2cd+d^2)"]
      AC-->X20["44 VS 26"]
      AC-->|Expand|ACA["a^2+2ab+b^2-(a^2+b^2)+a(c^2+2cd+d^2)"]
        ACA-->X21["51 VS 26"]
        ACA-->|Contract|ACAA["a^2+b^2-(a^2+b^2)+a(2b+c^2+2cd+d^2)"]
          ACAA-->X22["49 VS 26"]
          ACAA-->|Contract|ACAAA["a^2+b^2-(a^2+b^2)+a(c^2+d^2+2(b+cd))"]
            ACAAA-->X23["Full path, 50 VS 26"]
  Root-->|Expand|B["2ab+a(c+d)^2"]
    B-->X24["21 VS 26"]
    B-->|Contract|BA["a(2b+(c+d)^2)"]
      BA-->X25["19 VS 21"]
  Root-->|Contract|C["(a+b)^2-(a^2+b^2)+a(c+d)^2"]
    C-->X26["35 VS 19"]
    C-->|Expand|CA["a^2+2ab+b^2-(a^2+b^2)+a(c+d)^2"]
      CA-->X27["42 VS 19"]
      CA-->|Contract|CAA["a^2+b^2-(a^2+b^2)+a(2b+(c+d)^2)"]
        CAA-->X28["42 VS 19"]
```

</details>

## 9 - Simplify dependencies

TO COMPLETE

## 10 - Final approximation

With an approximation strategy, we approximate again here in case previous steps unlocked new possible approximations.

## 11 - Beautification

This step basically undo early steps and projections in the following order :

### Restore complex format

Unimplemented yet.

### Restore angle unit

All angle-dependant functions have been projected to radians in projection.

They are restored to the initial angle unit.

An advanced reduction may be called again after that because the created angle factors may be advanced reduced again. We can do this because, at this step, the expression is still mostly projected.

### Beautify

This step undo the projection by re-introducing nodes unhandled by simplification (For example, `Division`, `Log`, `Power` with non-integer indexes...).

`Addition`, `Multiplication`, `GCD` and `LCM` are also sorted differently.

### Restore Unit

The unit removed on projection is restored to the best prefix and representative.

### Restore Variable names

User variables, as well as nested local variables are restored to their original names.
