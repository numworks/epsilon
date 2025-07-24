# Poincare

Poincare is Epsilon's algebraic computation engine.

Available in-depth documentation:
- [Style guide](doc/style_guide.md)
- [Internal simplification algorithm](doc/simplification.md)
- [Internal tree data structure](doc/tree.md)
- To be completed: [External API](doc/external.md)

## Structure 

Poincare takes text input such as `1+2*3` and turns it into a tree structure, which can be simplified, approximated and pretty-printed.

#### Parsing

Lexing and parsing are done by homemade lexer and parser available [here](/poincare/src/layout/parsing).

#### Simplification

Our simplification algorithm is detailed [here](doc/simplification.md).

#### Pretty print

Poincare is responsible for laying out expressions in 2D as in a text book. Documentation for layouts can be found [here](doc/layout.md).
