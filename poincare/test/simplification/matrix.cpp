#include "helper.h"

QUIZ_CASE(pcj_simplification_matrix) {
  simplifies_to("[[1+2]]", "[[3]]");
  simplifies_to("trace([[1,2][3,4]])", "5");
  simplifies_to("identity(2)", "[[1,0][0,1]]");
  simplifies_to("identity(0)", "undef");
  simplifies_to("identity(256)", "undef");
  simplifies_to("identity(-3)", "undef");
  simplifies_to("identity(floor(π)-round(3/2))", "undef");
  simplifies_to("transpose([[1][3]])", "[[1,3]]");
  simplifies_to("transpose([[1,2][3,4]])", "[[1,3][2,4]]");
  simplifies_to("dim([[1,2][3,4][5,6]])", "[[3,2]]");
  simplifies_to("ref([[1,2][3,4]])", "[[1,4/3][0,1]]");
  simplifies_to("rref([[1,2][3,4]])", "[[1,0][0,1]]");
  simplifies_to("ref([[0,2,-1][5,6,7][12,11,10]])",
                "[[1,11/12,5/6][0,1,-1/2][0,0,1]]");
  simplifies_to("det([[0,2,-1][5,6,7][12,11,10]])", "85");
  simplifies_to("det([[1,2][3,4]])", "-2");
  simplifies_to("inverse([[1,2][3,4]])", "[[-2,1][3/2,-1/2]]");
  simplifies_to("[[1,2][3,4]]^5", "[[1069,1558][2337,3406]]");
  simplifies_to("[[1,2][3,4]]^-1", "[[-2,1][3/2,-1/2]]");
  simplifies_to("[[1,2][3,4]]^0 - identity(2)", "[[0,0][0,0]]");
  simplifies_to("trace(identity(3))", "3");
  simplifies_to("2+[[3]]", "undef");
  simplifies_to("[[2]]+[[3]]", "[[5]]");
  simplifies_to("2×[[3]]", "[[6]]");
  simplifies_to("[[3]]×i", "[[3×i]]");
  simplifies_to("[[1,2][3,4]]×[[2,3][4,5]]", "[[10,13][22,29]]");
  simplifies_to("norm([[2,3,6]])", "7");
  simplifies_to("dot([[1,2,3]],[[4,5,6]])", "32");
  simplifies_to("cross([[1,2,3]],[[4,5,6]])", "[[-3,6,-3]]");
  simplifies_to("trace([[√(2)+log(3)]])", "√(2)+log(3)");
  simplifies_to("transpose([[√(4)]])", "[[2]]");
  simplifies_to("[[0]]*[[0]]", "[[0]]");
  simplifies_to("sum([[k,0]],k,0,5)", "[[15,0]]");
  simplifies_to("sum([[k,0]],k,5,0)", "[[0,0]]");
  simplifies_to("product([[0,k][k,0]],k,1,5)", "[[0,120][120,0]]");
  simplifies_to("product([[0,k][k,0]],k,5,1)", "[[1,0][0,1]]");
  simplifies_to("det([[b]])", "det([[b]])");

  simplifies_to("0*[[2][4]]×[[1,2]]", "[[0,0][0,0]]");
  simplifies_to("0*[[2][inf]]", "[[0][undef]]");
  simplifies_to("0*inf*[[2][3]]", "undef");
  simplifies_to("undef*[[2][4]]", "undef");
  simplifies_to("[[1/0][4]]", "[[undef][4]]");
  simplifies_to("[[1,2]]^2", "undef");
  simplifies_to("[[1]]^π", "undef");

  simplifies_to("transpose(undef)", "undef");
  simplifies_to("transpose(identity(0))", "undef");
  simplifies_to("transpose(identity(-1))", "undef");
  simplifies_to("transpose(inverse([[0]]))", "undef");
  simplifies_to("dot([[1]], inverse([[0]]))", "undef");
  simplifies_to("log(det([[0]]^2))", "undef");
}

QUIZ_CASE(pcj_simplification_functions_of_matrices) {
  simplifies_to("abs([[1,-1][2,-3]])", "undef");
  simplifies_to("acos([[1/√(2),1/2][1,-1]])", "undef");
  simplifies_to("asin([[1,0]])", "undef");
  simplifies_to("binomial([[0,180]],1)", "undef");
  simplifies_to("binomial(1,[[0,180]])", "undef");
  simplifies_to("binomial([[0,180]],[[1]])", "undef");
  simplifies_to("ceil([[0.3,180]])", "undef");
  simplifies_to("arg([[1,1+i]])", "undef");
  simplifies_to("conj([[1,1+i]])", "undef");
  simplifies_to("cos([[π/3,0][π/7,π/2]])", "undef");
  simplifies_to("cos([[0,π]])", "undef");
  simplifies_to("diff([[0,180]],x,1)", "undef");
  simplifies_to("diff(1,x,[[0,180]])", "undef");
  simplifies_to("quo([[0,180]],1)", "undef");
  simplifies_to("quo(1,[[0,180]])", "undef");
  simplifies_to("quo([[0,180]],[[1]])", "undef");
  simplifies_to("rem([[0,180]],1)", "undef");
  simplifies_to("rem(1,[[0,180]])", "undef");
  simplifies_to("rem([[0,180]],[[1]])", "undef");
  simplifies_to("factor([[0,180]])", "undef");
  simplifies_to("[[1,3]]!", "undef");
  simplifies_to("floor([[1/√(2),1/2][1,-1.3]])", "undef");
  simplifies_to("frac([[0.3,180]])", "undef");
  simplifies_to("gcd([[0,180]],1)", "undef");
  simplifies_to("gcd(1,[[0,180]])", "undef");
  simplifies_to("gcd([[0,180]],[[1]])", "undef");
  simplifies_to("gcd(1,2,[[1]])", "undef");
  simplifies_to("arsinh([[0,π]])", "undef");
  simplifies_to("artanh([[0,π]])", "undef");
  simplifies_to("sinh([[0,π]])", "undef");
  simplifies_to("im([[1,1+i]])", "undef");
  simplifies_to("int([[0,180]],x,1,2)", "undef");
  simplifies_to("int(1,x,[[0,180]],1)", "undef");
  simplifies_to("int(1,x,1,[[0,180]])", "undef");
  simplifies_to("log([[2,3]])", "undef");
  simplifies_to("log(5,[[2,3]])", "undef");
  simplifies_to("ln([[2,3]])", "undef");
  simplifies_to("root([[2,3]],5)", "undef");
  simplifies_to("root(5,[[2,3]])", "undef");
  // Opposite is mapped on matrix
  simplifies_to("-[[1/√(2),1/2,3][2,1,-3]]", "[[-√(2)/2,-1/2,-3][-2,-1,3]]");
  simplifies_to("permute([[2,3]],5)", "undef");
  simplifies_to("permute(5,[[2,3]])", "undef");
  simplifies_to("product(1,x,[[0,180]],1)", "undef");
  simplifies_to("product(1,x,1,[[0,180]])", "undef");
  simplifies_to("re([[1,i]])", "undef");
  simplifies_to("round(1.3, [[2.1,3.4]])", "undef");
  simplifies_to("round(1.3, [[2.1,3.4]])", "undef");
  simplifies_to("sign([[2.1,3.4]])", "undef");
  simplifies_to("sin([[π/3,0][π/7,π/2]])", "undef");
  simplifies_to("sum(1,x,[[0,180]],1)", "undef");
  simplifies_to("sum(1,x,1,[[0,180]])", "undef");
  simplifies_to("√([[2.1,3.4]])", "undef");
  simplifies_to("[[2,3.4]]-[[0.1,3.1]]", "[[19/10,3/10]]");
  simplifies_to("[[2,3.4]]-1", "undef");
  simplifies_to("1-[[0.1,3.1]]", "undef");
}

QUIZ_CASE(pcj_simplification_store_matrix) {
  simplifies_to("1+1→a", "2→a");
  simplifies_to("[[8]]→f(x)", "[[8]]→f(x)");
  simplifies_to("[[x]]→f(x)", "[[x]]→f(x)");
}
