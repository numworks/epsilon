#include "helper.h"

QUIZ_CASE(pcj_approximation_complex_format) {
  // Real
  approximates_to<float>("0", "0");
  approximates_to<double>("0", "0");
  approximates_to<float>("10", "10");
  approximates_to<double>("-10", "-10");
  approximates_to<float>("100", "100");
  approximates_to<double>("0.1", "0.1");
  approximates_to<float>("0.1234567", "0.1234567");
  approximates_to<double>("0.123456789012345", "0.12345678901235");
#if 0  // TODO_PCJ: nonreal not correctly bubbled up
  projected_approximates_to<float>("1+2×i", "nonreal");
  projected_approximates_to<double>("1+i-i", "nonreal");
  projected_approximates_to<double>("3-i", "nonreal");
  projected_approximates_to<float>("3-i-3", "nonreal");
  projected_approximates_to<float>("i", "nonreal");
#endif
  projected_approximates_to<float>("1+i-1", "nonreal");
  projected_approximates_to<double>("1+i", "nonreal");
  projected_approximates_to<float>("3+i", "nonreal");
  projected_approximates_to<double>("√(-1)", "nonreal");
  projected_approximates_to<double>("√(-1)×√(-1)", "nonreal");
  projected_approximates_to<double>("ln(-2)", "nonreal");
  // Power/Root approximates to the first REAL root in Real mode
  // Power have to be simplified first in order to spot the right form c^(p/q)
  // with p, q integers
  simplified_approximates_to<double>("(-8)^(1/3)", "-2");
  // Root approximates to the first REAL root in Real mode
  approximates_to<double>("root(-8,3)", "-2");
  approximates_to<double>("8^(1/3)", "2");
  // Power have to be simplified first (cf previous comment)
  simplified_approximates_to<float>("(-8)^(2/3)", "4");
  approximates_to<float>("root(-8, 3)^2", "4");
  approximates_to<double>("root(-8,3)", "-2");

  // Cartesian
  approximates_to<float>("0", "0", k_cartesianCtx);
  approximates_to<double>("0", "0", k_cartesianCtx);
  approximates_to<float>("10", "10", k_cartesianCtx);
  approximates_to<double>("-10", "-10", k_cartesianCtx);
  approximates_to<float>("100", "100", k_cartesianCtx);
  approximates_to<double>("0.1", "0.1", k_cartesianCtx);
  approximates_to<float>("0.1234567", "0.1234567", k_cartesianCtx);
  approximates_to<double>("0.123456789012345", "0.12345678901235",
                          k_cartesianCtx);
  approximates_to<float>("1+2×i", "1+2×i", k_cartesianCtx);
  approximates_to<double>("1+i-i", "1", k_cartesianCtx);
  approximates_to<float>("1+i-1", "i", k_cartesianCtx);
  approximates_to<double>("1+i", "1+i", k_cartesianCtx);
  approximates_to<float>("3+i", "3+i", k_cartesianCtx);
  approximates_to<double>("3-i", "3-i", k_cartesianCtx);
  approximates_to<float>("3-i-3", "-i", k_cartesianCtx);
  approximates_to<float>("i", "i", k_cartesianCtx);
  approximates_to<double>("√(-1)", "i", k_cartesianCtx);
  approximates_to<double>("√(-1)×√(-1)", "-1", k_cartesianCtx);
  approximates_to<double>("ln(-2)", "0.69314718055995+3.1415926535898×i",
                          k_cartesianCtx);
  approximates_to<double>("(-8)^(1/3)", "1+1.7320508075689×i", k_cartesianCtx);
  approximates_to<float>("(-8)^(2/3)", "-2+3.4641×i", k_cartesianCtx, 6);
  approximates_to<double>("root(-8,3)", "1+1.7320508075689×i", k_cartesianCtx);

  // Polar
  approximates_to<float>("0", "0", k_polarCtx);
  approximates_to<double>("0", "0", k_polarCtx);
  approximates_to<float>("10", "10", k_polarCtx);
  approximates_to<double>("-10", "10×e^(3.1415926535898×i)", k_polarCtx);

  approximates_to<float>("100", "100", k_polarCtx);
  approximates_to<double>("0.1", "0.1", k_polarCtx);
  approximates_to<float>("0.1234567", "0.1234567", k_polarCtx);
  approximates_to<double>("0.12345678", "0.12345678", k_polarCtx);

  approximates_to<float>("1+2×i", "2.236068×e^(1.107149×i)", k_polarCtx);
  approximates_to<float>("1+i-i", "1", k_polarCtx);
  approximates_to<double>("1+i-1", "e^(1.57079632679×i)", k_polarCtx, 12);
  approximates_to<float>("1+i", "1.414214×e^(0.7853982×i)", k_polarCtx);
  approximates_to<double>("3+i", "3.16227766017×e^(0.321750554397×i)",
                          k_polarCtx, 12);
  approximates_to<float>("3-i", "3.162278×e^(-0.3217506×i)", k_polarCtx);
  approximates_to<double>("3-i-3", "e^(-1.57079632679×i)", k_polarCtx, 12);
  // 2e^(i) has a too low precision in float on the web platform
  approximates_to<float>("3e^(2*i)", "3×e^(2×i)", k_polarCtx, 4);
#if 0  // TODO_PCJ: approximates to -1*i, fix beautification?
  approximates_to<double>("2e^(-i)", "2×e^(-i)", k_polarCtx, 9);
#endif

  approximates_to<float>("i", "e^(1.570796×i)", k_polarCtx);
  approximates_to<double>("√(-1)", "e^(1.5707963267949×i)", k_polarCtx);
  approximates_to<double>("√(-1)×√(-1)", "e^(3.1415926535898×i)", k_polarCtx);
  approximates_to<double>("(-8)^(1/3)", "2×e^(1.0471975511966×i)", k_polarCtx);
  approximates_to<float>("(-8)^(2/3)", "4×e^(2.094395×i)", k_polarCtx);
  approximates_to<double>("root(-8,3)", "2×e^(1.0471975511966×i)", k_polarCtx);

  // Cartesian to Polar and vice versa
  approximates_to<double>("2+3×i", "3.60555127546×e^(0.982793723247×i)",
                          k_polarCtx, 12);
  approximates_to<double>("3.60555127546×e^(0.982793723247×i)", "2+3×i",
                          k_cartesianCtx, 12);
  approximates_to<float>("12.04159457879229548012824103×e^(1.4876550949×i)",
                         "1+12×i", k_cartesianCtx, 5);

  // Overflow
  approximates_to<float>("-2ᴇ20+2ᴇ20×i", "-2ᴇ20+2ᴇ20×i", k_cartesianCtx);
  /* TODO_PCJ: this test fails on the device because libm hypotf (which is
   * called eventually by std::abs) is not accurate enough. We might change the
   * embedded libm? */
  // approximates_to<float>("-2ᴇ20+2ᴇ20×i", "2.828427ᴇ20×e^(2.356194×i)",
  // k_polarCtx);
  approximates_to<double>("1ᴇ155-1ᴇ155×i", "1ᴇ155-1ᴇ155×i", k_cartesianCtx);
  approximates_to<double>("1ᴇ155-1ᴇ155×i", "∞×e^(-0.785398163397×i)",
                          k_polarCtx, 12);
#if 0  // TODO_PCJ: nonreal not correctly bubbled up
  approximates_to<float>("-2ᴇ100+2ᴇ100×i", "-∞+∞×i");
  approximates_to<double>("-2ᴇ360+2ᴇ360×i", "-∞+∞×i");
  approximates_to<float>("-2ᴇ100+2ᴇ10×i", "-∞+2ᴇ10×i");
  approximates_to<double>("-2ᴇ360+2×i", "-∞+2×i");
#endif
  approximates_to<float>("undef+2ᴇ100×i", "undef");
  approximates_to<double>("-2ᴇ360+undef×i", "undef");

  approximates_to<float>("re(2+i)", "2", k_cartesianCtx);
  approximates_to<double>("re(2+i)", "2", k_cartesianCtx);
  approximates_to<float>("im(2+3i)", "3", k_cartesianCtx);
  approximates_to<double>("im(2+3i)", "3", k_cartesianCtx);
  approximates_to<float>("conj(3+2×i)", "3-2×i", k_cartesianCtx);
  approximates_to<double>("conj(3+2×i)", "3-2×i", k_cartesianCtx);
}
