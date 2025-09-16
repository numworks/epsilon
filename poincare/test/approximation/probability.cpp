#include "helper.h"

QUIZ_CASE(pcj_approximation_probability) {
  // FIXME: precision problem
  approximates_to<float>("binomcdf(5.3, 9, 0.7)", "0.270341", k_realCtx, 6);
  // FIXME precision problem
  approximates_to<double>("binomcdf(5.3, 9, 0.7)", "0.270340902", k_realCtx,
                          10);
  // FIXME: precision problem
  approximates_to<float>("binompdf(4.4, 9, 0.7)", "0.0735138", k_realCtx, 6);
  approximates_to<double>("binompdf(4.4, 9, 0.7)", "0.073513818");
  approximates_to<float>("invbinom(0.9647324002, 15, 0.7)", "13");
  approximates_to<double>("invbinom(0.9647324002, 15, 0.7)", "13");
  approximates_to<float>("invbinom(0.95,100,0.42)", "50");
  approximates_to<double>("invbinom(0.95,100,0.42)", "50");
  approximates_to<float>("invbinom(0.01,150,0.9)", "126");
  approximates_to<double>("invbinom(0.01,150,0.9)", "126");

  approximates_to<double>("geompdf(1,1)", "1");
  approximates_to<double>("geompdf(2,0.5)", "0.25");
  approximates_to<double>("geompdf(2,1)", "0");
  approximates_to<double>("geompdf(1,0)", "undef");
  approximates_to<double>("geomcdf(2,0.5)", "0.75");
  approximates_to<double>("geomcdfrange(2,3,0.5)", "0.375");
  approximates_to<double>("geomcdfrange(2,2,0.5)", "0.25");
  approximates_to<double>("invgeom(1,1)", "1");
  approximates_to<double>("invgeom(0.825,0.5)", "3");

  approximates_to<double>("hgeompdf(-1,2,1,1)", "0");
  approximates_to<double>("hgeompdf(0,2,1,1)", "0.5");
  approximates_to<double>("hgeompdf(1,2,1,1)", "0.5");
  approximates_to<double>("hgeompdf(2,2,1,1)", "0");
  approximates_to<double>("hgeompdf(3,2,1,1)", "0");
  approximates_to<double>("hgeompdf(0,2,1,2)", "0");
  approximates_to<double>("hgeompdf(1,2,1,2)", "1");
  approximates_to<double>("hgeompdf(2,2,1,2)", "0");
  approximates_to<double>("hgeompdf(0,42,0,42)", "1");
  approximates_to<double>("hgeompdf(24,42,24,42)", "1");
  approximates_to<double>("hgeomcdf(1,4,2,3)", "0.5");
  approximates_to<double>("hgeomcdf(24,42,24,34)", "1");
  approximates_to<double>("hgeomcdfrange(2,3,6,3,4)", "0.8");
  approximates_to<double>("hgeomcdfrange(13,15,40,20,30)", "0.60937014162821");
  approximates_to<double>("invhgeom(.5,4,2,3)", "1");
  approximates_to<double>("invhgeom(.6,40,20,30)", "15");
  approximates_to<double>("invhgeom(-1,4,2,3)", "undef");
  approximates_to<double>("invhgeom(0,4,2,2)", "undef");
  approximates_to<double>("invhgeom(0,4,2,3)", "0");
  approximates_to<double>("invhgeom(1,4,2,3)", "2");

  approximates_to<double>("normcdf(5, 7, 0.3162)", "1.265256ᴇ-10", k_realCtx,
                          7);
  approximates_to<float>("normcdf(1.2, 3.4, 5.6)", "0.3472125");
  approximates_to<double>("normcdf(1.2, 3.4, 5.6)", "0.34721249841587");
  approximates_to<float>("normcdf(-1ᴇ99,3.4,5.6)", "0");
  approximates_to<float>("normcdf(1ᴇ99,3.4,5.6)", "1");
  approximates_to<float>("normcdf(-6,0,1)", "0");
  approximates_to<float>("normcdf(6,0,1)", "1");
  approximates_to<float>("normcdfrange(0.5, 3.6, 1.3, 3.4)", "0.3436388");
  approximates_to<double>("normcdfrange(0.5, 3.6, 1.3, 3.4)",
                          "0.34363881299147");
  approximates_to<float>("normpdf(1.2, 3.4, 5.6)", "0.06594901");
  approximates_to<float>("invnorm(0.56, 1.3, 2.4)", "1.662326");
  // FIXME precision error
  // approximates_to<double>("invnorm(0.56, 1.3, 2.4)", "1.6623258450088");

  approximates_to<float>("poissonpdf(2,1)", "0.1839397");
  approximates_to<double>("poissonpdf(2,1)", "0.18393972058572");
  approximates_to<float>("poissonpdf(2,2)", "0.2706706");
  approximates_to<float>("poissoncdf(2,2)", "0.6766764");
  approximates_to<double>("poissoncdf(2,2)", "0.67667641618306");

  approximates_to<float>("tpdf(1.2, 3.4)", "0.1706051");
  approximates_to<double>("tpdf(1.2, 3.4)", "0.17060506917323");
  approximates_to<float>("tcdf(0.5, 2)", "0.6666667");
  approximates_to<float>("tcdf(1.2, 3.4)", "0.8464878");
  // FIXME: precision problem
  approximates_to<double>("tcdf(1.2, 3.4)", "0.8464877995", k_realCtx, 10);
  approximates_to<float>("invt(0.8464878,3.4)", "1.2");
  approximates_to<double>("invt(0.84648779949601043,3.4)", "1.2", k_realCtx,
                          10);
}

QUIZ_CASE(pcj_approximation_combinatorics) {
  approximates_to<float>("binomial(10, 4)", "210");
  approximates_to<double>("binomial(10, 4)", "210");
  approximates_to<float>("binomial(12, 3)", "220");
  approximates_to<double>("binomial(12, 3)", "220");
  approximates_to<float>("binomial(-4.6, 3)", "-28.336");
  approximates_to<double>("binomial(-4.6, 3)", "-28.336");
  approximates_to<float>("binomial(π, 3)", "1.280108");
  approximates_to<double>("binomial(π, 3)", "1.2801081307019");
  approximates_to<float>("binomial(7, 9)", "0");
  approximates_to<double>("binomial(7, 9)", "0");
  approximates_to<float>("binomial(-7, 9)", "-5005");
  approximates_to<double>("binomial(-7, 9)", "-5005");
  approximates_to<float>("binomial(13, 0)", "1");
  approximates_to<double>("binomial(13, 0)", "1");
  approximates_to<float>("binomial(10, -1)", "0");
  approximates_to<double>("binomial(10, -1)", "0");
  approximates_to<float>("binomial(-5, -10)", "0");
  approximates_to<double>("binomial(-5, -10)", "0");
  approximates_to<float>("binomial(10, 2.1)", "undef");
  approximates_to<double>("binomial(10, 2.1)", "undef");

  approximates_to<float>("permute(10, 4)", "5040");
  approximates_to<double>("permute(10, 4)", "5040");
}

QUIZ_CASE(pcj_approximation_random) {
  /* All random nodes need to be at least projected so they can be seeded.
   * Randint of list also needs to be simplified to bubble up the list */
  simplified_approximates_to<double>("randint({1,1})", "{1,1}");
  simplified_approximates_to<double>("randint({1,2},{1,2})", "{1,2}");
  projected_approximates_to<double>("1/randint(2,2)+1/2", "1");
  projected_approximates_to<double>("randint(45,4)", "undef");
  projected_approximates_to<double>("randint(1, inf)", "undef");
  projected_approximates_to<double>("randint(-inf, 3)", "undef");
  projected_approximates_to<double>("randint(4, 3)", "undef");
#if 0  // TODO_PCJ: limit randint bounds
  projected_approximates_to<double>("randint(2, 23345678909876545678)",
                                    "undef");
#endif

  projected_approximates_to<double>("randintnorep(10,1,3)", "undef");
  projected_approximates_to<double>("randintnorep(1,10,100)", "undef");
  projected_approximates_to<double>("randintnorep(1,10,-1)", "undef");
  projected_approximates_to<double>("randintnorep(1,10,0)", "{}");
  projected_approximates_to<double>("randintnorep(536427840,-2145711360,4)",
                                    "undef");

  // Random lists can be sorted
  projected_approximates_to<float>("sort(randintnorep(1,4,4))", "{1,2,3,4}");
  projected_approximates_to<double>("sort(randintnorep(5,8,4))", "{5,6,7,8}");

  /* The simplification process should understand that the expression is not a
   * scalar if it encounters a randintnorep. */
  simplified_approximates_to<double>("rem(randintnorep(1,10,5),1)",
                                     "{0,0,0,0,0}");
}
