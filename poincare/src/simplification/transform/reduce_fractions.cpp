// Match a pattern Int*Int^-1
// can match pow(6,-1)*ln(x)*4. We want 2/3.ln(x).
bool SimplificationIntegerDivision(Multiplication * multiplication, Integer * numerator, Power * power, Integer * denominator) {
  Integer gcd = Arithmetic::GCD(*numerator, *denominator);
  if (gcd.isEqualTo(Integer(1))) {
    return false;
  }
  IntegerDivision numeratorDivision(*numerator, gcd);
  assert(numeratorDivision.remainder().isEqualTo(Integer(0)));
  IntegerDivision denominatorDivision(*denominator, gcd);
  assert(denominatorDivision.remainder().isEqualTo(Integer(0)));

  multiplication->replaceOperand(numerator, numeratorDivision.quotient().clone());
  power->replaceOperand(denominator, denominatorDivision.quotient().clone());

  return true;
}

bool SimplificationIntegerAddition(Addition * addition, Integer * first, Integer * second) {
  Integer result = Integer::Addition(*first, *second);
  if (addition->numberOfOperands() == 2) {
    addition->parent()->replaceOperand(addition, result.clone());
  } else {
    assert(addition->numberOfOperands() > 2);
    addition->replaceOperand(first, result.clone());
    addition->removeOperand(second);
  }
  return true;
}

// cos(pi*3*x)
void SimplificationCosineIntegerPi(Cosine * cosine, Multiplication * multiplication, Integer * integer) {
  if (multiplication->numberOfChildren() != 2) {
    // We only handle cases with cos(Integer*Pi)
    return false;
  }
}

// OK, on fait une classe Start qui hérite de Expression
//
Expression * simplify() {
}


class Start : StaticHierarchy<1> {
public:
  Start(Expression * e) : StaticHierarchy<1>({e}, false);
  void simplify();
};

void maFonction() {
}

class SimplifiableExpression : public Expression {

};

void autreFonction() {
  Expression * e = Parse("1+2+3");
  Expresion * simplified = e->simplify();
}

class Expression {
  void simplify() {
    assert(type() == Start);
    //Expression * start = new Start(this);
    //return start->simplify()->firstChild();
  }
};
