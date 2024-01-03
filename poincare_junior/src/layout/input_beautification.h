#ifndef POINCARE_INPUT_BEAUTIFICATION_H
#define POINCARE_INPUT_BEAUTIFICATION_H

#include <poincare_junior/src/expression/aliases.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>

#include <array>

#include "k_tree.h"
#include "layout_cursor.h"

namespace PoincareJ {
using BeautifiedLayoutBuilder = Tree* (*)(EditionReference* parameters);

struct BeautificationRule {
  Aliases listOfBeautifiedAliases;
  int numberOfParameters;
  BeautifiedLayoutBuilder layoutBuilder;
};

template <BlockType type, BlockType layoutType>
consteval static BeautificationRule ruleHelper() {
  static_assert(TypeBlock::NumberOfChildren(type) ==
                TypeBlock::NumberOfChildren(layoutType));
  return BeautificationRule{
      *Builtin::GetReservedFunction(type)->aliases(),
      TypeBlock::NumberOfChildren(type),
      [](EditionReference* parameters) -> Tree* {
        EditionReference ref = SharedEditionPool->push(layoutType);
        for (int i = 0; i < TypeBlock::NumberOfChildren(layoutType); i++) {
          parameters[i]->detachTree();
        }
        return ref;
      }};
}

class InputBeautification {
 public:
  struct BeautificationMethod {
    bool beautifyIdentifiersBeforeInserting;
    bool beautifyAfterInserting;
  };

  static BeautificationMethod BeautificationMethodWhenInsertingLayout(
      const Tree* insertedLayout);

  /* Both of the following functions return true if layouts were beautified.
   *
   * BeautifyLeftOfCursorBeforeCursorMove will only apply the
   * k_simpleIdentifiersRules. This is called either:
   *    - When moving out of an horizontal layout.
   *    - Just before inserting a non-identifier char.
   *
   * BeautifyLeftOfCursorAfterInsertion will apply:
   *    - k_onInsertionRules (all combinations of chars that are beautified
   *      immediatly when inserted).
   *    - k_logarithmRule and k_identifiersRules (if a left parenthesis was
   *      just inserted).
   *    - BeautifyPipeKey, BeautifyFractionIntoDerivative and
   *      BeautifyFirstOrderDerivativeIntoNthOrder.
   * This is called only after an insertion, if the relevant char was inserted.
   */
  static bool BeautifyLeftOfCursorBeforeCursorMove(LayoutCursor* layoutCursor,
                                                   Context* context);
  static bool BeautifyLeftOfCursorAfterInsertion(LayoutCursor* layoutCursor,
                                                 Context* context);

 private:
  constexpr static int k_maxNumberOfParameters = 4;

  /* TODO : Beautification input is applied within HorizontalLayouts only.
   * This excludes beautification of single char inputs that have not yet been
   * placed within a HorizontalLayouts (such as |*_|, _ being the cursor). This
   * means that BeautificationRule on 1 char aliases isn't always ensured.
   * Currently, "*" is the only beautification affected. */
  constexpr static const BeautificationRule k_symbolsRules[] = {
  // Comparison operators
#if 0
      {"<=", 0,
       [](Layout* parameters) {
         return static_cast<Layout>(ComparisonNode::ComparisonOperatorLayout(
             ComparisonNode::OperatorType::InferiorEqual));
       }},
      {">=", 0,
       [](Layout* parameters) {
         return static_cast<Layout>(ComparisonNode::ComparisonOperatorLayout(
             ComparisonNode::OperatorType::SuperiorEqual));
       }},
      {"!=", 0,
       [](Layout* parameters) {
         return static_cast<Layout>(ComparisonNode::ComparisonOperatorLayout(
             ComparisonNode::OperatorType::NotEqual));
       }},
#endif
      // Special char
      {"->", 0,
       [](EditionReference* parameters) {
         return KCodePointL<UCodePointRightwardsArrow>()->clone();
       }},
      {"*", 0,
       [](EditionReference* parameters) {
         return KCodePointL<UCodePointMultiplicationSign>()->clone();
       }},
  };

  constexpr static BeautificationRule k_infRule = {
      "inf", 0, [](EditionReference* parameters) {
        return KCodePointL<UCodePointInfinity>()->clone();
      }};

  constexpr static BeautificationRule k_piRule = {
      "pi", 0, [](EditionReference* parameters) {
        return KCodePointL<UCodePointGreekSmallLetterPi>()->clone();
      }};
  constexpr static BeautificationRule k_thetaRule = {
      "theta", 0, [](EditionReference* parameters) {
        return KCodePointL<UCodePointGreekSmallLetterTheta>()->clone();
      }};

  constexpr static BeautificationRule k_absoluteValueRule =
      ruleHelper<BlockType::Abs, BlockType::AbsoluteValueLayout>();

  constexpr static BeautificationRule k_derivativeRule = {
      "diff", 3, [](EditionReference* parameters) -> Tree* {
        EditionReference diff =
            SharedEditionPool->push(BlockType::DerivativeLayout);
        SharedEditionPool->push(0);
        parameters[1]->detachTree();
        parameters[2]->detachTree();
        parameters[0]->detachTree();
        if (RackLayout::IsEmpty(parameters[1])) {
          NAry::AddChildAtIndex(parameters[1], "x"_cl->clone(), 0);
        }
        return diff;
      }};

  /* WARNING 1: The following arrays (k_simpleIdentifiersRules and
   * k_identifiersRules) will be beautified only if the expression can be parsed
   * without being beautified. If you add any new identifiers to one of these
   * lists, they must be parsable. This is because we must be able to
   * distinguish "asqrt" = "a*sqrt" from "asqlt" != "a*sqlt".
   *
   * WARNING 2: These need to be sorted in alphabetical order like in
   * parsing/helper.h:
   * "If the function has multiple aliases, take the first alias
   * in alphabetical order to choose position in list." */
  constexpr static const BeautificationRule k_simpleIdentifiersRules[] = {
      k_infRule, k_piRule, k_thetaRule};
  constexpr static size_t k_lenOfSimpleIdentifiersRules =
      std::size(k_simpleIdentifiersRules);

  // simpleIdentifiersRules are included in identifiersRules
  constexpr static const BeautificationRule k_identifiersRules[] = {
      /* abs( */ k_absoluteValueRule,
      /* binomial( */
      ruleHelper<BlockType::Binomial, BlockType::BinomialLayout>(),
      /* ceil( */
      ruleHelper<BlockType::Ceiling, BlockType::CeilingLayout>(),
      /* conj( */
      ruleHelper<BlockType::Conjugate, BlockType::ConjugateLayout>(),
      /* diff( */ k_derivativeRule,
      {/* exp( */
       "exp", 1,
       [](EditionReference* parameters) -> Tree* {
         EditionReference exp = KRackL("e"_cl, KVertOffL(KRackL()))->clone();
         exp->child(1)->child(0)->moveTreeOverTree(parameters[0]);
         return exp;
       }},
      /* floor( */
      ruleHelper<BlockType::Floor, BlockType::FloorLayout>(),
      /* inf */ k_infRule,
      {/* int( */
       "int", 4,
       [](EditionReference* parameters) -> Tree* {
         EditionReference integral =
             SharedEditionPool->push(BlockType::IntegralLayout);
         parameters[1]->detachTree();
         parameters[2]->detachTree();
         parameters[3]->detachTree();
         parameters[0]->detachTree();
         if (RackLayout::IsEmpty(parameters[1])) {
           NAry::AddChildAtIndex(parameters[1], "x"_cl->clone(), 0);
         }
         return integral;
       }},
      /* norm( */
      ruleHelper<BlockType::Norm, BlockType::VectorNormLayout>(),
      /* pi */ k_piRule,
#if 0
      {/* piecewise( */
       PiecewiseOperator::s_functionHelper.aliasesList(), 2,
       [](Layout* parameters) {
         /* WARNING: The implementation of ReplaceEmptyLayoutsWithParameters
          * needs the created layout to have empty layouts where the
          * parameters should be inserted. Since Piecewise operator does not
          * have a fixed number of children, the implementation is not
          * perfect. Indeed, if the layout_field is currently filled with
          * "4, x>0, 5", and "piecewise(" is inserted left of it,
          * "piecewise(4, x>0, 5)" won't be beautified, since the piecewise
          * layout does not have 3 empty children. This is a fringe case
          * though, and everything works fine when "piecewise(" is inserted
          * with nothing on its right. */
         PiecewiseOperatorLayout layout = PiecewiseOperatorLayout::Builder();
         layout.addRow(parameters[0],
                       parameters[1].isEmpty() ? Layout() : parameters[1]);
         return static_cast<Layout>(layout);
       }},
#endif
      {/* product( */
       "product", 4,
       [](EditionReference* parameters) -> Tree* {
         // TODO factorize with diff and int
         EditionReference product =
             SharedEditionPool->push(BlockType::ProductLayout);
         parameters[1]->detachTree();
         parameters[2]->detachTree();
         parameters[3]->detachTree();
         parameters[0]->detachTree();
         if (RackLayout::IsEmpty(parameters[1])) {
           NAry::AddChildAtIndex(parameters[1], "k"_cl->clone(), 0);
         }
         return product;
       }},
      /* root( */
      ruleHelper<BlockType::NthRoot, BlockType::NthRootLayout>(),
      /* sqrt( */
      ruleHelper<BlockType::SquareRoot, BlockType::SquareRootLayout>(),
      /* theta */ k_thetaRule};

  constexpr static size_t k_lenOfIdentifiersRules =
      std::size(k_identifiersRules);

  constexpr static BeautificationRule k_sumRule = {
      "sum", 4, [](EditionReference* parameters) -> Tree* {
        // TODO factorize with diff and int
        EditionReference sum = SharedEditionPool->push(BlockType::SumLayout);
        parameters[1]->detachTree();
        parameters[2]->detachTree();
        parameters[3]->detachTree();
        parameters[0]->detachTree();
        if (RackLayout::IsEmpty(parameters[1])) {
          NAry::AddChildAtIndex(parameters[1], "k"_cl->clone(), 0);
        }
        return sum;
      }};

#if 0
  constexpr static BeautificationRule k_logarithmRule = {
      Logarithm::s_functionHelper.aliasesList(), 2, [](Layout* parameters) {
        return static_cast<Layout>(
            LayoutHelper::Logarithm(parameters[0], parameters[1])
                .makeEditable());
      }};
  constexpr static int k_indexOfBaseOfLog = 1;
#endif

  static bool LayoutIsIdentifierMaterial(const Tree* l);

  // All following methods return true if layout was beautified

  /* Apply k_symbolsRules  */
  static bool BeautifySymbols(Tree* rack, int rightmostIndexToBeautify,
                              LayoutCursor* layoutCursor);

  /* Apply the rules passed in rulesList as long as they match a tokenizable
   * identifiers. */
  static bool TokenizeAndBeautifyIdentifiers(
      Tree* rack, int rightmostIndexToBeautify,
      const BeautificationRule* rulesList, size_t numberOfRules,
      Context* context, LayoutCursor* layoutCursor,
      bool logBeautification = false);

  static bool BeautifyPipeKey(Tree* rack, int indexOfPipeKey,
                              LayoutCursor* cursor);

  static bool BeautifyFractionIntoDerivative(Tree* rack, int indexOfFraction,
                                             LayoutCursor* layoutCursor);
  static bool BeautifyFirstOrderDerivativeIntoNthOrder(
      Tree* rack, int indexOfSuperscript, LayoutCursor* layoutCursor);

  static bool BeautifySum(Tree* rack, int indexOfComma, Context* context,
                          LayoutCursor* layoutCursor);

  static bool CompareAndBeautifyIdentifier(
      const Tree* firstLayout, size_t identifierLength,
      BeautificationRule beautificationRule, Tree* rack, int startIndex,
      LayoutCursor* layoutCursor, int* comparisonResult,
      int* numberOfLayoutsAddedOrRemoved);

  static bool RemoveLayoutsBetweenIndexAndReplaceWithPattern(
      Tree* rack, int startIndex, int endIndex,
      BeautificationRule beautificationRule, LayoutCursor* layoutCursor,
      int* numberOfLayoutsAddedOrRemoved = nullptr,
      Tree* preProcessedParameter = nullptr,
      int indexOfPreProcessedParameter = -1);

  // Return false if there are too many parameters
  static bool CreateParametersList(EditionReference* parameters, Tree* rack,
                                   int parenthesisIndexInParent,
                                   BeautificationRule beautificationRule,
                                   LayoutCursor* layoutCursor);
};

}  // namespace PoincareJ

#endif
