#ifndef POINCARE_INPUT_BEAUTIFICATION_H
#define POINCARE_INPUT_BEAUTIFICATION_H

#include <poincare_junior/src/expression/aliases.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/memory/n_ary.h>
#include <poincare_junior/src/memory/tree_ref.h>

#include <array>

#include "k_tree.h"
#include "layout_cursor.h"

namespace PoincareJ {
using BeautifiedLayoutBuilder = Tree* (*)(TreeRef* parameters);

struct BeautificationRule {
  Aliases listOfBeautifiedAliases;
  int numberOfParameters;
  BeautifiedLayoutBuilder layoutBuilder;
};

template <Type type, Type layoutType>
consteval static BeautificationRule ruleHelper() {
  static_assert(TypeBlock::NumberOfChildren(type) ==
                TypeBlock::NumberOfChildren(layoutType));
  return BeautificationRule{
      *Builtin::GetReservedFunction(type)->aliases(),
      TypeBlock::NumberOfChildren(type), [](TreeRef* parameters) -> Tree* {
        TreeRef ref = SharedTreeStack->push(layoutType);
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
                                                   Poincare::Context* context);
  static bool BeautifyLeftOfCursorAfterInsertion(LayoutCursor* layoutCursor,
                                                 Poincare::Context* context);

 private:
  constexpr static int k_maxNumberOfParameters = 4;

  /* TODO: Beautification input is applied within HorizontalLayouts only.
   * This excludes beautification of single char inputs that have not yet been
   * placed within a HorizontalLayouts (such as |*_|, _ being the cursor). This
   * means that BeautificationRule on 1 char aliases isn't always ensured.
   * Currently, "*" is the only beautification affected. */
  constexpr static const BeautificationRule k_symbolsRules[] = {
      // Comparison operators
      {"<=", 0,
       [](TreeRef* parameters) -> Tree* {
         // TODO factorise the comparison operators once we have them in PCJ
         return KCodePointL<u'≤'>()->clone();
       }},
      {">=", 0,
       [](TreeRef* parameters) { return KCodePointL<u'≥'>()->clone(); }},
      {"!=", 0,
       [](TreeRef* parameters) {
         // ≠
         return KRackL("="_cl,
                       KCodePointL<UCodePointCombiningLongSolidusOverlay>())
             ->clone();
       }},
      // Special char
      {"->", 0,
       [](TreeRef* parameters) {
         return KCodePointL<UCodePointRightwardsArrow>()->clone();
       }},
      {"*", 0,
       [](TreeRef* parameters) {
         return KCodePointL<UCodePointMultiplicationSign>()->clone();
       }},
  };

  constexpr static BeautificationRule k_infRule = {
      "inf", 0, [](TreeRef* parameters) {
        return KCodePointL<UCodePointInfinity>()->clone();
      }};

  constexpr static BeautificationRule k_piRule = {
      "pi", 0, [](TreeRef* parameters) {
        return KCodePointL<UCodePointGreekSmallLetterPi>()->clone();
      }};
  constexpr static BeautificationRule k_thetaRule = {
      "theta", 0, [](TreeRef* parameters) {
        return KCodePointL<UCodePointGreekSmallLetterTheta>()->clone();
      }};

  constexpr static BeautificationRule k_absoluteValueRule =
      ruleHelper<Type::Abs, Type::AbsLayout>();

  constexpr static BeautificationRule k_derivativeRule = {
      "diff", 3, [](TreeRef* parameters) -> Tree* {
        TreeRef diff = SharedTreeStack->push(Type::DiffLayout);
        SharedTreeStack->push(0);
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
      ruleHelper<Type::Binomial, Type::BinomialLayout>(),
      /* ceil( */
      ruleHelper<Type::Ceil, Type::CeilLayout>(),
      /* conj( */
      ruleHelper<Type::Conj, Type::ConjLayout>(),
      /* diff( */ k_derivativeRule,
      {/* exp( */
       "exp", 1,
       [](TreeRef* parameters) -> Tree* {
         TreeRef exp = KRackL("e"_cl, KSuperscriptL(KRackL()))->clone();
         exp->child(1)->child(0)->moveTreeOverTree(parameters[0]);
         return exp;
       }},
      /* floor( */
      ruleHelper<Type::Floor, Type::FloorLayout>(),
      /* inf */ k_infRule,
      {/* int( */
       "int", 4,
       [](TreeRef* parameters) -> Tree* {
         TreeRef integral = SharedTreeStack->push(Type::IntegralLayout);
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
      ruleHelper<Type::Norm, Type::VectorNormLayout>(),
      /* pi */ k_piRule,
      {/* piecewise( */
       "piecewise", 2,
       [](TreeRef* parameters) -> Tree* {
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
         TreeRef ref = SharedTreeStack->push(Type::PiecewiseLayout);
         // TODO we need a builder to make this safe
         SharedTreeStack->push(2);
         SharedTreeStack->push(2);
         parameters[0]->detachTree();
         parameters[1]->detachTree();
         KRackL()->clone();
         KRackL()->clone();
         return ref;
       }},
      {/* product( */
       "product", 4,
       [](TreeRef* parameters) -> Tree* {
         // TODO factorize with diff and int
         TreeRef product = SharedTreeStack->push(Type::ProductLayout);
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
      ruleHelper<Type::Root, Type::RootLayout>(),
      /* sqrt( */
      ruleHelper<Type::Sqrt, Type::SqrtLayout>(),
      /* theta */ k_thetaRule};

  constexpr static size_t k_lenOfIdentifiersRules =
      std::size(k_identifiersRules);

  constexpr static BeautificationRule k_sumRule = {
      "sum", 4, [](TreeRef* parameters) -> Tree* {
        // TODO factorize with diff and int
        TreeRef sum = SharedTreeStack->push(Type::SumLayout);
        parameters[1]->detachTree();
        parameters[2]->detachTree();
        parameters[3]->detachTree();
        parameters[0]->detachTree();
        if (RackLayout::IsEmpty(parameters[1])) {
          NAry::AddChildAtIndex(parameters[1], "k"_cl->clone(), 0);
        }
        return sum;
      }};

  constexpr static BeautificationRule k_logarithmRule = {
      "log", 2, [](TreeRef* parameters) -> Tree* {
        // TODO handle NL-log cf LayoutHelper::Logarithm
        TreeRef log = "log"_l->clone();
        NAry::SetNumberOfChildren(log, 5);
        SharedTreeStack->push<Type::VerticalOffsetLayout>(true, false);
        parameters[1]->detachTree();
        // TODO would be nicer with a temporary parenthesis ?
        SharedTreeStack->push<Type::ParenthesisLayout>(false, false);
        parameters[0]->detachTree();
        return log;
      }};
  constexpr static int k_indexOfBaseOfLog = 1;

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
      Poincare::Context* context, LayoutCursor* layoutCursor,
      bool logBeautification = false);

  static bool BeautifyPipeKey(Tree* rack, int indexOfPipeKey,
                              LayoutCursor* cursor);

  static bool BeautifyFractionIntoDerivative(Tree* rack, int indexOfFraction,
                                             LayoutCursor* layoutCursor);
  static bool BeautifyFirstOrderDerivativeIntoNthOrder(
      Tree* rack, int indexOfSuperscript, LayoutCursor* layoutCursor);

  static bool BeautifySum(Tree* rack, int indexOfComma,
                          Poincare::Context* context,
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
  static bool CreateParametersList(TreeRef* parameters, Tree* rack,
                                   int parenthesisIndexInParent,
                                   BeautificationRule beautificationRule,
                                   LayoutCursor* layoutCursor);
};

}  // namespace PoincareJ

#endif
