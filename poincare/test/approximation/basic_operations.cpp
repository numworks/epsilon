#include "helper.h"

QUIZ_CASE(pcj_approximation_addition) {
  approximates_to<float>("1+2", 3.0f);
  approximates_to<double>("i+i", NAN);
  approximates_to<float>("1+2", "3", k_cartesianCtx);
  approximates_to<float>("i+i", "2×i", k_cartesianCtx);
  approximates_to<double>("2+i+4+i", "6+2×i", k_cartesianCtx);

  // Matrices
  approximates_to<float>("[[1,2][3,4][5,6]]+3", "undef");
  approximates_to<double>("[[1,2+i][3,4][5,6]]+3+i", "undef", k_cartesianCtx);
  approximates_to<float>("3+[[1,2][3,4][5,6]]", "undef");
  approximates_to<double>("3+i+[[1,2+i][3,4][5,6]]", "undef", k_cartesianCtx);
  approximates_to<float>("[[1,2][3,4][5,6]]+[[1,2][3,4][5,6]]",
                         "[[2,4][6,8][10,12]]");
  approximates_to<double>("[[1,2+i][3,4][5,6]]+[[1,2+i][3,4][5,6]]",
                          "[[2,4+2×i][6,8][10,12]]", k_cartesianCtx);
  approximates_to<double>("0×sum([[0]],k,2,4)^2", "[[0]]");
  approximates_to<double>("sum([[k,2]]*k,k,2,4)", "[[29,18]]");

  // Lists
  approximates_to<float>("{1,2,3}+10", "{11,12,13}");
  approximates_to<float>("10+{1,2,3}", "{11,12,13}");
  approximates_to<float>("{1,2,3}+{4,5,6}", "{5,7,9}");
  approximates_to<float>("{1,2,3}+{4,5}", "undef");
  approximates_to<float>("{1,2,3}+[[4,5,6]]", "undef");
  approximates_to<double>("{1,2,3}+10", "{11,12,13}");
  approximates_to<double>("10+{1,2,3}", "{11,12,13}");
  approximates_to<double>("{1,2,3}+{4,5,6}", "{5,7,9}");
  approximates_to<double>("{1,2,3}+{4,5}", "undef");
  approximates_to<double>("{1,2,3}+[[4,5,6]]", "undef");
}

QUIZ_CASE(pcj_approximation_multiplication) {
  approximates_to<float>("1×2", 2.0f);
  approximates_to<double>("(3+i)×(4+i)", NAN);
  approximates_to<float>("1×2", "2");
  approximates_to<double>("(3+i)×(4+i)", "11+7×i", k_cartesianCtx);

  // Matrices
  approximates_to<float>("[[1,2][3,4][5,6]]×2", "[[2,4][6,8][10,12]]");
  approximates_to<double>("[[1,2+i][3,4][5,6]]×(3+i)",
                          "[[3+i,5+5×i][9+3×i,12+4×i][15+5×i,18+6×i]]",
                          k_cartesianCtx);
  approximates_to<float>("2×[[1,2][3,4][5,6]]", "[[2,4][6,8][10,12]]");
  approximates_to<double>("(3+i)×[[1,2+i][3,4][5,6]]",
                          "[[3+i,5+5×i][9+3×i,12+4×i][15+5×i,18+6×i]]",
                          k_cartesianCtx);
  approximates_to<float>("[[1,2][3,4][5,6]]×[[1,2,3,4][5,6,7,8]]",
                         "[[11,14,17,20][23,30,37,44][35,46,57,68]]");
  approximates_to<double>(
      "[[1,2+i][3,4][5,6]]×[[1,2+i,3,4][5,6+i,7,8]]",
      "[[11+5×i,13+9×i,17+7×i,20+8×i][23,30+7×i,37,44][35,46+11×i,57,68]]",
      k_cartesianCtx);
  approximates_to<double>("product([[k,2][-2,0]]*k,k,2,4)",
                          "[[0,96][-384,-288]]");

  // Lists
  approximates_to<float>("{1,2,3}×10", "{10,20,30}");
  approximates_to<float>("10×{1,2,3}", "{10,20,30}");
  approximates_to<float>("{1,2,3}×{4,5,6}", "{4,10,18}");
  approximates_to<float>("{1,2,3}×{4,5}", "undef");
  approximates_to<float>("{1,2,3}×[[4,5,6]]", "undef");
  approximates_to<double>("{1,2,3}×10", "{10,20,30}");
  approximates_to<double>("10×{1,2,3}", "{10,20,30}");
  approximates_to<double>("{1,2,3}×{4,5,6}", "{4,10,18}");
  approximates_to<double>("{1,2,3}×{4,5}", "undef");
  approximates_to<double>("{1,2,3}×[[4,5,6]]", "undef");
}

QUIZ_CASE(pcj_approximation_subtraction) {
  approximates_to<float>("1-2", -1.0f);
  approximates_to<double>("(1)-(4+i)", NAN);
  approximates_to<float>("[[1,2][3,4][5,6]]-[[3,2][3,4][5,6]]",
                         "[[-2,0][0,0][0,0]]");
  approximates_to<float>("1-2", "-1");
  approximates_to<double>("3+i-(4+i)", "-1", k_cartesianCtx);

  // Matrices
  approximates_to<float>("[[1,2][3,4][5,6]]-3", "undef");
  approximates_to<double>("[[1,2+i][3,4][5,6]]-(4+i)", "undef", k_cartesianCtx);
  approximates_to<float>("3-[[1,2][3,4][5,6]]", "undef");
  approximates_to<double>("3+i-[[1,2+i][3,4][5,6]]", "undef", k_cartesianCtx);
  approximates_to<float>("[[1,2][3,4][5,6]]-[[6,5][4,3][2,1]]",
                         "[[-5,-3][-1,1][3,5]]");
  approximates_to<double>("[[1,2+i][3,4][5,6]]-[[1,2+i][3,4][5,6]]",
                          "[[0,0][0,0][0,0]]", k_cartesianCtx);

  // Lists
  approximates_to<float>("{1,2,3}-10", "{-9,-8,-7}");
  approximates_to<float>("10-{1,2,3}", "{9,8,7}");
  approximates_to<float>("{1,2,3}-{4,5,6}", "{-3,-3,-3}");
  approximates_to<float>("{1,2,3}-{4,5}", "undef");
  approximates_to<float>("{1,2,3}-[[4,5,6]]", "undef");
  approximates_to<double>("{1,2,3}-10", "{-9,-8,-7}");
  approximates_to<double>("10-{1,2,3}", "{9,8,7}");
  approximates_to<double>("{1,2,3}-{4,5,6}", "{-3,-3,-3}");
  approximates_to<double>("{1,2,3}-{4,5}", "undef");
  approximates_to<double>("{1,2,3}-[[4,5,6]]", "undef");
}

QUIZ_CASE(pcj_approximation_division) {
  approximates_to<float>("1/2", 0.5f);
  approximates_to<float>("(3+i)/(4+i)", NAN);
  approximates_to<float>("1/2", "0.5");
  approximates_to<double>("(3+i)/(4+i)", "0.76470588235294+0.058823529411765×i",
                          k_cartesianCtx);
  approximates_to<float>("1ᴇ20/(1ᴇ20+1ᴇ20i)", "0.5-0.5×i", k_cartesianCtx);
  approximates_to<double>("1ᴇ155/(1ᴇ155+1ᴇ155i)", "0.5-0.5×i", k_cartesianCtx);

  // Matrices
  approximates_to<float>("[[1,2][3,4][5,6]]/2", "[[0.5,1][1.5,2][2.5,3]]");
  approximates_to<double>(
      "[[1,2+i][3,4][5,6]]/(1+i)",
      "[[0.5-0.5×i,1.5-0.5×i][1.5-1.5×i,2-2×i][2.5-2.5×i,3-3×i]]",
      k_cartesianCtx);
  approximates_to<float>("[[1,2][3,4][5,6]]/2", "[[0.5,1][1.5,2][2.5,3]]");
  approximates_to<double>("[[1,2][3,4]]/[[3,4][6,9]]", "undef");
  approximates_to<double>("3/[[3,4][5,6]]", "undef");
  // approximates_to<double>("(3+4i)/[[3,4][1,i]]",
  // "[[1,4×i][i,-3×i]]");
  /* TODO_PCJ: this test fails because of negligible real or imaginary parts.
   * It currently approximates to
   * [[1+5.5511151231258ᴇ-17×i,-2.2204460492503ᴇ-16+4×i][i,-3×i]] or
   * [[1-1.1102230246252ᴇ-16×i,2.2204460492503ᴇ-16+4×i]
   *  [-1.1102230246252ᴇ-16+i,-2.2204460492503ᴇ-16-3×i]] on Linux */

  // Lists
  approximates_to<float>("{1,2,3}/10", "{0.1,0.2,0.3}");
  approximates_to<float>("10/{1,2,4}", "{10,5,2.5}");
  approximates_to<float>("{12,100,1}/{4,2,1}", "{3,50,1}");
  approximates_to<float>("{1,2,3}/{4,5}", "undef");
  approximates_to<float>("{1,2,3}/[[4,5,6]]", "undef");
  approximates_to<double>("{1,2,3}/10", "{0.1,0.2,0.3}");
  approximates_to<double>("10/{1,2,4}", "{10,5,2.5}");
  approximates_to<double>("{12,100,1}/{4,2,1}", "{3,50,1}");
  approximates_to<double>("{1,2,3}/{4,5}", "undef");
  approximates_to<double>("{1,2,3}/[[4,5,6]]", "undef");

  // Euclidean division
  approximates_to<float>("quo(23,12)", 1);
  approximates_to<float>("rem(23,12)", 11);
  approximates_to<float>("quo(-23,12)", -2);
  approximates_to<float>("rem(-23,12)", 1);
  approximates_to<float>("quo(23,-12)", -1);
  approximates_to<float>("rem(23,-12)", 11);
  approximates_to<float>("quo(-23,-12)", 2);
  approximates_to<float>("rem(-23,-12)", 1);
}

QUIZ_CASE(pcj_approximation_power) {
  approximates_to<float>("0^0", "undef");
  approximates_to<float>("0^0.5", "0");
  approximates_to<float>("0^2", "0");
  approximates_to<float>("0^(-0.5)", "undef");
  approximates_to<float>("0^(-2)", "undef");
  approximates_to<float>("0^(-2.5)", "undef");
  approximates_to<float>("0^(-3)", "undef");
  approximates_to<float>("(-1)^(-2024)", "1");
  approximates_to<float>("(-1)^(-2025)", "-1");
  approximates_to<float>("(-1)^(pi)", "nonreal");
  approximates_to<float>("(-2.5)^(-3.14)", "nonreal");
  approximates_to<float>("0^i", "undef", k_cartesianCtx);
  approximates_to<float>("0^(3+4i)", "0", k_cartesianCtx);
  approximates_to<float>("0^(3-4i)", "0", k_cartesianCtx);
  approximates_to<float>("0^(-3+4i)", "undef", k_cartesianCtx);
  approximates_to<float>("0^(-3-4i)", "undef", k_cartesianCtx);
  approximates_to<float>("1-e^(-90000000/563)", "1");
  approximates_to<float>("√(888888)", "942.8086");
  approximates_to<float>("888888^(.5)", "942.8086");
  approximates_to<float>("√(-888888)", "942.8086×i", k_cartesianCtx);
  approximates_to<float>("1/(0.752^-0.245)", 0.9325527);
  approximates_to<float>("1/(0.75^-0.245)", 0.9319444);
#if 0
  // TODO_PCJ: should be a pure imaginary
  approximates_to<float>("(-888888)^(.5)", "7.118005ᴇ-5+942.8084×i",
                         k_cartesianCtx);
#endif

  approximates_to<float>("2^3", 8.0f);
  approximates_to<double>("(3+i)^(4+i)", NAN, k_cartesianCtx);
  approximates_to<float>("[[1,2][3,4]]^2", "[[7,10][15,22]]");

  approximates_to<float>("2^3", "8");
  approximates_to<double>("(3+i)^4", "28+96×i", k_cartesianCtx);
  approximates_to<float>("4^(3+i)", "11.74125+62.91378×i", k_cartesianCtx);
  approximates_to<double>("(3+i)^(3+i)", "-11.898191759852+19.592921596609×i",
                          k_cartesianCtx);
  approximates_to<double>("(-2)^4.2", "14.86906384965+10.803007238398×i",
                          k_cartesianCtx);
  approximates_to<double>("(-0.1)^4", "1ᴇ-4", k_cartesianCtx);
  approximates_to<double>("i^i", "0.20787957635076", k_cartesianCtx);
  approximates_to<float>("1.0066666666667^60", "1.489845");
  approximates_to<double>("1.0066666666667^60", "1.4898457083046");
  approximates_to<double>("1.0092^50", "1.5807460027336");
  approximates_to<float>("1.0092^50", "1.580744");
  // TODO_PCJ: approximation of exp(i×π) != approximation of e^(i×π)
  approximates_to<float>("e^(i×π)", "-1", k_cartesianCtx);
  approximates_to<double>("e^(i×π)", "-1", k_cartesianCtx);
  // -7.389054 on simulator, -7.389056 on device
  approximates_to<float>("e^(i×π+2)", "-7.38906", k_cartesianCtx, 6);
  approximates_to<double>("e^(i×π+2)", "-7.3890560989307", k_cartesianCtx);
  approximates_to<double>("(-1)^2", "1");
  approximates_to<double>("(-1)^3", "-1");
  approximates_to<float>("(-1)^(1/3)", "0.5+0.8660254×i", k_cartesianCtx);
  approximates_to<double>("(-1)^(1/3)", "0.5+0.86602540378444×i",
                          k_cartesianCtx);
  // 0.5000001+0.8660254×i on simulator, 0.5+0.8660254×i on device
  approximates_to<float>("e^(i×π/3)", "0.5+0.866025×i", k_cartesianCtx, 6);
  approximates_to<double>("e^(i×π/3)", "0.5+0.86602540378444×i",
                          k_cartesianCtx);
  approximates_to<float>("i^(2/3)", "0.5+0.8660254×i", k_cartesianCtx);
  approximates_to<double>("i^(2/3)", "0.5+0.86602540378444×i", k_cartesianCtx);
  approximates_to<double>("1/cos(-1+50×i)",
                          "2.0842159805955ᴇ-22-3.2459740680286ᴇ-22×i",
                          k_cartesianCtx);
  approximates_to<float>("(-10)^0.00000001", "nonreal");
  approximates_to<float>("(-10)^0.00000001", "1+3.141593ᴇ-8×i", k_cartesianCtx);
  projected_approximates_to<float>("3.5^2.0000001", "12.25");
  projected_approximates_to<float>("3.7^2.0000001", "13.69");
  projected_approximates_to<double>("(13619-(185477161)^(1/2))^(-1)", "undef");

  approximates_to<float>("1.2×e^(1)", "3.261938");
  // WARNING: the 7th significant digit is wrong on blackbos simulator
  approximates_to<float>("2e^(3)", "40.1711", k_realCtx, 6);
  // WARNING: the 7th significant digit is wrong on simulator
  approximates_to<float>("e^2×e^(1)", "20.0855", k_realCtx, 6);
  approximates_to<double>("e^2×e^(1)", "20.085536923188");
  approximates_to<double>("2×3^4+2", "164");
  approximates_to<float>("-2×3^4+2", "-160");
  simplified_approximates_to<double>("1.0092^(20)", "1.2010050593402");
  simplified_approximates_to<double>("1.0092^(50)×ln(3/2)", "0.6409373488899",
                                     k_realCtx, 13);
  simplified_approximates_to<double>("1.0092^(50)×ln(1.0092)",
                                     "0.01447637354655", k_realCtx, 13);
  approximates_to<double>("1.0092^(20)", "1.2010050593402");
  approximates_to<double>("1.0092^(50)×ln(3/2)", "0.6409373488899", k_realCtx,
                          13);
  approximates_to<double>("1.0092^(50)×ln(1.0092)", "0.01447637354655",
                          k_realCtx, 13);
  simplified_approximates_to<double>("1.0092^(20)", "1.2010050593402");
  simplified_approximates_to<double>("1.0092^(50)×ln(3/2)", "0.6409373488899",
                                     k_realCtx, 13);
  approximates_to<float>("1.0092^(50)×ln(3/2)", "0.6409366");
  approximates_to<float>("1.0092^(20)", "1.201005");
  // TODO_PCJ: failing power tests
  // simplified_approximates_to<float>("1.0092^(20)", "1.2010050593402");
  // simplified_approximates_to<float>("1.0092^(50)×ln(3/2)",
  //                                   "6.4093734888993ᴇ-1");

  // Lists
  approximates_to<float>("{1,2,3}^2", "{1,4,9}");
  approximates_to<float>("2^{1,2,3}", "{2,4,8}");
  approximates_to<float>("{1,2,3}^{1,2,3}", "{1,4,27}");
  approximates_to<float>("{1,2,3}^{4,5}", "undef");
  approximates_to<float>("{1,2,3}^[[4,5,6]]", "undef");
  approximates_to<double>("{1,2,3}^2", "{1,4,9}");
  approximates_to<double>("2^{1,2,3}", "{2,4,8}");
  approximates_to<double>("{1,2,3}^{1,2,3}", "{1,4,27}");
  approximates_to<double>("{1,2,3}^{4,5}", "undef");
  approximates_to<double>("{1,2,3}^[[4,5,6]]", "undef");
}
