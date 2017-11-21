#ifndef ION_CHARSET_H
#define ION_CHARSET_H

namespace Ion {

enum Charset : char {
  Integral = (char)127,
  XBar = (char)128,
  YBar = (char)130,
  CapitalGamma = (char)132,
  CapitalSigma = (char)133,
  SmallTheta = (char)134,
  SmallLambda = (char)135,
  SmallMu = (char)136,
  SmallPi = (char)137,
  SmallSigma = (char)138,
  IComplex = (char)139,
  Exponent = (char)140,
  Prime = (char)141,
  Exponential = (char)142,
  Sto = (char)143,
  Root = (char)144,
  LessEqual = (char)145,
  GreaterEqual = (char)146,
  MultiplicationSign = (char)147,
  MiddleDot = (char)148,
  AlmostEqual = (char)149
};

}

#endif
