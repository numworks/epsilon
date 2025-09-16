#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/list.h>

#include "../helpers/symbol_store.h"
#include "helper.h"

using namespace Poincare::Internal;

QUIZ_CASE(pcj_simplification_list_bubble_up) {
  // Bubble-up with no reduction to test the bubble-up itself
  Tree* l1 = KPow(2_e, KList(3_e, 4_e))->cloneTree();
  List::BubbleUp(l1, [](Tree*) { return false; });
  assert_trees_are_equal(l1, KList(KPow(2_e, 3_e), KPow(2_e, 4_e)));
  l1->removeTree();

  Tree* l2 =
      KMult(2_e, KAdd(3_e, KList(5_e, 6_e)), KList(7_e, 8_e))->cloneTree();
  List::BubbleUp(l2, [](Tree*) { return false; });
  assert_trees_are_equal(l2, KList(KMult(2_e, KAdd(3_e, 5_e), 7_e),
                                   KMult(2_e, KAdd(3_e, 6_e), 8_e)));
  l2->removeTree();
}

QUIZ_CASE(pcj_simplification_list) {
  Poincare::ProjectionContext replaceSymbolCtx = {
      .m_symbolic = Poincare::SymbolicComputation::ReplaceAllSymbols};
  simplifies_to("{1,2}+3", "{4,5}");
  simplifies_to("{1,2}*{3,4}", "{3,8}");
  simplifies_to("{0,1}=0", "{True,False}");
  simplifies_to("{}=1", "{}");
  simplifies_to("sequence(k, k, 2)<0", "{False,False}");
  simplifies_to("sequence(k<0, k, 2)", "{False,False}");
  simplifies_to("sequence(2*k, k, 3)+1", "{3,5,7}");
  simplifies_to("sum(sequence(2*k*t, k, 3)+1, t, 1, 3)", "{15,27,39}");
  simplifies_to("sum(sequence(1, k, 255))", "255");
  simplifies_to("sequence(1, k, 256)", "undef");
  simplifies_to("sequence(k,k,0)", "{}");
  simplifies_to("mean({1,3*x,2})", "x+1");
  simplifies_to("sum({1,3*x,2})", "3×(x+1)");
  simplifies_to("min({1,-4/7,2,-2})", "-2");
  simplifies_to("var(sequence(k,k,6))", "35/12");
  simplifies_to("sort({2+1,1,2})", "{1,2,3}");
  simplifies_to("med(π*{1,2,3},{4,1,1})", "π");
  simplifies_to("med(π*{4,2,1,3})", "(5×π)/2");
  simplifies_to("dim(1+sequence(k,k,3))", "3");
  simplifies_to("mean({1,2}, {2,1})", "4/3");
  simplifies_to("dim({{1,2}})", "undef");
  simplifies_to("{2*[[1]]}", "undef");
  simplifies_to("{(1)/2, 1}", "{1/2, 1}");
  simplifies_to("prod({1,1,1})", "1");
  simplifies_to("sort({})", "{}");
  simplifies_to("prod({})", "1");
  simplifies_to("{dim({2,4})}", "{2}");
  simplifies_to("min({})", "undef");
  simplifies_to("sequence(k,k,{1,2})", "undef");
  simplifies_to("{3,4}(2)", "4");
  simplifies_to("{3,4}(1,2)", "{3,4}");
  simplifies_to("{3,4}(0,5)", "{3,4}");
  simplifies_to("0*{3,4}", "{0,0}");
  simplifies_to("0*{3,inf}", "{0,undef}");
  simplifies_to("0*inf*{3,4}", "{undef,undef}");
  simplifies_to("0^({3,4}*2)", "{0,0}");
  simplifies_to("{1,2}/{1,0}", "{1,undef}");
  simplifies_to("med(π*{undef,nonreal,x,3})", "undef");
  simplifies_to("sort(randintnorep(1,4,4))", "sort(randintnorep(1,4,4))");
  simplifies_to("med(sequence(random(),k,10))", "med(sequence(random(),k,10))");
  simplifies_to("sort({π,3,e})", "{e,3,π}");
  simplifies_to("sort({i,2})", "{undef,undef}");
  simplifies_to("sort(sort({3, 1, 2, 4}))", "{1, 2, 3, 4}");
  simplifies_to("max({π,e})", "π");
  simplifies_to("max({π,i})", "undef");
  simplifies_to("min({π,e})", "e");
  simplifies_to("med({π,3,e})", "3");
  // Ensure that [GetElement] returning [nullptr] doesn't crash
  simplifies_to("mean(sort({x,0}))", "undef", replaceSymbolCtx);
  simplifies_to("mean(sort({x,0}))", "mean(sort({x,0}))", k_keepAllSymbolsCtx);
  simplifies_to("max(sort({x,0}))", "max(sort({x,0}))", k_keepAllSymbolsCtx);
  simplifies_to("samplestddev({1,2},sort({x,0}))",
                "samplestddev({1,2},sort({x,0}))", k_keepAllSymbolsCtx);

  simplifies_to("{3,4}(0)", "undef");
  simplifies_to("{3,4}(4)", "undef");
  simplifies_to("{3,4}(-1)", "undef");
  simplifies_to("{3,4}(1/3)", "undef");
  simplifies_to("{3,4}([[2]])", "undef");
  simplifies_to("{3,4}(true)", "undef");
  simplifies_to("{3,4}(-1,0)", "undef");
  simplifies_to("{3,4}(5,6)", "{}");
  simplifies_to("{3,4}(2,1)", "{}");
  simplifies_to("{3,4}(2,2/3)", "undef");
  simplifies_to("{3,4}(2,true)", "undef");
  simplifies_to("{3,4}(2,[[2]])", "undef");
  simplifies_to("{{0}}", "undef");
  simplifies_to("{0,{}}", "undef");
  simplifies_to("{0,{1}}", "undef");
  simplifies_to("{0}=x", "{undef}", replaceSymbolCtx);
  simplifies_to("{0,x}=1", "{False,undef}", replaceSymbolCtx);
  simplifies_to("{0,1,2,3,4,5,6,7,8,{9}}", "undef");
  simplifies_to("sequence(k*_Gs, k, 2)", "undef");

  simplifies_to("{}", "{}");
  // Lists can't contain matrix or lists
  simplifies_to("{{1,2},3}", "undef");
  simplifies_to("{[[1,2][3,4]],2,3}", "undef");
  // Inner simplifications
  simplifies_to("{1,2,3}", "{1,2,3}");
  simplifies_to("{1,8/4,27/45}", "{1,2,3/5}");
  simplifies_to("{1,2+2,3^2}", "{1,4,9}");
  simplifies_to("{-1,1/0}", "{-1,undef}");

  // Operations on lists of different lengths
  simplifies_to("{}+{1}+{2,3}", "undef");
  simplifies_to("{1,2,3,4}×{-1,-2}", "undef");
  simplifies_to("{1,4,9}^{1,1/2}", "undef");
  // Operations on lists of the same length
  simplifies_to("{}×{}", "{}");
  simplifies_to("{1,2,3}-{3,2,1}", "{-2,0,2}");
  simplifies_to("{1,2,3,4}×{2,-2,2,-2}", "{2,-4,6,-8}");
  simplifies_to("{4,8,16}^{1/2,1/3,1/4}", "{2,2,2}");
  // Operations on lists and scalars
  simplifies_to("{}^(-1)", "{}");
  simplifies_to("{1,2,3}+1", "{2,3,4}");
  simplifies_to("11/{11,33,55,77}", "{1,1/3,1/5,1/7}");
  simplifies_to("{1,4,9,16,25}^(1/2)", "{1,2,3,4,5}");

  {
    // Access to an element
    PoincareTest::SymbolStore symbolStore;
    Poincare::ProjectionContext projCtx = {
        .m_symbolic = Poincare::SymbolicComputation::ReplaceDefinedSymbols,
        .m_context = symbolStore,
    };
    store("{1,4,9}→l1", symbolStore);
    store("{}→l2", symbolStore);
    simplifies_to("l1(1)", "1", projCtx);
    simplifies_to("l1(2)", "4", projCtx);
    simplifies_to("l1(3)", "9", projCtx);
    simplifies_to("l1(0)", "undef", projCtx);
    simplifies_to("l1(5)", "undef", projCtx);
    simplifies_to("l1(-2)", "undef", projCtx);
    simplifies_to("l1(1.23)", "undef", projCtx);
    simplifies_to("l2(1)", "undef", projCtx);
    // Slice of a list
    simplifies_to("l1(1,2)", "{1,4}", projCtx);
    simplifies_to("l1(2,3)", "{4,9}", projCtx);
    simplifies_to("l1(1,3)", "{1,4,9}", projCtx);
    simplifies_to("l1(2,2)", "{4}", projCtx);
    simplifies_to("l1(0,2)", "{1,4}", projCtx);
    simplifies_to("l1(1,5)", "{1,4,9}", projCtx);
    simplifies_to("l1(3,2)", "{}", projCtx);
    simplifies_to("l2(1,2)", "{}", projCtx);
  }
  // Functions on lists
  // List length
  simplifies_to("dim({})", "0");
  simplifies_to("dim({1,2,3})", "3");
  simplifies_to("dim({{1,2,3,4,5}})", "undef");
  // Sum of elements
  simplifies_to("sum({})", "0");
  simplifies_to("sum({1,2,3})", "6");
  // Product of elements
  simplifies_to("prod({})", "1");
  simplifies_to("prod({1,4,9})", "36");

  // Sort a list of complexes
  simplifies_to("sort({})", "{}");
  simplifies_to("sort({4})", "{4}");
  simplifies_to("sort({undef})", "{undef}");
  simplifies_to("sort({i})", "{i}");
  simplifies_to("sort({-1,5,2+6,-0})", "{-1,0,5,8}");
  simplifies_to("sort({-1,-2,-inf,inf})", "{-∞,-2,-1,∞}");
  // Sort list of points
  simplifies_to("sort({(8,1),(5,0),(5,-3),(1,0),(5,9)})",
                "{(1,0),(5,-3),(5,0),(5,9),(8,1)}");
  simplifies_to("sort({(inf,1),(6,1),(5,-3),(-inf,9),(-inf,1)})",
                "{(-∞,1),(-∞,9),(5,-3),(6,1),(∞,1)}");
  // Sort fail returns undef
  simplifies_to("sort({-1,undef,-2,-inf,inf})",
                "{undef,undef,undef,undef,undef}");
  simplifies_to("sort({-1,i,8,-0})", "{undef,undef,undef,undef}");
  simplifies_to("sort({-1,undef,1})", "{undef,undef,undef}");
  simplifies_to("sort({(8,1),(5,i),(5,-3)})",
                "{(undef,undef),(undef,undef),(undef,undef)}");
  simplifies_to("sort({(undef,1),(6,1),(5,-3)})",
                "{(undef,undef),(undef,undef),(undef,undef)}");
  simplifies_to("sort(ln({7-π7,7}))", "{undef,undef}");

  // Mean of a list
  simplifies_to("mean({})", "undef");
  simplifies_to("mean({1,2,3})", "2");
  simplifies_to("mean({5,8,7,4,12})", "36/5");
  simplifies_to("mean({1,6,3,4,5,2},{2,3,1,2,3,1})", "4");
  simplifies_to("mean({1,6,3,undef,5,2},{2,3,1,2,3,1})", "undef");
  simplifies_to("mean({5,8,7,4,12},{2})", "undef");
  simplifies_to("mean({5,8,7,4,12},{0,0,0,0,0})", "undef");
  simplifies_to("mean({5,8,7,4,12},{-2,4,4,4,4})", "undef");
  // Minimum of a list
  simplifies_to("min({})", "undef");
  simplifies_to("min({1,2,3})", "1");
  simplifies_to("min({3,undef,-2})", "undef");
  // Do not simplify when a value can't be approximated
  simplifies_to("min({3,x,-2})", "min({3,x,-2})");
  simplifies_to("min({3,-inf,-2})", "-∞");
  simplifies_to("min({-7,0,i})", "undef");
  simplifies_to("min({-7,undef,i})", "undef");

  // Maximum of a list
  simplifies_to("max({})", "undef");
  simplifies_to("max({1,2,3})", "3");
  simplifies_to("max({3,undef,-2})", "undef");
  // Do not simplify when a value can't be approximated
  simplifies_to("max({3,x,-2})", "max({3,x,-2})");
  simplifies_to("max({3,inf,-2})", "∞");
  simplifies_to("max({-7,0,i})", "undef");
  simplifies_to("max({-7,undef,i})", "undef");

  // Variance of a list
  simplifies_to("var({})", "undef");
  simplifies_to("var({1})", "0");
  simplifies_to("var({1,2,3,4,5,6})", "35/12");
  simplifies_to("var({1,2,3,4,5,6},{2,3,2,1,3,1})", "43/16");
  // Standard deviation of a list
  simplifies_to("stddev({})", "undef");
  simplifies_to("stddev({1})", "0");
  simplifies_to("stddev({1,2,3,4,5,6})", "√(105)/6");
  simplifies_to("stddev({1,2,3,4,5,6},{2,3,1,1,2,4})", "(2×√(157))/13");
  // Sample standard deviation of a list
  simplifies_to("samplestddev({})", "undef");
  simplifies_to("samplestddev({1})", "undef");
  simplifies_to("samplestddev({1,2,3,4,5,6})", "√(14)/2");
  simplifies_to("samplestddev({1,2,3,4,5,6},{2,3,1,1,2,4})", "√(6123)/39");
  // Median of a list
  simplifies_to("med({})", "undef");
  simplifies_to("med({1})", "1");
  simplifies_to("med({4,2,3,1,6})", "3");
  simplifies_to("med({1,6,3,4,5,2})", "7/2");
  simplifies_to("med({1,undef,2,3})", "undef");
  simplifies_to("med({1,6,3,4,5,2},{1,2,1,1,2,2})", "4");
  simplifies_to("med({1,6,3},{1,1,undef})", "undef");
  simplifies_to("med({1,6,3},{1,1,-1})", "undef");
  simplifies_to("med({1,x,2,3})", "med({1,x,2,3})");
  simplifies_to("med({1,6,3},{1,1,x})", "med({1,6,3},{1,1,x})");
  // List sequences
  simplifies_to("sequence(1,k,1)", "{1}");
  simplifies_to("sequence(k,k,10)", "{1,2,3,4,5,6,7,8,9,10}");
  simplifies_to("sequence(1/(n-3),n,5)", "{-1/2,-1,undef,1,1/2}");
  simplifies_to("sequence(x^2,x,3)", "{1,4,9}");
  {
    // Do not confuse u{n} and L*{n}
    PoincareTest::SymbolStore symbolStore;
    store("{3}→L", symbolStore);
    simplifies_to(
        "L{2}", "{6}",
        {
            .m_symbolic = Poincare::SymbolicComputation::ReplaceDefinedSymbols,
            .m_context = symbolStore,
        });  // L*{2}
  }
}

QUIZ_CASE(pcj_simplification_functions_of_lists) {
  simplifies_to("abs({1,-1,2,-3})", "{1,1,2,3}");
  simplifies_to("acos({1/√(2),1/2,1,-1})", "{π/4,π/3,0,π}");
  simplifies_to("acos({1,0})", "{0,π/2}");
  simplifies_to("asin({1/√(2),1/2,1,-1})", "{π/4,π/6,π/2,-π/2}");
  simplifies_to("asin({1,0})", "{π/2,0}");
  simplifies_to("atan({√(3),1,1/√(3),-1})", "{π/3,π/4,π/6,-π/4}");
  simplifies_to("atan({1,0})", "{π/4,0}");
  simplifies_to("binomial(3,{2,3})", "{3,1}");
  simplifies_to("binomial({2,3},1)", "{2,3}");
  simplifies_to("binomial({0,180},{1})", "undef");
  simplifies_to("ceil({0.3,180})", "{1,180}");
  simplifies_to("arg({1,1+i})", "{0,π/4}");
  simplifies_to("conj({1,1+i})", "{1,1-i}");
  simplifies_to("cos({π/3,0,π/7,π/2})", "{1/2,1,cos(π/7),0}");
  simplifies_to("cos({0,π})", "{1,-1}");
  simplifies_to("diff({0,x},x,1)", "{0,1}");
  simplifies_to("diff(x^2,x,{0,180})", "{0,360}");
  simplifies_to("{1,3}!", "{1,6}");
  simplifies_to("{1,2,3,4}!", "{1,2,6,24}");
  simplifies_to("floor({1/√(2),1/2,1,-1.3})", "{0,0,1,-2}");
  simplifies_to("floor({0.3,180})", "{0,180}");
  simplifies_to("frac({1/√(2),1/2,1,-1.3})", "{√(2)/2,1/2,0,7/10}");
  simplifies_to("frac({0.3,180})", "{3/10,0}");
  simplifies_to("gcd({25,60},15)", "{5,15}");
  simplifies_to("arcosh({0,π})", "{π/2×i,arcosh(π)}");
  simplifies_to("arsinh({0,π})", "{0,arsinh(π)}");
  simplifies_to("artanh({0,π})", "{0,artanh(π)}");
  simplifies_to("cosh({0,π})", "{1,cosh(π)}");
  simplifies_to("sinh({0,π})", "{0,sinh(π)}");
  simplifies_to("tanh({0,π})", "{0,tanh(π)}");
  simplifies_to("im({1/√(2),1/2,1,-1})", "{0,0,0,0}");
  simplifies_to("im({1,1+i})", "{0,1}");
  simplifies_to("int({0,180},x,1,2)", "{0,180}");
  simplifies_to("int({x,x^2},x,1,2)", "{int(x,x,1,2),int(x^2,x,1,2)}");
  simplifies_to("int(1,x,{0,1},1)", "{1,0}");
  simplifies_to("int(x,x,{0,1},1)", "{int(x,x,0,1),int(x,x,1,1)}");
  simplifies_to("log({2,3})", "{log(2),log(3)}");
  simplifies_to("log({2,3},5)", "{log(2,5),log(3,5)}");
  simplifies_to("log(5,{2,3})", "{log(5,2),log(5,3)}");
  simplifies_to("log({√(2),1/2,1,3})", "{log(2)/2,-log(2),0,log(3)}");
  simplifies_to("log({1/√(2),1/2,1,-3})", "{-log(2)/2,-log(2),0,log(-3)}");
  simplifies_to("log({1/√(2),1/2,1,-3},3)",
                "{-log(2,3)/2,-log(2,3),0,log(-3,3)}");
  simplifies_to("ln({2,3})", "{ln(2),ln(3)}");
  simplifies_to("ln({√(2),1/2,1,3})", "{ln(2)/2,-ln(2),0,ln(3)}");
  simplifies_to("root({2,3},5)", "{root(2,5),root(3,5)}");
  simplifies_to("-{1/√(2),1/2,3,2,1,-3}", "{-√(2)/2,-1/2,-3,-2,-1,3}");
  simplifies_to("re({1,i})", "{1,0}");
  simplifies_to("round({2.12,3.42}, 1)", "{21/10,17/5}");
  simplifies_to("round(1.23456, {2,3})", "{123/100,247/200}");
  simplifies_to("sin({π/3,0,π/7,π/2})", "{√(3)/2,0,sin(π/7),1}");
  simplifies_to("sin({0,π})", "{0,0}");
  simplifies_to("{2,3.4}-{0.1,3.1}", "{19/10,3/10}");
  simplifies_to("tan({0,π/4})", "{0,1}");
  simplifies_to("{}%", "{}");
  simplifies_to("abs({1}%)", "{1/100}");
  simplifies_to("2+{5,10}%", "{2×(1+5/100),2×(1+10/100)}");
  simplifies_to("rem(sort({i})^4,0)", "{undef}");
}

QUIZ_CASE(pcj_simplification_mix_lists) {
  simplifies_to("{{1,2},{3,4}}", "undef");
  simplifies_to("{[[1,2][3,4]]}", "undef");
  simplifies_to("[[{1,2},{3,4}][3,4]]", "undef");
  simplifies_to("{1,2}+[[1,2][3,4]]", "undef");
  simplifies_to("{1,2}*[[1,2][3,4]]", "undef");
#if TODO_PCJ
  simplifies_to("{1_kg, 2_kg}+3_kg", "{4×_kg,5×_kg}");
#endif
}
