#pragma once

#include <poincare/context.h>
#include <poincare/helpers/parser.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/src/memory/tree_stack.h>
#include <poincare/variable_store.h>
#include <quiz.h>

#include <span>

#if POINCARE_TREE_LOG
#include <iostream>
#endif

constexpr Poincare::Preferences::PrintFloatMode DecimalMode =
    Poincare::Preferences::PrintFloatMode::Decimal;
constexpr Poincare::Preferences::PrintFloatMode ScientificMode =
    Poincare::Preferences::PrintFloatMode::Scientific;
constexpr Poincare::Preferences::PrintFloatMode EngineeringMode =
    Poincare::Preferences::PrintFloatMode::Engineering;

// Default context is realCtx
constexpr Poincare::Internal::ProjectionContext realCtx = {
    .m_complexFormat = Poincare::ComplexFormat::Real};

constexpr Poincare::Internal::ProjectionContext cartesianCtx = {
    .m_complexFormat = Poincare::ComplexFormat::Cartesian};
constexpr Poincare::Internal::ProjectionContext polarCtx = {
    .m_complexFormat = Poincare::ComplexFormat::Polar};

constexpr Poincare::Internal::ProjectionContext degreeCtx = {
    .m_angleUnit = Poincare::AngleUnit::Degree};
constexpr Poincare::Internal::ProjectionContext gradianCtx = {
    .m_angleUnit = Poincare::AngleUnit::Gradian};

constexpr Poincare::Internal::ProjectionContext degreeCartesianCtx = {
    .m_complexFormat = Poincare::ComplexFormat::Cartesian,
    .m_angleUnit = Poincare::AngleUnit::Degree};
constexpr Poincare::Internal::ProjectionContext gradianCartesianCtx = {
    .m_complexFormat = Poincare::ComplexFormat::Cartesian,
    .m_angleUnit = Poincare::AngleUnit::Gradian};

constexpr Poincare::Internal::ProjectionContext keepAllSymbolsCtx = {
    .m_symbolic = Poincare::SymbolicComputation::KeepAllSymbols,
};

typedef void (*ProcessTree)(
    Poincare::Internal::Tree*,
    Poincare::Internal::ProjectionContext projectionContext);
void process_tree_and_compare(
    const char* input, const char* output, ProcessTree process,
    Poincare::Internal::ProjectionContext projectionContext,
    int nbOfSignificantDigits =
        Poincare::PrintFloat::k_undefinedNumberOfSignificantDigits,
    bool preserveInput = true);

void quiz_tolerate_print_if_failure(bool test, const char* input,
                                    const char* expected = "",
                                    const char* observed = "");
void quiz_assert_print_if_failure(bool test, const char* information,
                                  const char* expected = nullptr,
                                  const char* observed = nullptr);

void remove_system_codepoints(char* buffer);

inline void assert_tree_equals_blocks(
    const Poincare::Internal::Tree* tree,
    std::initializer_list<Poincare::Internal::Block> blocks) {
  const Poincare::Internal::Block* block = tree->block();
  for (Poincare::Internal::Block b : blocks) {
    quiz_assert(*block == b);
    block = block->next();
  }
  quiz_assert(tree->treeSize() == blocks.size());
}

inline void assert_trees_are_equal(const Poincare::Internal::Tree* tree0,
                                   const Poincare::Internal::Tree* tree1) {
  quiz_assert((tree0 == nullptr) == (tree1 == nullptr));
  if (!tree0->treeIsIdenticalTo(tree1)) {
#if POINCARE_TREE_LOG
    tree0->logDiffWith(tree1);
#endif
    quiz_assert(false);
  }
}

inline void flush_stack() { Poincare::Internal::SharedTreeStack->flush(); }

inline void assert_tree_stack_contains(
    std::initializer_list<const Poincare::Internal::Tree*> trees) {
  quiz_assert(Poincare::Internal::SharedTreeStack->size() > 0);
  Poincare::Internal::Tree* tree = Poincare::Internal::Tree::FromBlocks(
      Poincare::Internal::SharedTreeStack->firstBlock());
  for (const Poincare::Internal::Tree* t : trees) {
    assert_trees_are_equal(t, tree);
    tree = tree->nextTree();
  }
  quiz_assert(tree->block() ==
              Poincare::Internal::SharedTreeStack->lastBlock());
}

#if PLATFORM_DEVICE
#define QUIZ_ASSERT(test) quiz_assert(test)
#else
#include <chrono>
#include <iomanip>
#include <iostream>
#define QUIZ_ASSERT(test)                                                     \
  if (!(test)) {                                                              \
    std::cerr << __FILE__ << ':' << __LINE__ << ": test failed" << std::endl; \
    abort();                                                                  \
  }

inline void assertionsWarn() {
#if ASSERTIONS
  std::cout << "Compile with DEBUG=0 and ASSERTIONS=0 for more precise results"
            << std::endl;
#endif
}

#if POINCARE_METRICS
#define METRICS(F)                                                        \
  {                                                                       \
    Poincare::Internal::Tree::nextNodeCount = 0;                          \
    Poincare::Internal::Tree::nextNodeInTreeStackCount = 0;               \
    int refId;                                                            \
    {                                                                     \
      Poincare::Internal::TreeRef r =                                     \
          Poincare::Internal::SharedTreeStack->pushZero();                \
      refId = r.identifier();                                             \
      r->removeNode();                                                    \
    }                                                                     \
    auto startTime = std::chrono::high_resolution_clock::now();           \
    F;                                                                    \
    auto elapsed = std::chrono::high_resolution_clock::now() - startTime; \
    {                                                                     \
      Poincare::Internal::TreeRef r =                                     \
          Poincare::Internal::SharedTreeStack->pushZero();                \
      refId = r.identifier() - refId;                                     \
      r->removeNode();                                                    \
    }                                                                     \
    if (refId != 0) {                                                     \
      std::cout << "WARNING ! " << refId << " references have leaked.\n"; \
    }                                                                     \
    std::cout << "Metrics [" << #F << "]\n"                               \
              << "  nextNode:      " << std::right << std::setw(6)        \
              << Poincare::Internal::Tree::nextNodeCount                  \
              << "\n  nextNodeInTreeStack:" << std::right << std::setw(6) \
              << Poincare::Internal::Tree::nextNodeInTreeStackCount       \
              << "\n  nextNodeOutOfStack:" << std::right << std::setw(6)  \
              << Poincare::Internal::Tree::nextNodeCount -                \
                     Poincare::Internal::Tree::nextNodeInTreeStackCount   \
              << "\n  microseconds:  " << std::right << std::setw(6)      \
              << std::chrono::duration_cast<std::chrono::microseconds>(   \
                     elapsed)                                             \
                     .count()                                             \
              << std::endl;                                               \
    assertionsWarn();                                                     \
  }
#endif
#endif

// Integer

const char* MaxIntegerString();            // (2^8)^k_maxNumberOfDigits-1
const char* AlmostMaxIntegerString();      // (2^8)^k_maxNumberOfDigits-2
const char* OverflowedIntegerString();     // (2^8)^k_maxNumberOfDigits
const char* BigOverflowedIntegerString();  // OverflowedIntegerString with a 2
                                           // on first digit
const char* ApproximatedParsedIntegerString();

// Parsing
using ParsingParameters = Poincare::ParserHelper::ParsingParameters;

Poincare::Internal::Tree* parse(
    const char* input,
    const Poincare::Context& context = Poincare::EmptyContext{},
    ParsingParameters params = {});

Poincare::Internal::Tree* parse_and_reduce(const char* input,
                                           bool beautify = false);

void assert_parsed_expression_is(const char* expression,
                                 const Poincare::Internal::Tree* expected,
                                 ParsingParameters params = {});
void assert_parsed_expression_is(const char* expression,
                                 const Poincare::Internal::Tree* expected,
                                 const Poincare::Context& context,
                                 ParsingParameters params = {});
void assert_parse_to_same_expression(const char* expression1,
                                     const char* expression2,
                                     const Poincare::Context& context);

void assert_expression_serializes_and_parses_to(
    const Poincare::Internal::Tree* expression,
    const Poincare::Internal::Tree* result);

void assert_text_not_parsable(
    const char* input,
    const Poincare::Context& context = Poincare::EmptyContext{},
    ParsingParameters params = {});
void assert_parse_to_integer_overflow(
    const char* input,
    const Poincare::Context& context = Poincare::EmptyContext{});

inline Poincare::Internal::Tree* parseAndPrepareForApproximation(
    const char* function, Poincare::Internal::ProjectionContext ctx = {}) {
  constexpr const char* k_symbol = "x";
  Poincare::Internal::Tree* e = parse(function);
  Poincare::Internal::Simplification::ToSystem(e, &ctx);
  Poincare::Internal::Approximation::PrepareFunctionForApproximation(
      e, k_symbol, ctx.m_complexFormat);
  return e;
}

// Serialization

void assert_expression_serializes_to(
    const Poincare::Internal::Tree* expression, const char* serialization,
    Poincare::Preferences::PrintFloatMode mode =
        Poincare::Preferences::PrintFloatMode::Scientific,
    int numberOfSignificantDigits = 7, OMG::Base base = OMG::Base::Decimal);

void assert_layout_serializes_to(const Poincare::Internal::Tree* layout,
                                 const char* serialization);

void assert_expression_parses_and_serializes_to(
    const char* expression, const char* result,
    const Poincare::Context& context = Poincare::EmptyContext{},
    Poincare::Preferences::PrintFloatMode mode = ScientificMode,
    int numberOfSignificantDigits = 7, OMG::Base base = OMG::Base::Decimal);
void assert_expression_parses_and_serializes_to_itself(
    const char* expression,
    const Poincare::Context& context = Poincare::EmptyContext{});

void serialize_expression(const Poincare::Internal::Tree* expression,
                          std::span<char> buffer,
                          int numberOfSignificantDigits);

// Simplification
void simplify(Poincare::Internal::Tree* e,
              const Poincare::Internal::ProjectionContext& ctx,
              bool beautify = true);
