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

  compare_approximates<T>(
      e1, e2, equal, Approximation::Parameters{.isRootAndCanHaveRandom = true});
  e2->removeTree();
  e1->removeTree();
  return;
}

template <typename T>
void simplify_and_compare_approximates(const char* input1, const char* input2,
                                       bool equal) {
  Tree* e1 = parse_and_reduce(input1, true);
  Tree* e2 = parse_and_reduce(input2, true);

  compare_approximates<T>(
      e1, e2, equal, Approximation::Parameters{.isRootAndCanHaveRandom = true});
  e2->removeTree();
  e1->removeTree();
  return;
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

  e->removeTree();
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

QUIZ_CASE(pcj_random_seeds) {
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

void assert_approximate_is_float(const char* input, float lowerBound = 0.0f,
                                 float upperBound = 1.0f) {
  Tree* e = parse_and_reduce(input, true);
  float approx = Approximation::To<float>(
      e, Approximation::Parameters{.isRootAndCanHaveRandom = true},
      Approximation::Context(AngleUnit::Radian));
  e->removeTree();
  quiz_assert_print_if_failure(approx >= lowerBound && approx <= upperBound,
                               input);
}

void assert_approximate_is_int(const char* input, int lowerBound,
                               int upperBound) {
  Tree* e = parse_and_reduce(input, true);
  if (Dimension::IsList(e)) {
    assert(Dimension::ListLength(e) > 0);
    // Only check first element of list
    TreeRef eApproximated = Approximation::ToTree<float>(
        e, Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                     .projectLocalVariables = true});
    e->cloneTreeOverTree(eApproximated->child(0));
    eApproximated->removeTree();
  }
  float approx = Approximation::To<float>(
      e, Approximation::Parameters{.isRootAndCanHaveRandom = true},
      Approximation::Context(AngleUnit::Radian));
  quiz_assert_print_if_failure(
      OMG::Float::RoughlyEqual(approx, std::round(approx),
                               OMG::Float::EpsilonLax<float>()) &&
          approx >= lowerBound && approx <= upperBound,
      input);
}

QUIZ_CASE(pcj_random_range_values) {
  assert_approximate_is_float("random()");
  assert_approximate_is_float("random()-random()", -1.0f, 1.0f);
  assert_approximate_is_float("3*random()", 0.0f, 3.0f);

  assert_approximate_is_int("randint(0,1)", 0, 1);
  assert_approximate_is_int("randint(-10,0)", -10, 0);

  assert_approximate_is_int("randintnorep(-2, 2, 4)", -2, 2);
}
