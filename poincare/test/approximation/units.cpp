#include "helper.h"

QUIZ_CASE(pcj_approximation_units) {
  // Make input scalar to bypass hindered unit approximation.
  approximates_to<float>("(12_m)/(_m)", "12");
  approximates_to<float>("(1_s)/(_s)", "1");
  approximates_to<float>("1_m+1_s", "undef");
  approximates_to<float>("(1_m+1_yd)/(_m)", "1.9144");
  approximates_to<float>("1_m+x", "undef");
  approximates_to<float>("(1_mm+1_km)/(_m)", "1000.001");
  approximates_to<float>("(2_month×7_dm)/(_s×_m)", "3681720");
  approximates_to<float>("(1234_g)/(_kg)", "1.234");
  approximates_to<float>("(sum(_s,x,0,1))/(_s)", "2");

  // Temperature
#if 0  // Cannot turn these temperature expressions into scalars
  approximates_to<float>("4_°C", "277.15×_K");
  // Note: this used to be undef in previous Poincare.
  approximates_to<float>("((4-2)_°C)×2", "277.15×_K");
  approximates_to<float>("((4-2)_°F)×2", "257.5945×_K");
  approximates_to<float>("8_°C/2", "277.15×_K");
  approximates_to<float>("2_K+2_K", "4×_K");
  approximates_to<float>("2_K×2_K", "4×_K^2");
  approximates_to<float>("1/_K", "1×_K^-1");
  approximates_to<float>("(2_K)^2", "4×_K^2");
#endif

  // Undefined
  approximates_to<float>("2_°C-1_°C", "undef");
  approximates_to<float>("2_°C+2_K", "undef");
  approximates_to<float>("2_K+2_°C", "undef");
  approximates_to<float>("2_K×2_°C", "undef");
  approximates_to<float>("1/_°C", "undef");
  approximates_to<float>("(1_°C)^2", "undef");
  approximates_to<float>("tan(2_m)", "undef");
  approximates_to<float>("tan(2_rad^2)", "undef");

  // Constants
  approximates_to<float>("(_mn + _mp)/(_kg)", "3.34755ᴇ-27");
  approximates_to<float>("_mn + _G", "undef");
}
