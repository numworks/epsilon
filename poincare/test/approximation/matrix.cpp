#include "helper.h"

QUIZ_CASE(pcj_approximation_matrix) {
  approximates_to<float>("trace([[1,2][4,3]])", "4");
  approximates_to<float>("identity(3)", "[[1,0,0][0,1,0][0,0,1]]");
  approximates_to<float>("[[1,2][4,3]]+[[1,2][4,3]]*2*[[1,2][4,3]]",
                         "[[19,18][36,37]]");
  approximates_to<float>("([[0]]^(2π))^2×0×[[0]]^(2π)", "undef");
  approximates_to<float>("[[0]]^(2π)", "undef");
  approximates_to<float>("[[1,2]]^2", "undef");
  approximates_to<float>("[[0]]^2", "[[0]]");
  approximates_to<float>("det(inverse([[0]]))", "undef");
  approximates_to<float>("transpose(inverse([[0]]))", "undef");
  approximates_to<float>("dot([[1]], inverse([[0]]))", "undef");

  approximates_to<float>("[[1,2,3][4,5,6]]", "[[1,2,3][4,5,6]]");
  approximates_to<double>("[[1,2,3][4,5,6]]", "[[1,2,3][4,5,6]]");

  approximates_to<float>("trace([[1,2,3][4,5,6][7,8,9]])", "15");
  approximates_to<double>("trace([[1,2,3][4,5,6][7,8,9]])", "15");

  approximates_to<float>("dim([[1,2,3][4,5,-6]])", "[[2,3]]");
  approximates_to<double>("dim([[1,2,3][4,5,-6]])", "[[2,3]]");

  approximates_to<float>("det([[1,23,3][4,5,6][7,8,9]])", "126");
  approximates_to<double>("det([[1,23,3][4,5,6][7,8,9]])", "126");
  approximates_to<double>("det([[1,undef][4,6]])", "undef");
  approximates_to<float>("det([[i,23-2i,3×i][4+i,5×i,6][7,8×i+2,9]])",
                         "126-231×i", k_cartesianCtx);
  approximates_to<double>("det([[i,23-2i,3×i][4+i,5×i,6][7,8×i+2,9]])",
                          "126-231×i", k_cartesianCtx);

  approximates_to<float>("transpose([[1,2,3][4,5,-6][7,8,9]])",
                         "[[1,4,7][2,5,8][3,-6,9]]");
  approximates_to<float>("transpose([[1,7,5][4,2,8]])", "[[1,4][7,2][5,8]]");
  approximates_to<float>("transpose([[1,2][4,5][7,8]])", "[[1,4,7][2,5,8]]");
  approximates_to<double>("transpose([[1,2,3][4,5,-6][7,8,9]])",
                          "[[1,4,7][2,5,8][3,-6,9]]");
  approximates_to<double>("transpose([[1,7,5][4,2,8]])", "[[1,4][7,2][5,8]]");
  approximates_to<double>("transpose([[1,2][4,5][7,8]])", "[[1,4,7][2,5,8]]");

  approximates_to<double>("ref([[0,2,-1][5,6,7][10,11,10]])",
                          "[[1,1.1,1][0,1,-0.5][0,0,1]]");
  approximates_to<double>("rref([[0,2,-1][5,6,7][10,11,10]])",
                          "[[1,0,0][0,1,0][0,0,1]]");
  approximates_to<float>("ref([[0,2,-1][5,6,7][10,11,10]])",
                         "[[1,1.1,1][0,1,-0.5][0,0,1]]");
  approximates_to<float>("rref([[0,2,-1][5,6,7][10,11,10]])",
                         "[[1,0,0][0,1,0][0,0,1]]");
  approximates_to<float>("ref([[0,-1][undef,10]])", "undef");

  approximates_to<float>("cross([[1][2][3]],[[4][7][8]])", "[[-5][4][-1]]");
  approximates_to<double>("cross([[1][2][3]],[[4][7][8]])", "[[-5][4][-1]]");
  approximates_to<float>("cross([[1,2,3]],[[4,7,8]])", "[[-5,4,-1]]");
  approximates_to<double>("cross([[1,2,3]],[[4,7,8]])", "[[-5,4,-1]]");

  approximates_to<float>("dot([[1][2][3]],[[4][7][8]])", "42");
  approximates_to<double>("dot([[1][2][3]],[[4][7][8]])", "42");
  approximates_to<float>("dot([[1,2,3]],[[4,7,8]])", "42");
  approximates_to<double>("dot([[1,2,3]],[[4,7,8]])", "42");

  approximates_to<float>("norm([[-5][4][-1]])", "6.480741");
  approximates_to<double>("norm([[-5][4][-1]])", "6.4807406984079");
  approximates_to<float>("norm([[-5,4,-1]])", "6.480741");
  approximates_to<double>("norm([[-5,4,-1]])", "6.4807406984079");

  // inverse is not precise enough to display 7 significative digits
  approximates_to<float>(
      "inverse([[1,2,3][4,5,-6][7,8,9]])",
      "[[-1.2917,-0.083333,0.375][1.0833,0.16667,-0.25][0.041667,-0.083333,0."
      "041667]]",
      k_realCtx, 5);
  approximates_to<double>(
      "inverse([[1,2,3][4,5,-6][7,8,9]])",
      "[[-1.2916666666667,-0.083333333333333,0.375][1.0833333333333,0."
      "16666666666667,-0.25][0.041666666666667,-0.083333333333333,0."
      "041666666666667]]");
  // inverse is not precise enough to display 7 significative digits
  approximates_to<float>(
      "inverse([[i,23-2i,3×i][4+i,5×i,6][7,8×i+2,9]])",
      "[[-0.0118-0.0455×i,-0.5-0.727×i,0.318+0.489×i][0.0409+0.00364×i,0.04-0."
      "0218×i,-0.0255+9.1ᴇ-4×i][0.00334-0.00182×i,0.361+0.535×i,-0.13-0.358×i]"
      "]",
      k_cartesianCtx, 3);
  // FIXME: inverse is not precise enough to display 14 significative digits
  approximates_to<double>(
      "inverse([[i,23-2i,3×i][4+i,5×i,6][7,8×i+2,9]])",
      "[[-0.0118289353958-0.0454959053685×i,-0.500454959054-0.727024567789×i,0."
      "31847133758+0.488626023658×i][0.0409463148317+0.00363967242948×i,0."
      "0400363967243-0.0218380345769×i,-0.0254777070064+9.0991810737ᴇ-4×i][0."
      "00333636639369-0.00181983621474×i,0.36093418259+0.534728541098×i,-0."
      "130118289354-0.357597816197×i]]",
      k_cartesianCtx, 12);

  // We do not map on matrices anymore
  approximates_to<float>("abs([[1,-2][3,-4]])", "undef");
  approximates_to<double>("abs([[1,-2][3,-4]])", "undef");

  // Undefined
  approximates_to<float>("cross([[0]],[[0]])", "undef");
  approximates_to<float>("det(cross([[0]],[[0]]))", "undef");
  approximates_to<float>("dim(cross([[0]],[[0]]))", "undef");
  approximates_to<float>("inverse(cross([[0]],[[0]]))", "undef");
  approximates_to<float>("norm(cross([[0]],[[0]]))", "undef");
  approximates_to<float>("ref(cross([[0]],[[0]]))", "undef");
  approximates_to<float>("trace(cross([[0]],[[0]]))", "undef");
  approximates_to<float>("transpose(cross([[0]],[[0]]))", "undef");
}
