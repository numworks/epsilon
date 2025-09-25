#include "helper.h"

QUIZ_CASE(pcj_approximation_parametrics) {
  approximates_to<float>("sum(k/2, k, 1, 12)", "39");
  approximates_to<float>("sum(r,r, 4, 10)", "49");
  approximates_to<double>("sum(k,k, 4, 10)", "49");
  approximates_to<double>("sum(2+n×i,n,1,5)", "10+15×i", k_cartesianCtx);
  approximates_to<double>("sum(2+n×i,n,1,5)", "10+15×i", k_cartesianCtx);

  approximates_to<float>("product(n,n, 4, 10)", "604800");
  approximates_to<double>("product(n,n, 4, 10)", "604800");
  approximates_to<float>("product(2+k×i,k, 1, 5)", "-100-540×i",
                         k_cartesianCtx);
  approximates_to<double>("product(2+o×i,o, 1, 5)", "-100-540×i",
                          k_cartesianCtx);
}

QUIZ_CASE(pcj_approximation_integrals) {
  approximates_to<float>("int(x, x, 1, 2)", "1.5");
  approximates_to<double>("int(e^x, x, 0, 200)", "7.2259737681256ᴇ86");
  approximates_to<double>("int(e^(x^3), x, 0, 4)", "1.312888495564ᴇ26");
  approximates_to<double>("int(√(x^2-3)/x, x, √(3), 2)", "0.093100317882891");

  approximates_to<double>("int(710×x×e^(-710×x), x, 0, 10)",
                          "0.0014084507042254");
  approximates_to<double>("710*int(x×e^(-710×x), x, 0, 10)",
                          "0.0014084507042254");
  approximates_to<float>("int(710×x×e^(-710×x), x, 0, 10)", "0.001408451");
  approximates_to<float>("710*int(x×e^(-710×x), x, 0, 10)", "0.001408451");
  approximates_to<double>("int(√(5-x^2), x, -√(5), √(5))", "7.8539816339745");
  approximates_to<float>("int(2*(2x-2)^3,x,1,2)", "4");

  simplified_approximates_to<double>("int(3x^2+x-4,x,-2,2)", "0");

  // Double integration
  approximates_to<float>("int(int(t^2,t,0,x),x,0,4)", "21.33333");
  approximates_to<float>("int(int(t,t,0,x),x,0,1)", "0.1666667");

  approximates_to<float>("int(x,x, 1, 2)", "1.5");
  approximates_to<double>("int(x,x, 1, 2)", "1.5");
  approximates_to<float>("int(1/x,x,0,1)", "undef");

  // Ensure this does not take up too much time
  approximates_to<double>("int(e^(x^3),x,0,6)", "5.9639380918976ᴇ91");

  approximates_to<float>("int(1+cos(a),a, 0, 180)", "180", k_degreeCtx);
  approximates_to<double>("int(1+cos(a),a, 0, 180)", "180", k_degreeCtx);

  // former precision issues
  // #1912
  approximates_to<double>("int(abs(2*sin(e^(x/4))+1),x,0,6)",
                          "12.573260585347");
  // #1912
  approximates_to<double>("int(4*√(1-x^2),x,0,1)", "3.1415926535898");
  // Runge function
  approximates_to<double>("int(1/(1+25*x^2),x,0,1)", "0.274680153389");
  // #1378
  approximates_to<double>("int(2*√(9-(x-3)^2),x,0,6)", "28.274333882309");

  // far bounds
  approximates_to<double>("int(e^(-x^2),x,0,100000)", "0.88622692545276");
  approximates_to<double>("int(1/(x^2+1),x,0,100000)", "1.5707863267949");
  approximates_to<double>("int(1/(x^2+1),x,0,inf)", "1.5707963267949");

  approximates_to<double>("int(1/x,x,1,1000000)", "13.81551056", k_realCtx, 10);
  approximates_to<double>("int(e^(-x)/√(x),x,0,inf)", "1.7724", k_realCtx, 5);
  approximates_to<double>("int(1,x,inf,0)", "undef");
  approximates_to<double>("int(e^(-x),x,inf,0)", "-1");

  // singularities, project for better precision
  projected_approximates_to<double>("int(ln(x)*√(x),x,0,1)", "-0.444444444444",
                                    k_realCtx, 12);
  projected_approximates_to<double>("int(1/√(x),x,0,1)", "2", k_realCtx, 12);
  projected_approximates_to<double>("int(1/√(1-x),x,0,1)", "2", k_realCtx, 12);
  projected_approximates_to<double>("int(1/√(x)+1/√(1-x),x,0,1)", "4",
                                    k_realCtx, 12);
  projected_approximates_to<double>("int(ln(x)^2,x,0,1)", "2", k_realCtx, 12);
  // #596
  projected_approximates_to<double>("int(1/√(x-1),x,1,2)", "2", k_realCtx, 12);
  // #1780
  projected_approximates_to<double>("int(2/√(1-x^2),x,0,1)", "3.1415926535898");
  // #1780
  projected_approximates_to<double>("int(4x/√(1-x^4),x,0,1)",
                                    "3.1415926535898");
  // convergence is slow with 1/x^k k≈1, therefore precision is poor
  projected_approximates_to<float>("int(1/x^0.9,x,0,1)", "10", k_realCtx, 3);

  // double integration
  approximates_to<float>("int(int(x×x,x,0,x),x,0,4)", "21.33333");
  approximates_to<double>("int(int(x×x,x,0,x),x,0,4)", "21.333333333333");
  approximates_to<double>("int(sum(sin(x)^k,k,0,100),x,0,π)", "48.3828",
                          k_realCtx, 6);
  approximates_to<double>("int(int(1/(1-x)^k,x,0,1),k,0.5,0.25)", "-0.405465",
                          k_realCtx, 6);
  // this integral on R2 takes about one minute to compute on N110
  // approximates_to<double>("int(int(e^(-(x^2+y^2)),x,-inf,inf),y,-inf,inf)",
  //                         "3.141592654", k_realCtx, 10);
  // approximates_to<double>("int(int(e^(-x*t)/t^2,t,1,inf),x,0,1)",
  //                         "0.3903080328", k_realCtx, 10);

  // oscillator
  // poor precision
  approximates_to<double>("int((sin(x)/x)^2,x,0,inf)", "1.5708", k_realCtx, 5);
  approximates_to<double>("int(x*sin(1/x)*√(abs(1-x)),x,0,3)", "1.9819412",
                          k_realCtx, 8);

  // Complex bounds
  approximates_to<double>("int(e^x,x,i,2i)", "undef", k_cartesianCtx);

  /* The integral approximation escapes before finding a result, because it
   * would take too much time. */
  approximates_to<float>("int(sin((10^7)*x),x,0,1)", "undef");
}

QUIZ_CASE(pcj_approximation_derivatives) {
  approximates_to<float>("diff(2×x, x, 2)", "2");
  approximates_to<float>("diff(ln(x), x, -1)", "undef");

  approximates_to_float_and_double("diff(2×x, x, 2)", "2");
  approximates_to_float_and_double("diff(2×\"TO\"^2, \"TO\", 7)", "28");
  approximates_to_float_and_double("diff(ln(x),x,1)", "1");
  approximates_to_float_and_double("diff(ln(x),x,2.2)", "0.455");
  approximates_to_float_and_double("diff(ln(x),x,0)", "undef");
  approximates_to_float_and_double("diff(ln(x),x,-3.1)", "undef");
  approximates_to_float_and_double("diff(log(x),x,-10)", "undef");
  approximates_to_float_and_double("diff(abs(x),x,123)", "1");
  approximates_to_float_and_double("diff(abs(x),x,-2.34)", "-1");
  approximates_to_float_and_double("diff(1/x,x,-2)", "-0.25");
  approximates_to_float_and_double("diff(x^3+5*x^2,x,0)", "0");
  approximates_to_float_and_double("diff(abs(x),x,0)", "0");  // "undef");
  // TODO_PCJ: error too big on floats
  // approximates_to<float>("diff(-1/3×x^3+6x^2-11x-50,x,11)", "0");
  approximates_to<double>("diff(-1/3×x^3+6x^2-11x-50,x,11)", "0");
  // On points
  approximates_to_float_and_double("diff((sin(t),cos(t)),t,π/2)", "(0,-1)");

  // Higher order
  // We have to approximate to double because error is too big on floats
  approximates_to<double>("diff(x^3,x,10,2)", "60");
  approximates_to<double>("diff(x^3,x,1,4)", "0");
  approximates_to<double>("diff(e^(2x),x,0,4)", "16");
  approximates_to_float_and_double("diff(x^3,x,3,0)", "27");
  approximates_to_float_and_double("diff(x^3,x,3,-1)", "undef");
  approximates_to_float_and_double("diff(x^3,x,3,1.3)", "undef");
  // Order 5 and above are not handled because recursively too long
  approximates_to_float_and_double("diff(e^(2x),x,0,5)", "undef");
  // On points
  approximates_to<double>("diff((2t,ln(t)),t,2,2)", "(0,-0.25)");
}
