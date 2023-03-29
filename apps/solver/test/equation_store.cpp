#include <quiz.h>

#include "helpers.h"

QUIZ_CASE(equation_solver_error) {
  assert_solves_to_error("cos(x)=0", RequireApproximateSolution);
  assert_solves_to_error("x+y+z+a+b+c+d=0", TooManyVariables);
  assert_solves_to_error("x^2+y=0", NonLinearSystem);
  assert_solves_to_error("x^3+x^2+1=int(1/t,t,0,1)", EquationUndefined);
  assert_solves_to_error("x×(x^2×int(1/t,t,0,1)+1)=0", EquationUndefined);
}

QUIZ_CASE(equation_solve_linear_system) {
  assert_solves_to(
      {"x+y=0", "3x+y+z=-5", "4z-π=0", "a+b+c=0", "a=3", "c=a+2"},
      {"x=(-π-20)/8", "y=(π+20)/8", "z=π/4", "a=3", "b=-8", "c=5"});
  assert_solves_to("2x+3=4", "x=1/2");
  assert_solves_to({"x+y=0", "3x+y+z=-5", "4z-π=0"},
                   {"x=(-π-20)/8", "y=(π+20)/8", "z=π/4"});
  assert_solves_to({"x+y=0", "3x+y=-5"}, {"x=-5/2", "y=5/2"});
  assert_solves_to_infinite_solutions("0=0");
  assert_solves_to_infinite_solutions({"x+y=0"}, {"x=-t", "y=t"});
  assert_solves_to_infinite_solutions({"x-x=0"}, {"x=t"});
  assert_solves_to_infinite_solutions({"4y+(1-√(5))x=0", "x=(1+√(5))y"},
                                      {"x=√(5)t+t", "y=t"});
  assert_solves_to_infinite_solutions({"x+y+z=0"},
                                      {"x=-t1-t2", "y=t2", "z=t1"});
  assert_solves_to_infinite_solutions({"x+y+z=0", "x+2y+3z=0"},
                                      {"x=t", "y=-2t", "z=t"});
  assert_solves_to_infinite_solutions({"x+y+z=2", "2x+y-z=3", "3x+2y=5"},
                                      {"x=2t+1", "y=-3t+1", "z=t"});
  assert_solves_to_infinite_solutions({"a=b", "c=d"},
                                      {"a=t2", "b=t2", "c=t1", "d=t1"});
  assert_solves_to_infinite_solutions({"a-b+c=0", "c-d=0"},
                                      {"a=-t1+t2", "b=t2", "c=t1", "d=t1"});
  assert_solves_to_infinite_solutions({"x=x", "y=y"}, {"x=t2", "y=t1"});
  assert_solves_to_infinite_solutions({"x=t"}, {"t=t1", "x=t1"});
  assert_solves_to_infinite_solutions(
      {"t1=t2+t3", "t5=t1-t6"},
      {"t1=t4+t7", "t2=t4+t7-t8", "t3=t8", "t5=t7", "t6=t4"});
  assert_solves_to_infinite_solutions(
      {"a+2b+c+3d=0", "c-d=2"}, {"a=-4t1-2t2-2", "b=t2", "c=t1+2", "d=t1"});
  assert_solves_to_no_solution("2=0");
  assert_solves_to_no_solution("e=1");
  assert_solves_to_no_solution("i=5");
  assert_solves_to_no_solution("x-x+2=0");
}

QUIZ_CASE(equation_solve_polynomials) {
  set_complex_format(Cartesian);
  assert_solves_to("(x-3)^2=0", {"x=3", "delta=0"});
  assert_solves_to("(x-π)(x-ln(2))=0",
                   {"x=ln(2)", "x=π", "delta=ln(2)^2+π^2-2×π×ln(2)"});
  assert_solves_to("(x-√(2))(x-√(3))=0",
                   {"x=√(2)", "x=√(3)", "delta=5-2×√(6)"});
  assert_solves_to("2×x^2-4×x+2=0", {"x=1", "delta=0"});
  assert_solves_to("2×x^2-4×x+4=3",
                   {"x=(2-√(2))/2", "x=(2+√(2))/2", "delta=8"});
  assert_solves_to("3×x^2-4x+4=2",
                   {"x=2/3-(√(2)/3)i", "x=2/3+(√(2)/3)i", "delta=-8"});
  assert_solves_to("x^2+x+1=3×x^2+π×x-√(5)",
                   {"x=(-π+1-√(π^2-2π+9+8×√(5)))/4",
                    "x=(-π+1+√(π^2-2π+9+8√(5)))/4", "delta=π^2-2π+9+8√(5)"});
  assert_solves_to("x^3+x+1=0",
                   {"x=-0.6823278038", "x=0.3411639019-1.1615414×i",
                    "x=0.3411639019+1.1615414×i", "delta=-31"});
  assert_solves_to("x^3+x^2+1=0",
                   {"x=-1.465571232", "x=0.2327856159-0.7925519925×i",
                    "x=0.2327856159+0.7925519925×i", "delta=-31"});
  assert_solves_to("x^3+x^2=10^200", {"delta=-27×10^400+4×10^200"});
  assert_solves_to("x^3-3x-2=0", {"x=-1", "x=2", "delta=0"});
  assert_solves_to("x^3-4x^2+6x-24=0",
                   {"x=4", "x=-√(6)×i", "x=√(6)×i", "delta=-11616"});
}

QUIZ_CASE(equation_solve_approximate) {
  Poincare::Preferences::sharedPreferences->setAngleUnit(Degree);
  assert_solves_numerically_to("(3x)^3/(0.1-3x)^3=10^(-8)", -10, 10,
                               {0.000071660});
  assert_solves_numerically_to("(x-1)/(2×(x-2)^2)=20.8", -10, 10,
                               {1.856511, 2.167528});
  assert_solves_numerically_to("-2/(x-4)=-x^2+2x-4", -10, 10, {4.154435});
  assert_solves_numerically_to("1/cos(x)=0", 0, 10000, {});
  assert_solves_numerically_to("4.4ᴇ-9/(0.12+x)^2=1.1ᴇ-9/x^2", -10, 10,
                               {-0.04, 0.12});
  assert_solves_numerically_to("8x^4-22x^2+15=0", -10, 10,
                               {-1.224745, -1.118034, 1.118034, 1.224745});
  assert_solves_numerically_to("cos(x)=0", -100, 100, {-90.0, 90.0});
  assert_solves_numerically_to("cos(x)=0", -900, 1000,
                               {-810.0, -630.0, -450.0, -270.0, -90.0, 90.0,
                                270.0, 450.0, 630.0, 810.0});
  assert_solves_numerically_to("e^x/1000=0", -1000, 1000, {});
  assert_solves_numerically_to("e^x=0", -1000, 1000, {});
  assert_solves_numerically_to("√(y)=0", -900, 1000, {0}, "y");
  assert_solves_numerically_to("√(y+1)=0", -900, 1000, {-1}, "y");
  // The ends of the interval are solutions
  assert_solves_numerically_to(
      "sin(x)=0", 0, 10000,
      {0, 180, 360, 540, 720, 900, 1080, 1260, 1440, 1620});
  assert_solves_numerically_to("(x-1)^2×(x+1)^2=0", -1, 1, {-1, 1});
  assert_solves_numerically_to("(x-1.00001)^2×(x+1.00001)^2=0", -1, 1, {});
  assert_solves_numerically_to("sin(x)=0", -180, 180, {-180, 0, 180});

  // conj(x)*x+1 = 0
  assert_solves_to_error("conj(x)*x+1=0", RequireApproximateSolution);
  assert_solves_numerically_to("conj(x)*x+1=0", -100, 100, {});

  assert_solves_to_error("(x-10)^7=0", RequireApproximateSolution);
  assert_solves_numerically_to("(x-10)^7=0", -100, 100, {10});

  assert_solves_numerically_to("abs(x-145)=0.75", 0, 300, {144.25, 145.75});
  assert_solves_numerically_to(
      "6x^5-x^4-43x^3+42x^2+x-7=0", -10, 10,
      {-2.99103, -0.3591962, 0.6322375, 0.8400476, 2.044608});

  // Filter out fake root and keep real ones
  assert_solves_numerically_to("floor(x)-0.5=0", -10, 10, {});
  assert_solves_numerically_to("7×10^9×(3×10^8-x)/(3×10^8+x)=7×10^9-320", -10,
                               10, {6.857143});
  assert_solves_numerically_to("250e^(1.6x)=10^8", -10, 10, {8.062012});
  // Gentle slope, far from x=0
  assert_solves_numerically_to("10^(-4)×abs(x-10^4)=0", 9000, 11000, {10000});
  // Steep slope, close to x=0
  assert_solves_numerically_to("10^4×abs(x-10^(-4))=0", -10, 10, {0.0001});
  assert_solves_numerically_to("10^4×abs(x-10^(-4))+0.001=0", -10, 10, {});
  /* TODO: This does not work in real-mode because abs(x) is reduced to
   * sign(x)*x which does not always approximate to the same value.
   * set_complex_format(Real);
   * assert_solves_numerically_to("10^4×abs(x-10^(-4))=0", -10, 10, {0.0001});
   */
}

QUIZ_CASE(equation_solve_complex_real) {
  set_complex_format(Real);
  // We still want complex solutions if the input has some complex value
  assert_solves_to("x+i=0", "x=-i");
  assert_solves_to_error("x+√(-1)=0", EquationNonreal);
  assert_solves_to("x^2+x+1=0", {"delta=-3"});
  assert_solves_to_error("x^2-√(-1)=0", EquationNonreal);
  assert_solves_to_error("x+√(-1)×√(-1)=0", EquationNonreal);
  assert_solves_to("root(-8,3)*x+3=0", "x=3/2");
  // With a predefined variable that should be ignored
  set("h", "3");
  assert_solves_to("(h-1)*(h-2)=0", {"h=1", "h=2", "delta=1"});
  set("h", "1");
  assert_solves_to("h^2=-1", {"delta=-4"});  // No real solutions
  set("h", "i+1");
  assert_solves_to("h^2=-1", {"delta=-4"});  // No real solutions
  //  - We still want complex solutions if the input has some complex value
  set("h", "1");
  assert_solves_to("(h-i)^2=0", {"h=i", "delta=0"});  // Complex solutions
  set("h", "i+1");
  assert_solves_to("(h-i)^2=0", {"h=i", "delta=0"});  // Complex solutions
  unset("h");
  reset_complex_format();
}

QUIZ_CASE(equation_solve_complex_cartesian) {
  set_complex_format(Cartesian);
  assert_solves_to("x+i=0", "x=-i");
  assert_solves_to("x+√(-1)=0", "x=-i");
  assert_solves_to({"x^2+x+1=0"},
                   {"x=-1/2-((√(3))/2)i", "x=-1/2+((√(3))/2)i", "delta=-3"});
  assert_solves_to("x^2-√(-1)=0",
                   {"x=-√(2)/2-(√(2)/2)i", "x=√(2)/2+(√(2)/2)i", "delta=4i"});
  assert_solves_to("x+√(-1)×√(-1)=0", "x=1");
  assert_solves_to("root(-8,3)*x+3=0", "x=-3/4+(3√(3)/4)*i");
  reset_complex_format();
}

QUIZ_CASE(equation_solve_complex_polar) {
  set_complex_format(Polar);
  assert_solves_to("x+i=0", "x=e^(-(π/2)i)");
  assert_solves_to("x+√(-1)=0", "x=e^(-(π/2)i)");
  assert_solves_to("x^2+x+1=0",
                   {"x=e^(-(2π/3)i)", "x=e^((2π/3)i)", "delta=3e^(πi)"});
  assert_solves_to("x^2-√(-1)=0",
                   {"x=e^(-(3π/4)i)", "x=e^((π/4)i)", "delta=4e^((π/2)i)"});
  assert_solves_to("root(-8,3)*x+3=0", "x=3/2×e^((2π/3)i)");
  reset_complex_format();
}

QUIZ_CASE(equation_and_symbolic_computation) {
  /* This test case needs the user defined variable. Indeed, in the equation
   * store, m_variables is just before m_userVariables, so bad fetching in
   * m_variables might fetch into m_userVariables and create problems. */
  set("x", "0");
  assert_solves_to_infinite_solutions(
      {"b=0", "D=0", "c=0", "x+y+z+t=0"},
      {"b=0", "D=0", "c=0", "t=-t1-t2", "y=t2", "z=t1"});
  unset("x");

  // Long variable names
  assert_solves_to("2\"abcde\"+3=4", "\"abcde\"=1/2");
  assert_solves_to({"\"Big1\"+\"Big2\"=0", "3\"Big1\"+\"Big2\"=-5"},
                   {"\"Big1\"=-5/2", "\"Big2\"=5/2"});

  /* Without the user defined variable, this test has too many variables.
   * With the user defined variable, it has no solutions. */
  set("g", "0");
  assert_solves_to_no_solution({"a=a+1", "a+b+c+d+f+g+h=0"});
  unset("g");

  set("a", "x");
  // a is undef
  assert_solves_to({"a=0"}, {"a=0"});

  unset("a");

  assert_solves_to_infinite_solutions({"x+a=0"}, {"a=-t", "x=t"});

  set("a", "-3");
  assert_solves_to("x+a=0", "x=3");

  assert_solves_to("a=0", "a=0");
  /* The equation has no solution since the user defined a = -3. So a is not
   * replaced with its context value, and the solution is a = 0. */

  set("b", "-4");
  assert_solves_to_infinite_solutions({"a+b=0"}, {"a=-t", "b=t"});
  /* The equation has no solution since the user defined a = -3 and b = -4.
   * So neither a nor b are replaced with their context values. Therefore the
   * solution is an infinity of solutions. */

  assert_solves_to("a+b+c=0", "c=7");

  assert_solves_to({"a+c=0", "a=3"}, {"a=3", "c=-3"});
  /* The system has no solution since the user defined a = -3. So a is not
   * replaced with its context value, and the solution is a = 3 and c = -3. */

  set("f(x)", "x+1");

  assert_solves_to("f(x)=0", "x=-1");

  assert_solves_to("f(a)=0", "a=-1");
  /* The equation has no solution since the user defined a = -3. So a is not
   * replaced with its context value, and the solution is a = -1. */

  set("g(x)", "a+x+2");

  assert_solves_to("g(x)=0", "x=1");

  assert_solves_to("g(a)=0", "a=-1");
  /* The equation has no solution since the user defined a = -3. So a is not
   * replaced with its context value, and the equation becomes a + a + 2 = 0.
   * The solution is therefore a = -1. */

  set("d", "5");
  set("c", "d");
  set("h(x)", "c+d+3");
  assert_solves_to_infinite_solutions({"h(x)=0", "c=-3"},
                                      {"c=-3", "d=0", "x=t"});
  // c and d context values should not be used

  assert_solves_to({"c+d=5", "c-d=1"}, {"c=3", "d=2"});

  set("j", "8_g");
  assert_solves_to({"j+1=0"}, {"j=-1"});

  unset("a");
  unset("b");
  unset("c");
  unset("d");
  unset("j");
  unset("f");
  unset("g");
  unset("h");

  set("a", "0");
  assert_solves_to("a=0", "a=0");
  unset("a");

  set("b", "0");
  assert_solves_to_no_solution({"b*b=1", "a=b"});
  // If predefined variable had been ignored, there would have been this error
  // assert_solves_to_error({"b*b=1","a=b"}, NonLinearSystem);
  unset("b");

  set("x", "-1");
  assert_solves_to_error("x^5+x^2+x+1=0", RequireApproximateSolution);
  set("x", "1");
  assert_solves_to_error("x^5+x^2+x+1=0", RequireApproximateSolution);
  unset("x");
}
