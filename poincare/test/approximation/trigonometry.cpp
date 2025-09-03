#include "helper.h"

QUIZ_CASE(pcj_approximation_trigonometry) {
  approximates_to<double>("sin(3)2(4+2)", "1.6934400967184");
  approximates_to<double>("-sin(3)×2-3", "-3.2822400161197");
  approximates_to<float>("arccot(0)", "1.570796");
  approximates_to<float>("arccot(0)", "90", degreeCtx);
  approximates_to<float>("arccot(0.5)", "1.107149");
  approximates_to<float>("arccot(-0.5)", "2.034444");
  approximates_to<float>("arcsec(0)", "undef");
  approximates_to<float>("arccsc(0)", "undef");

  /* cos: R  ->  R (oscillator)
   *      Ri ->  R (even)
   */
  // On R
  approximates_to<double>("cos(2)", "-0.41614683654714");
  approximates_to<double>("cos(2)", "0.9993908270191", degreeCtx);
  approximates_to<double>("cos(2)", "0.99950656036573", gradianCtx);
  // Oscillator
  approximates_to<float>("cos(π/2)", "0");
  approximates_to<float>("cos(100)", "0", gradianCtx);
  approximates_to<double>("cos(3×π/2)", "0");
  approximates_to<float>("cos(300)", "0", gradianCtx);
  approximates_to<float>("cos(3×π)", "-1");
  approximates_to<float>("cos(-540)", "-1", degreeCtx);
  approximates_to<float>("cos(-600)", "-1", gradianCtx);
  // On R×i
  approximates_to<double>("cos(-2×i)", "3.7621956910836", cartesianCtx);
  approximates_to<double>("cos(-2×i)", "1.0006092967033", degreeCartesianCtx);
  approximates_to<double>("cos(-2×i)", "1.0004935208085", gradianCartesianCtx);
  // Symmetry: even
  approximates_to<double>("cos(2×i)", "3.7621956910836", cartesianCtx);
  approximates_to<double>("cos(2×i)", "1.0006092967033", degreeCartesianCtx);
  approximates_to<double>("cos(2×i)", "1.0004935208085", gradianCartesianCtx);

  // On C
  approximates_to<float>("cos(i-4)", "-1.008625-0.8893952×i", cartesianCtx);
  approximates_to<float>("cos(i-4)", "0.997716+0.00121754×i",
                         degreeCartesianCtx, 6);
  approximates_to<float>("cos(i-4)", "0.99815+9.86352ᴇ-4×i",
                         gradianCartesianCtx, 6);

  // Advanced function : sec
  approximates_to<double>("sec(2)", "-2.4029979617224");
  approximates_to<float>("sec(200)", "-1", gradianCtx);
  approximates_to<float>("sec(3×i)", "0.9986307", degreeCartesianCtx);
  approximates_to<float>("sec(-3×i)", "0.9986307", degreeCartesianCtx);
  approximates_to<float>("sec(i-4)", "-0.5577604+0.4918275×i", cartesianCtx, 7);

  /* sin: R  ->  R (oscillator)
   *      Ri ->  Ri (odd)
   */
  // On R
  approximates_to<double>("sin(2)", "0.90929742682568");
  approximates_to<double>("sin(2)", "0.034899496702501", degreeCtx);
  approximates_to<double>("sin(2)", "0.031410759078128", gradianCtx);
  // Oscillator
  approximates_to<float>("sin(π/2)", "1");
  approximates_to<double>("sin(3×π/2)", "-1");
  approximates_to<float>("sin(3×π)", "0");
  approximates_to<float>("sin(-540)", "0", degreeCtx);
  approximates_to<float>("sin(-600)", "0", gradianCtx);
  approximates_to<float>("sin(300)", "-1", gradianCtx);
  approximates_to<float>("sin(100)", "1", gradianCtx);
  // On R×i
  approximates_to<double>("sin(3×i)", "10.01787492741×i", cartesianCtx);
  approximates_to<float>("sin(3×i)", "0.05238381×i", degreeCartesianCtx);
  approximates_to<double>("sin(3×i)", "0.047141332771113×i",
                          gradianCartesianCtx);
  // Symmetry: odd
  approximates_to<double>("sin(-3×i)", "-10.01787492741×i", cartesianCtx);
  approximates_to<float>("sin(-3×i)", "-0.05238381×i", degreeCartesianCtx);
  approximates_to<double>("sin(-3×i)", "-0.047141332771113×i",
                          gradianCartesianCtx);

  // On: C
  approximates_to<float>("sin(i-4)", "1.16781-0.768163×i", cartesianCtx, 6);
  approximates_to<float>("sin(i-4)", "-0.0697671+0.0174117×i",
                         degreeCartesianCtx, 6);
  approximates_to<float>("sin(i-4)", "-0.0627983+0.0156776×i",
                         gradianCartesianCtx, 6);
  approximates_to<float>("sin(1.234567890123456ᴇ-15)", "1.23457ᴇ-15",
                         cartesianCtx, 6);

  // Advanced function : csc
  approximates_to<double>("csc(2)", "1.0997501702946");
  approximates_to<float>("csc(100)", "1", gradianCtx);
  approximates_to<float>("csc(3×i)", "-19.08987×i", degreeCartesianCtx);
  approximates_to<float>("csc(-3×i)", "19.08987×i", degreeCartesianCtx);
  approximates_to<float>("csc(i-4)", "0.597696+0.393154×i", cartesianCtx, 6);

  /* tan: R  ->  R (tangent-style)
   *      Ri ->  Ri (odd)
   */
  // On R
  approximates_to<double>("tan(2)", "-2.1850398632615");
  approximates_to<double>("tan(2)", "0.034920769491748", degreeCtx);
  approximates_to<double>("tan(2)", "0.031426266043351", gradianCtx);
  // Tangent-style
  approximates_to<float>("tan(3×π)", "0");
  approximates_to<float>("tan(-540)", "0", degreeCtx);
  approximates_to<float>("tan(-600)", "0", gradianCtx);
  // On R×i
  approximates_to<double>("tan(-2×i)", "-0.96402758007582×i", cartesianCtx);
  approximates_to<float>("tan(-2×i)", "-0.03489241×i", degreeCartesianCtx);
  approximates_to<float>("tan(-3×i)", "-0.04708904×i", gradianCartesianCtx);
  // Symmetry: odd
  approximates_to<double>("tan(2×i)", "0.96402758007582×i", cartesianCtx);
  approximates_to<float>("tan(2×i)", "0.03489241×i", degreeCartesianCtx);
  approximates_to<float>("tan(3×i)", "0.04708904×i", gradianCartesianCtx);
  // On C
  approximates_to<float>("tan(i-4)", "-0.273553+1.00281×i", cartesianCtx, 6);
  approximates_to<float>("tan(i-4)", "-0.0699054+0.0175368×i",
                         degreeCartesianCtx, 6);
  approximates_to<float>("tan(i-4)", "-0.0628991+0.0157688×i",
                         gradianCartesianCtx, 6);
  // tan(π/2)
  approximates_to<double>("tan(π/2)", "undef");
  approximates_to<double>("sin(π/2)/cos(π/2)", "undef");
  approximates_to<double>("1/cot(π/2)", "undef");
  // tan(almost π/2)
  approximates_to<double>("tan(1.57079632)", "147169276.57047");
  approximates_to<double>("sin(1.57079632)/cos(1.57079632)", "147169276.57047");
  approximates_to<double>("1/cot(1.57079632)", "147169276.57047");

  // Advanced function : cot
  approximates_to<double>("cot(2)", "-0.45765755436029");
  approximates_to<float>("cot(100)", "0", gradianCtx);
  approximates_to<float>("cot(3×i)", "-19.11604×i", degreeCartesianCtx);
  approximates_to<float>("cot(-3×i)", "19.11604×i", degreeCartesianCtx);
  approximates_to<float>("cot(i-4)", "-0.253182-0.928133×i", cartesianCtx, 6);
  // cot(π/2)
  approximates_to<double>("1/tan(π/2)", "undef");
  approximates_to<double>("cos(π/2)/sin(π/2)", "0");
  approximates_to<double>("cot(π/2)", "0");
  // cot(almost π/2)
  approximates_to<double>("1/tan(1.57079632)", "6.7948964845335ᴇ-9");
  approximates_to<double>("cos(1.57079632)/sin(1.57079632)",
                          "6.7948964845335ᴇ-9");
  approximates_to<double>("cot(1.57079632)", "6.7948964845335ᴇ-9");

  /* acos: [-1,1]    -> R
   *       ]-inf,-1[ -> π+R×i (odd imaginary)
   *       ]1, inf[  -> R×i (odd imaginary)
   *       R×i       -> π/2+R×i (odd imaginary)
   */
  // On [-1, 1]
  approximates_to<double>("acos(0.5)", "1.0471975511966");
  approximates_to<double>("acos(0.03)", "1.5407918249714");
  approximates_to<double>("acos(0.5)", "60", degreeCtx);
  approximates_to<double>("acos(0.5)", "66.666666666667", gradianCtx);
  // On [1, inf[
  approximates_to<double>("acos(2)", "nonreal");
  approximates_to<double>("acos(2)", "1.3169578969248×i", cartesianCtx);
  approximates_to<double>("acos(2)", "75.456129290217×i", degreeCartesianCtx);
  approximates_to<double>("acos(2)", "83.84×i", gradianCartesianCtx, 4);
  // Symmetry: odd on imaginary
  approximates_to<double>("acos(-2)", "3.1415926535898-1.3169578969248×i",
                          cartesianCtx);
  approximates_to<double>("acos(-2)", "180-75.456129290217×i",
                          degreeCartesianCtx);
  approximates_to<double>("acos(-2)", "200-83.84×i", gradianCartesianCtx, 4);
  // On ]-inf, -1[
  approximates_to<double>("acos(-32)", "3.14159265359-4.158638853279×i",
                          cartesianCtx, 13);
  approximates_to<float>("acos(-32)", "180-238.3×i", degreeCartesianCtx, 4);
  approximates_to<float>("acos(-32)", "200-264.7×i", gradianCartesianCtx, 4);
  // On R×i
  approximates_to<float>("acos(3×i)", "1.5708-1.8184×i", cartesianCtx, 5);
  approximates_to<float>("acos(3×i)", "90-104.19×i", degreeCartesianCtx, 5);
  approximates_to<float>("acos(3×i)", "100-115.8×i", gradianCartesianCtx, 4);
  // Symmetry: odd on imaginary
  approximates_to<float>("acos(-3×i)", "1.5708+1.8184×i", cartesianCtx, 5);
  approximates_to<float>("acos(-3×i)", "90+104.19×i", degreeCartesianCtx, 5);
  approximates_to<float>("acos(-3×i)", "100+115.8×i", gradianCartesianCtx, 4);
  // On C
  approximates_to<float>("acos(i-4)", "2.8894-2.0966×i", cartesianCtx, 5);
  approximates_to<float>("acos(i-4)", "165.551-120.126×i", degreeCartesianCtx,
                         6);
  approximates_to<float>("acos(i-4)", "183.9-133.5×i", gradianCartesianCtx, 4);
  // Key values
  approximates_to<double>("acos(0)", "90", degreeCtx);
  approximates_to<float>("acos(-1)", "180", degreeCtx);
  approximates_to<double>("acos(1)", "0", degreeCtx);
  approximates_to<double>("acos(0)", "100", gradianCtx);
  approximates_to<float>("acos(-1)", "200", gradianCtx);
  approximates_to<double>("acos(1)", "0", gradianCtx);
  // Advanced function : asec
  approximates_to<double>("arcsec(-2.4029979617224)", "2");
  approximates_to<float>("arcsec(-1)", "200", gradianCtx);
  approximates_to<float>("arcsec(0.9986307857)", "2.9999×i", degreeCartesianCtx,
                         5);
  approximates_to<float>("arcsec(-0.55776+0.491828×i)", "2.28318+1×i",
                         cartesianCtx, 6);

  /* asin: [-1,1]    -> R
   *       ]-inf,-1[ -> -π/2+R×i (odd)
   *       ]1, inf[  -> π/2+R×i (odd)
   *       R×i       -> R×i (odd)
   */
  // On [-1, 1]
  approximates_to<double>("asin(0.5)", "0.5235987755983");
  approximates_to<double>("asin(0.03)", "0.030004501823477");
  approximates_to<double>("asin(0.5)", "30", degreeCtx);
  approximates_to<double>("asin(0.5)", "33.3333", gradianCtx, 6);
  // On [1, inf[
  approximates_to<double>("asin(2)", "nonreal");
  approximates_to<double>("asin(2)", "1.5707963267949-1.3169578969248×i",
                          cartesianCtx);
  approximates_to<double>("asin(2)", "90-75.456129290217×i",
                          degreeCartesianCtx);
  approximates_to<double>("asin(2)", "100-83.84×i", gradianCartesianCtx, 4);
  // Symmetry: odd
  approximates_to<double>("asin(-2)", "-1.5707963267949+1.3169578969248×i",
                          cartesianCtx);
  approximates_to<double>("asin(-2)", "-90+75.456129290217×i",
                          degreeCartesianCtx);
  approximates_to<double>("asin(-2)", "-100+83.84×i", gradianCartesianCtx, 4);
  // On ]-inf, -1[
  approximates_to<float>("asin(-32)", "-1.571+4.159×i", cartesianCtx, 4);
  approximates_to<float>("asin(-32)", "-90+238×i", degreeCartesianCtx, 3);
  approximates_to<float>("asin(-32)", "-100+265×i", gradianCartesianCtx, 3);
  // On R×i
  approximates_to<double>("asin(3×i)", "1.8184464592321×i", cartesianCtx);
  approximates_to<double>("asin(3×i)", "115.8×i", gradianCartesianCtx, 4);
  // Symmetry: odd
  approximates_to<double>("asin(-3×i)", "-1.8184464592321×i", cartesianCtx);
  approximates_to<double>("asin(-3×i)", "-115.8×i", gradianCartesianCtx, 4);
  // On C
  approximates_to<float>("asin(i-4)", "-1.3186+2.0966×i", cartesianCtx, 5);
  approximates_to<float>("asin(i-4)", "-75.551+120.13×i", degreeCartesianCtx,
                         5);
  approximates_to<float>("asin(i-4)", "-83.95+133.5×i", gradianCartesianCtx, 4);
  // Key values
  approximates_to<double>("asin(0)", "0", degreeCtx);
  approximates_to<double>("asin(0)", "0", gradianCtx);
  approximates_to<float>("asin(-1)", "-90", degreeCtx, 6);
  approximates_to<float>("asin(-1)", "-100", gradianCtx, 6);
  approximates_to<double>("asin(1)", "90", degreeCtx);
  approximates_to<double>("asin(1)", "100", gradianCtx);
  // Advanced function : acsc
  approximates_to<double>("arccsc(1.0997501702946)", "1.1415926535898");
  approximates_to<double>("arccsc(1)", "100", gradianCtx, 6);
  approximates_to<double>("arccsc(-19.08987×i)", "3×i", degreeCartesianCtx, 6);
  approximates_to<double>("arccsc(19.08987×i)", "-3×i", degreeCartesianCtx, 6);
  approximates_to<double>("arccsc(0.5+0.4×i)", "0.792676-1.13208×i",
                          cartesianCtx, 6);

  /* atan: R         ->  R (odd)
   *       [-i,i]    ->  R×i (odd)
   *       ]-inf×i,-i[ -> -π/2+R×i (odd)
   *       ]i, inf×i[  -> π/2+R×i (odd)
   */
  // On R
  approximates_to<double>("atan(2)", "1.1071487177941");
  approximates_to<double>("atan(0.01)", "0.0099996666866652");
  approximates_to<double>("atan(2)", "63.434948822922", degreeCtx);
  approximates_to<double>("atan(2)", "70.48", gradianCtx, 4);
  approximates_to<float>("atan(0.5)", "0.4636476");
  // Symmetry: odd
  approximates_to<double>("atan(-2)", "-1.1071487177941");
  approximates_to<double>("atan(-2)", "-63.434948822922", degreeCtx);
  // On [-i, i]
  approximates_to<float>("atan(0.2×i)", "0.202733×i", cartesianCtx, 6);
  approximates_to<float>("atan(i)", "undef");
  // Symmetry: odd
  approximates_to<float>("atan(-0.2×i)", "-0.202733×i", cartesianCtx, 6);
  // On [i, inf×i[
  approximates_to<double>("atan(26×i)", "1.5707963267949+0.038480520568064×i",
                          cartesianCtx);
  approximates_to<double>("atan(26×i)", "90+2.2047714220164×i",
                          degreeCartesianCtx);
  approximates_to<double>("atan(26×i)", "100+2.45×i", gradianCartesianCtx, 3);
  // Symmetry: odd
  approximates_to<double>("atan(-26×i)", "-1.5707963267949-0.038480520568064×i",
                          cartesianCtx);
  approximates_to<double>("atan(-26×i)", "-90-2.2047714220164×i",
                          degreeCartesianCtx);
  approximates_to<double>("atan(-26×i)", "-100-2.45×i", gradianCartesianCtx, 3);
  // On ]-inf×i, -i[
  approximates_to<float>("atan(-3.4×i)", "-1.570796-0.3030679×i", cartesianCtx);
  approximates_to<float>("atan(-3.4×i)", "-90-17.3645×i", degreeCartesianCtx,
                         6);
  approximates_to<float>("atan(-3.4×i)", "-100-19.29×i", gradianCartesianCtx,
                         4);
  // On C
  approximates_to<float>("atan(i-4)", "-1.338973+0.05578589×i", cartesianCtx);
  approximates_to<float>("atan(i-4)", "-76.7175+3.1963×i", degreeCartesianCtx,
                         6);
  approximates_to<float>("atan(i-4)", "-85.24+3.551×i", gradianCartesianCtx, 4);
  // Key values
  approximates_to<float>("atan(0)", "0", degreeCtx);
  approximates_to<float>("atan(0)", "0", gradianCtx);
  approximates_to<double>("atan(-i)", "undef", cartesianCtx);
  approximates_to<double>("atan(i)", "undef", cartesianCtx);
  // Advanced function : acot
  approximates_to<double>("arccot(-0.45765755436029)", "2");
  approximates_to<double>("arccot(0)", "90", degreeCtx);
  approximates_to<float>("arccot(0)", "1.570796");
  approximates_to<float>("arccot(-19.11604×i)", "180+3×i", degreeCartesianCtx,
                         6);
  // TODO_PCJ: small real part instead of 0
  // approximates_to<float>("arccot(19.11604×i)", "-3×i", degreeCartesianCtx,
  // 6);
  approximates_to<float>("arccot(-0.253182-0.928133×i)", "2.28319+1×i",
                         cartesianCtx, 6);

  /* cosh: R         -> R (even)
   *       R×i       -> R (oscillator)
   */
  // On R
  approximates_to<double>("cosh(2)", "3.7621956910836");
  approximates_to<double>("cosh(2)", "3.7621956910836", degreeCtx);
  approximates_to<double>("cosh(2)", "3.7621956910836", gradianCtx);
  // Symmetry: even
  approximates_to<double>("cosh(-2)", "3.7621956910836");
  approximates_to<double>("cosh(-2)", "3.7621956910836", degreeCtx);
  approximates_to<double>("cosh(-2)", "3.7621956910836", gradianCtx);
  // On R×i
  approximates_to<double>("cosh(43×i)", "0.55511330152063", cartesianCtx);
  // Oscillator
  approximates_to<float>("cosh(π×i/2)", "0", cartesianCtx);
  approximates_to<float>("cosh(5×π×i/2)", "0", cartesianCtx);
  approximates_to<float>("cosh(8×π×i/2)", "1", cartesianCtx);
  approximates_to<float>("cosh(9×π×i/2)", "0", cartesianCtx);
  // On C
  approximates_to<float>("cosh(i-4)", "14.7547-22.9637×i", cartesianCtx, 6);
  approximates_to<float>("cosh(i-4)", "14.7547-22.9637×i", degreeCartesianCtx,
                         6);
  approximates_to<float>("cosh(i-4)", "14.7547-22.9637×i", gradianCartesianCtx,
                         6);

  /* sinh: R         -> R (odd)
   *       R×i       -> R×i (oscillator)
   */
  // On R
  approximates_to<double>("sinh(2)", "3.626860407847");
  approximates_to<double>("sinh(2)", "3.626860407847", degreeCtx);
  approximates_to<double>("sinh(2)", "3.626860407847", gradianCtx);
  // Symmetry: odd
  approximates_to<double>("sinh(-2)", "-3.626860407847");
  // On R×i
  approximates_to<double>("sinh(43×i)", "-0.8317747426286×i", cartesianCtx);
  // Oscillator
  approximates_to<float>("sinh(π×i/2)", "i", cartesianCtx);
  approximates_to<float>("sinh(5×π×i/2)", "i", cartesianCtx);
  approximates_to<float>("sinh(7×π×i/2)", "-i", cartesianCtx);
  approximates_to<float>("sinh(8×π×i/2)", "0", cartesianCtx);
  approximates_to<float>("sinh(9×π×i/2)", "i", cartesianCtx);
  // On C
  approximates_to<float>("sinh(i-4)", "-14.7448+22.9791×i", cartesianCtx, 6);
  approximates_to<float>("sinh(i-4)", "-14.7448+22.9791×i", degreeCartesianCtx,
                         6);

  /* tanh: R         -> R (odd)
   *       R×i       -> R×i (tangent-style)
   */
  // On R
  approximates_to<double>("tanh(2)", "0.96402758007582");
  approximates_to<double>("tanh(2)", "0.96402758007582", gradianCtx);
  // Symmetry: odd
  approximates_to<double>("tanh(-2)", "-0.96402758007582", degreeCtx);
  // On R×i
  approximates_to<double>("tanh(43×i)", "-1.4983873388552×i", cartesianCtx);
  // Tangent-style
  // FIXME: this depends on the libm implementation and does not work on
  // travis/appveyor servers
  /* approximates_to<float>("tanh(π×i/2)", "undef", cartesianCtx);
   * approximates_to<float>("tanh(5×π×i/2)","undef", cartesianCtx);
   * approximates_to<float>("tanh(7×π×i/2)", "undef", cartesianCtx);
   * approximates_to<float>("tanh(8×π×i/2)", "0", cartesianCtx);
   * approximates_to<float>("tanh(9×π×i/2)", "undef", cartesianCtx); */
  // On C
  approximates_to<float>("tanh(i-4)", "-1.00028+6.10241ᴇ-4×i", cartesianCtx, 6);
  approximates_to<float>("tanh(i-4)", "-1.00028+6.10241ᴇ-4×i",
                         degreeCartesianCtx, 6);

  /* acosh: [-1,1]       -> R×i
   *        ]-inf,-1[    -> π×i+R (even on real)
   *        ]1, inf[     -> R (even on real)
   *        ]-inf×i, 0[  -> -π/2×i+R (even on real)
   *        ]0, inf*i[   -> π/2×i+R (even on real)
   */
  // On [-1,1]
  approximates_to<double>("arcosh(2)", "1.3169578969248");
  approximates_to<double>("arcosh(2)", "1.3169578969248", degreeCtx);
  approximates_to<double>("arcosh(2)", "1.3169578969248", gradianCtx);
  // On ]-inf, -1[
  approximates_to<double>("arcosh(-4)", "2.0634370688956+3.1415926535898×i",
                          cartesianCtx);
  approximates_to<float>("arcosh(-4)", "2.06344+3.14159×i", cartesianCtx, 6);
  // On ]1,inf[: Symmetry: even on real
  approximates_to<double>("arcosh(4)", "2.0634370688956");
  approximates_to<float>("arcosh(4)", "2.063437");
  // On ]-inf×i, 0[
  approximates_to<double>("arcosh(-42×i)", "4.43095849208-1.57079632679×i",
                          cartesianCtx, 12);
  approximates_to<float>("arcosh(-42×i)", "4.431-1.571×i", cartesianCtx, 4);
  // On ]0, i×inf[: Symmetry: even on real
  approximates_to<double>("arcosh(42×i)", "4.4309584920805+1.5707963267949×i",
                          cartesianCtx);
  approximates_to<float>("arcosh(42×i)", "4.431+1.571×i", cartesianCtx, 4);
  // On C
  approximates_to<float>("arcosh(i-4)", "2.0966+2.8894×i", cartesianCtx, 5);
  approximates_to<float>("arcosh(i-4)", "2.0966+2.8894×i", degreeCartesianCtx,
                         5);
  // Key values
  // approximates_to<double>("arcosh(-1)", "3.1415926535898×i", cartesianCtx);
  approximates_to<double>("arcosh(1)", "0", cartesianCtx);
  approximates_to<float>("arcosh(0)", "1.570796×i", cartesianCtx);

  /* asinh: R            -> R (odd)
   *        [-i,i]       -> R*i (odd)
   *        ]-inf×i,-i[  -> -π/2×i+R (odd)
   *        ]i, inf×i[   -> π/2×i+R (odd)
   */
  // On R
  approximates_to<double>("arsinh(2)", "1.4436354751788");
  approximates_to<double>("arsinh(2)", "1.4436354751788", degreeCtx);
  approximates_to<double>("arsinh(2)", "1.4436354751788", gradianCtx);
  // Symmetry: odd
  approximates_to<double>("arsinh(-2)", "-1.4436354751788");
  approximates_to<double>("arsinh(-2)", "-1.4436354751788", degreeCtx);
  // On [-i,i]
  approximates_to<double>("arsinh(0.2×i)", "0.20135792079033×i", cartesianCtx);
  // arsinh(0.2*i) has a too low precision in float on the web platform
  approximates_to<float>("arsinh(0.3×i)", "0.3046927×i", degreeCartesianCtx);
  // Symmetry: odd
  approximates_to<double>("arsinh(-0.2×i)", "-0.20135792079033×i",
                          cartesianCtx);
  // arsinh(-0.2*i) has a too low precision in float on the web platform
  approximates_to<float>("arsinh(-0.3×i)", "-0.3046927×i", degreeCartesianCtx);
  // On ]-inf×i, -i[
  approximates_to<double>("arsinh(-22×i)", "-3.78367270433-1.57079632679×i",
                          cartesianCtx, 12);
  approximates_to<float>("arsinh(-22×i)", "-3.784-1.571×i", degreeCartesianCtx,
                         4);
  // On ]i, inf×i[, Symmetry: odd
  approximates_to<double>("arsinh(22×i)", "3.7836727043295+1.5707963267949×i",
                          cartesianCtx);
  approximates_to<float>("arsinh(22×i)", "3.784+1.571×i", degreeCartesianCtx,
                         4);
  // On C
  approximates_to<float>("arsinh(i-4)", "-2.123+0.2383×i", cartesianCtx, 4);
  approximates_to<float>("arsinh(i-4)", "-2.123+0.2383×i", degreeCartesianCtx,
                         4);

  /* atanh: [-1,1]       -> R (odd)
   *        ]-inf,-1[    -> π/2*i+R (odd)
   *        ]1, inf[     -> -π/2×i+R (odd)
   *        R×i          -> R×i (odd)
   */
  // On [-1,1]
  approximates_to<double>("artanh(0.4)", "0.4236489301936");
  approximates_to<double>("artanh(0.4)", "0.4236489301936", degreeCtx);
  approximates_to<double>("artanh(0.4)", "0.4236489301936", gradianCtx);
  // Symmetry: odd
  approximates_to<double>("artanh(-0.4)", "-0.4236489301936");
  approximates_to<double>("artanh(-0.4)", "-0.4236489301936", degreeCtx);
  // On ]1, inf[
  approximates_to<double>("artanh(4)", "0.255412811883-1.5707963267949×i",
                          cartesianCtx);
  approximates_to<float>("artanh(4)", "0.2554128-1.570796×i", cartesianCtx);
  // On ]-inf,-1[, Symmetry: odd
  approximates_to<double>("artanh(-4)", "-0.255412811883+1.5707963267949×i",
                          cartesianCtx);
  approximates_to<float>("artanh(-4)", "-0.2554128+1.570796×i",
                         degreeCartesianCtx);
  // On R×i
  approximates_to<double>("artanh(4×i)", "1.325817663668×i", cartesianCtx);
  approximates_to<float>("artanh(4×i)", "1.325818×i", cartesianCtx);
  // Symmetry: odd
  approximates_to<double>("artanh(-4×i)", "-1.325817663668×i", cartesianCtx);
  approximates_to<float>("artanh(-4×i)", "-1.325818×i", cartesianCtx);
  // On C
  approximates_to<float>("artanh(i-4)", "-0.238878+1.50862×i", cartesianCtx, 6);
  approximates_to<float>("artanh(i-4)", "-0.238878+1.50862×i",
                         degreeCartesianCtx, 6);

  // Check that the complex part is not neglected
  approximates_to<double>("artanh(0.99999999999+1.0ᴇ-26×i)", "13+5ᴇ-16×i",
                          cartesianCtx, 3);
  approximates_to<double>("artanh(0.99999999999+1.0ᴇ-60×i)", "13+5ᴇ-50×i",
                          cartesianCtx, 3);
  approximates_to<double>("artanh(0.99999999999+1.0ᴇ-150×i)", "13+5ᴇ-140×i",
                          cartesianCtx, 3);
  approximates_to<double>("artanh(0.99999999999+1.0ᴇ-250×i)", "13+5ᴇ-240×i",
                          cartesianCtx, 3);
  approximates_to<double>("artanh(0.99999999999+1.0ᴇ-300×i)", "13+5ᴇ-290×i",
                          cartesianCtx, 3);

  // WARNING: evaluate on branch cut can be multi-valued
  approximates_to<double>("acos(2)", "1.3169578969248×i", cartesianCtx);
  approximates_to<double>("acos(2)", "75.456129290217×i", degreeCartesianCtx);
  approximates_to<double>("asin(2)", "1.5707963267949-1.3169578969248×i",
                          cartesianCtx);
  approximates_to<double>("asin(2)", "90-75.456129290217×i",
                          degreeCartesianCtx);
  approximates_to<double>("artanh(2)", "0.54930614433405-1.5707963267949×i",
                          cartesianCtx);
  approximates_to<double>("atan(2i)", "1.5707963267949+0.54930614433405×i",
                          cartesianCtx);
  approximates_to<double>("atan(2i)", "90+31.472923730945×i",
                          degreeCartesianCtx);
  approximates_to<double>("arsinh(2i)", "1.3169578969248+1.5707963267949×i",
                          cartesianCtx);
  approximates_to<double>("arcosh(-2)", "1.3169578969248+3.1415926535898×i",
                          cartesianCtx);
}
