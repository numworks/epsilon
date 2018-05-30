#ifndef ION_CHARSET_H
#define ION_CHARSET_H

namespace Ion {

enum Charset : char {
  Integral = (char)127,
  XBar = (char)128,
  YBar = (char)130,
  CapitalGamma = (char)132,
  CapitalDelta = (char)133,
  CapitalSigma = (char)134,
  SmallTheta = (char)135,
  SmallLambda = (char)136,
  SmallMu = (char)137,
  SmallPi = (char)138,
  SmallSigma = (char)139,
  IComplex = (char)140,
  Exponent = (char)141,
  Prime = (char)142,
  Exponential = (char)143,
  Sto = (char)144,
  Root = (char)145,
  LessEqual = (char)146,
  GreaterEqual = (char)147,
  MultiplicationSign = (char)148,
  MiddleDot = (char)149,
  AlmostEqual = (char)150,
  Empty = (char)151 // This char is used to be parsed into EmptyExpression.
};

}

#endif
