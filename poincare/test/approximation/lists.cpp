#include "../helpers/symbol_store.h"
#include "helper.h"

QUIZ_CASE(pcj_approximation_list) {
  approximates_to<float>("{1,2,3,4,5,6}", "{1,2,3,4,5,6}");
  approximates_to<double>("{1,2,3,4,5,6}", "{1,2,3,4,5,6}");
  approximates_to<float>("{1,2,3,4}(-5,1)", "undef");
  approximates_to<float>("{1,2,3,4}(0,2)", "{1,2}");

  approximates_to<float>("sequence(k<=1, k, 2)", "{True,False}");
  approximates_to<float>("sequence(1/(k-2)<=3, k, 5)",
                         "{True,undef,True,True,True}");
  approximates_to<float>("sequence(1/(k-2)=3, k, 5)",
                         "{False,undef,False,False,False}");
  approximates_to<float>("sequence(k^2,k,4)", "{1,4,9,16}");
  approximates_to<double>("sequence(k/2,k,7)", "{0.5,1,1.5,2,2.5,3,3.5}");

  PoincareTest::SymbolStore symbolStore;
  PoincareTest::store("{1,2,3,4,5}→L", symbolStore);
  approximates_to<float>("L(1)", "1", {.m_context = symbolStore});
  approximates_to<float>("L(0)", "undef", {.m_context = symbolStore});
  approximates_to<float>("L(7)", "undef", {.m_context = symbolStore});
  approximates_to<double>("L(1)", "1", {.m_context = symbolStore});
  approximates_to<double>("L(0)", "undef", {.m_context = symbolStore});
  approximates_to<double>("L(7)", "undef", {.m_context = symbolStore});
  approximates_to<float>("L(1,3)", "{1,2,3}", {.m_context = symbolStore});
  approximates_to<float>("L(1,9)", "{1,2,3,4,5}", {.m_context = symbolStore});
  approximates_to<float>("L(-5,3)", "undef", {.m_context = symbolStore});
  approximates_to<float>("L(3,1)", "{}", {.m_context = symbolStore});
  approximates_to<float>("L(8,9)", "{}", {.m_context = symbolStore});
}

QUIZ_CASE(pcj_approximation_lists_functions) {
  // Mean
  approximates_to<double>("mean({5,8,7,4,12})", 7.2);
  approximates_to<double>("mean({5,8,7,4,12},{1,2,3,5,6})", 7.882352941176471);
  approximates_to<double>("mean({5,8,7,4,12},{2})", "undef");
  approximates_to<double>("mean({5,8,7,4,12},{-1,1,1,1,1})", "undef");
  approximates_to<double>("mean({5,8,7,4,12},{0,0,0,0,0})", "undef");
  // Median
  approximates_to<double>("med({1,6,3,5,2})", 3.);
  approximates_to<double>("med({1,6,3,4,5,2})", 3.5);
  approximates_to<double>("med({1,6,3,4,5,2},{2,3,0.1,2.8,3,1})", 5.);
  approximates_to<double>("med({1,undef,6,3,5,undef,2})", "undef");
  // Standard deviation
  approximates_to<double>("stddev({1,2,3,4,5,6})", 1.707825127659933);
  approximates_to<double>("stddev({1,2,3,4,5,6},{6,2,3,4,5,1})",
                          1.6700645635000173);
  approximates_to<double>("stddev({1})", 0.);
  approximates_to<double>("samplestddev({1,2,3,4,5,6})", 1.8708286933869704);
  approximates_to<double>("samplestddev({1,2,3,4,5,6},{6,2,3,4,5,1})",
                          1.7113069358158486);
  approximates_to<double>("samplestddev({1})", "undef");
  // Variance
  approximates_to<double>("var({1,2,3,4,5,6},{7,0.1,2,0,1,10})",
                          5.2815524368208706);
  approximates_to<double>("var({1,2,3,4,5,6})", 2.916666666666666);
  approximates_to<double>("var({1,2,3,undef,4,5,6})", "undef");
  approximates_to<double>("var({1,2,3,3,4,5,6},{-2,2,2,2,2,2,2})", "undef");
  approximates_to<double>("var({1,2,3,4,5,6},{0,0,0,0,0,0})", "undef");
  // Dimension
  approximates_to<double>("dim({1,2,3})", 3.);
  // Minimum
  approximates_to<double>("min({1,2,3})", 1.);
  approximates_to<double>("min({undef})", "undef");
  approximates_to<double>("min({1,undef,3})", "undef");
  approximates_to<double>("min({1,undef,i})", "undef");
  approximates_to<double>("min({1,7,i})", "undef");
  // Maximum
  approximates_to<double>("max({1,2,3})", 3.);
  approximates_to<double>("max({undef})", "undef");
  approximates_to<double>("max({1,undef,3})", "undef");
  approximates_to<double>("max({1,undef,i})", "undef");
  approximates_to<double>("max({1,7,i})", "undef");
  // Sum
  approximates_to<double>("sum({1,2,3})", 6.);
  // Product
  approximates_to<double>("prod({1,4,9})", 36.);

  // Sort a list of scalars or booleans
  approximates_to<float>("sort({True,False,True})", "{False,True,True}");
  approximates_to<float>("sort({1,3,4,2})", "{1,2,3,4}");
  approximates_to<float>("sort({(3,2),(1,4),(2,0),(1,1)})",
                         "{(1,1),(1,4),(2,0),(3,2)}");
  approximates_to<double>("sort({})", "{}");
  approximates_to<double>("sort({4})", "{4}");
  approximates_to<double>("sort({undef})", "{undef}");
  approximates_to<double>("sort({-1,5,2+6,-0})", "{-1,0,5,8}");
  approximates_to<double>("sort({-1,-2,-inf,inf})", "{-∞,-2,-1,∞}");
  approximates_to<double>("sort({i})", "{i}", cartesianCtx);
  approximates_to<double>("sort({-1,undef,-2,-inf,inf})",
                          "{undef,undef,undef,undef,undef}");
  approximates_to<double>("sort({-1,undef,1})", "{undef,undef,undef}");
  // Sort list of points
  approximates_to<double>("sort({(8,1),(5,0),(5,-3),(1,0),(5,9)})",
                          "{(1,0),(5,-3),(5,0),(5,9),(8,1)}");
  approximates_to<double>("sort({(inf,1),(6,1),(5,-3),(-inf,9),(-inf,1)})",
                          "{(-∞,1),(-∞,9),(5,-3),(6,1),(∞,1)}");

  // Do not sort complexes or undefined expressions
  approximates_to<double>("sort({-1,i,8,-0})", "{undef,undef,undef,undef}",
                          cartesianCtx);
  approximates_to<double>("sort({(8,1),(5,i),(5,-3)})",
                          "{(undef,undef),(undef,undef),(undef,undef)}",
                          cartesianCtx);
  approximates_to<double>("sort({(undef,1),(6,1),(5,-3)})",
                          "{(undef,undef),(undef,undef),(undef,undef)}",
                          cartesianCtx);
}

QUIZ_CASE(pcj_approximation_map_on_list) {
  // TODO: Implement more tests on lists, with every functions
  approximates_to<float>("abs({1,-1,2,-3})", "{1,1,2,3}");
  approximates_to<float>("ceil({0.3,180})", "{1,180}");
  approximates_to<float>("cos({0,π})", "{1,-1}");
  approximates_to<float>("{1,3}!", "{1,6}");
  approximates_to<float>("{1,2,3,4}!", "{1,2,6,24}");
  approximates_to<float>("floor({1/√(2),1/2,1,-1.3})", "{0,0,1,-2}");
  approximates_to<float>("floor({0.3,180})", "{0,180}");
  approximates_to<float>("frac({0.3,180})", "{0.3,0}");
  approximates_to<float>("im({1/√(2),1/2,1,-1})", "{0,0,0,0}");
  approximates_to<float>("im({1,1+i})", "{0,1}", cartesianCtx);
  approximates_to<float>("re({1,i})", "{1,0}", cartesianCtx);
  approximates_to<float>("round({2.12,3.42}, 1)", "{2.1,3.4}");
  approximates_to<float>("round(1.23456, {2,3})", "{1.23,1.235}");
  approximates_to<float>("sin({0,π})", "{0,0}");
  approximates_to<float>("{2,3.4}-{0.1,3.1}", "{1.9,0.3}");
  approximates_to<float>("tan({0,π/4})", "{0,1}");
  approximates_to<float>("abs(sum({0}×k,k,0,0))", "{0}");
}
