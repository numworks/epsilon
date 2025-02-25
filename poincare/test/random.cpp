#include <float.h>
#include <omg/float.h>
#include <poincare/src/expression/approximation.h>

#include "helper.h"

using namespace Poincare::Internal;

template <typename T>
void compare_approximates(const Tree* e1, const Tree* e2, bool equal,
                          Approximation::Parameters params) {
  Approximation::Context ctx(AngleUnit::Radian);
  T approx1 = Approximation::To<T>(e1, params, ctx);
  T approx2 = Approximation::To<T>(e2, params, ctx);
  bool equalResult = OMG::Float::RoughlyEqual<T>(
      approx1, approx2, OMG::Float::EpsilonLax<T>(), true);
#if POINCARE_TREE_LOG
  if (equalResult != equal) {
    std::cout << "Random test failure: \n";
    e1->logSerialize();
    std::cout << "approximated to " << approx1 << "\n";
    e2->logSerialize();
    std::cout << "approximated to " << approx2 << "\n";
    if (equal) {
      std::cout << "Approximations should be equal. \n";
    } else {
      std::cout << "Approximations should not be equal. \n";
    }
  }
#endif
  quiz_assert(equalResult == equal);
}

template <typename T>
void project_and_compare_approximates(const char* input1, const char* input2,
                                      bool equal) {
  ProjectionContext ctx;
  Tree* e1 = parse(input1);
  Simplification::ToSystem(e1, &ctx);
  Tree* e2 = parse(input2);
  Simplification::ToSystem(e2, &ctx);

  return compare_approximates<T>(
      e1, e2, equal, Approximation::Parameters{.isRootAndCanHaveRandom = true});
}

template <typename T>
void simplify_and_compare_approximates(const char* input1, const char* input2,
                                       bool equal) {
  Tree* e1 = parse_and_reduce(input1, true);
  Tree* e2 = parse_and_reduce(input2, true);

  return compare_approximates<T>(
      e1, e2, equal, Approximation::Parameters{.isRootAndCanHaveRandom = true});
}

// Compares the approximated elements of a list of size 2
template <typename T>
void simplify_and_compare_approximates_list(const char* input, bool equal) {
  Tree* e = parse_and_reduce(input, true);
  assert(Dimension::ListLength(e) == 2);
  Tree* eApproximated = Approximation::ToTree<T>(
      e, Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                   .projectLocalVariables = true});
  T approx1 = Approximation::To<T>(eApproximated->child(0),
                                   Approximation::Parameters{});
  T approx2 = Approximation::To<T>(eApproximated->child(1),
                                   Approximation::Parameters{});
  bool equalResult = OMG::Float::RoughlyEqual<T>(
      approx1, approx2, OMG::Float::EpsilonLax<T>(), true);
#if POINCARE_TREE_LOG
  if (equalResult != equal) {
    std::cout << "Random test failure: \n";
    e->logSerialize();
    std::cout << "approximated to {" << approx1 << ", " << approx2 << "} \n";
    if (equal) {
      std::cout << "Terms should be equal. \n";
    } else {
      std::cout << "Terms should not be equal. \n";
    }
  }
#endif
  quiz_assert(equalResult == equal);
}

QUIZ_CASE(pcj_random) {
  simplify_and_compare_approximates<double>("random()", "random()", false);
  simplify_and_compare_approximates<double>("random()-random()", "0", false);
  simplify_and_compare_approximates<double>(
      "sum(random(),k,0,10)-sum(random(),k,0,10)", "0", false);

  simplify_and_compare_approximates_list<double>("sequence(random(), k, 2)",
                                                 false);
  simplify_and_compare_approximates_list<double>("random() + {0,0}", true);
  // Ensures both numerator and denominator of tan's projection have same seed.
  project_and_compare_approximates<double>("tan(randint(0,1))", "sin(1)/cos(0)",
                                           false);
}
