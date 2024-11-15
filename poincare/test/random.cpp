#include <float.h>
#include <omg/float.h>
#include <poincare/src/expression/approximation.h>

#include "helper.h"

using namespace Poincare::Internal;

template <typename T>
void simplify_and_compare_approximates(const char* input1, const char* input2,
                                       bool equal) {
  Tree* e1 = parse_and_simplify(input1);
  Tree* e2 = parse_and_simplify(input2);
  T approx1 = Approximation::RootTreeToReal<T>(e1);
  T approx2 = Approximation::RootTreeToReal<T>(e2);
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

// Compares the approximated elements of a list of size 2
template <typename T>
void simplify_and_compare_approximates_list(const char* input, bool equal) {
  Tree* e = parse_and_simplify(input);
  assert(Dimension::ListLength(e) == 2);
  Tree* eApproximated = Approximation::ToTree<T>(
      e, Approximation::Parameter(true, true, false, false));
  T approx1 = Approximation::RootTreeToReal<T>(eApproximated->child(0));
  T approx2 = Approximation::RootTreeToReal<T>(eApproximated->child(1));
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
}
