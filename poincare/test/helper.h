#pragma once

#include <poincare/helpers/parser.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/src/memory/tree_stack.h>
#include <poincare/symbol_context.h>
#include <poincare/symbol_store_interface.h>
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
constexpr Poincare::ProjectionContext k_realCtx = {
    .m_complexFormat = Poincare::ComplexFormat::Real};

constexpr Poincare::ProjectionContext k_cartesianCtx = {
    .m_complexFormat = Poincare::ComplexFormat::Cartesian};
constexpr Poincare::ProjectionContext k_polarCtx = {
    .m_complexFormat = Poincare::ComplexFormat::Polar};

constexpr Poincare::ProjectionContext k_degreeCtx = {
    .m_angleUnit = Poincare::AngleUnit::Degree};
constexpr Poincare::ProjectionContext k_gradianCtx = {
    .m_angleUnit = Poincare::AngleUnit::Gradian};

constexpr Poincare::ProjectionContext k_degreeCartesianCtx = {
    .m_complexFormat = Poincare::ComplexFormat::Cartesian,
    .m_angleUnit = Poincare::AngleUnit::Degree};
constexpr Poincare::ProjectionContext k_gradianCartesianCtx = {
    .m_complexFormat = Poincare::ComplexFormat::Cartesian,
    .m_angleUnit = Poincare::AngleUnit::Gradian};
constexpr Poincare::ProjectionContext k_degreePolarCtx = {
    .m_complexFormat = Poincare::ComplexFormat::Polar,
    .m_angleUnit = Poincare::AngleUnit::Degree};
constexpr Poincare::ProjectionContext k_gradianPolarCtx = {
    .m_complexFormat = Poincare::ComplexFormat::Polar,
    .m_angleUnit = Poincare::AngleUnit::Gradian};

constexpr Poincare::ProjectionContext k_keepAllSymbolsCtx = {
    .m_symbolic = Poincare::SymbolicComputation::KeepAllSymbols,
};

typedef void (*ProcessTree)(Poincare::Internal::Tree*,
                            Poincare::ProjectionContext projectionContext);
void process_tree_and_compare(
    const char* input, const char* output, ProcessTree process,
    Poincare::ProjectionContext projectionContext,
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

constexpr const char* k_almostMaxIntegerString =
    "179769313486231590772930519078902473361797697894230657273430081157732675"
    "805500963132708477322407536021120113879871393357658789768814416622492847"
    "430639474124377767893424865485276302219601246094119453082952085005768838"
    "150682342462881473913110540827237163350510684586298239947245938479716304"
    "835356329624224137214";  // (2^8)^k_maxNumberOfDigits-2
constexpr const char* k_maxIntegerString =
    "179769313486231590772930519078902473361797697894230657273430081157732675"
    "805500963132708477322407536021120113879871393357658789768814416622492847"
    "430639474124377767893424865485276302219601246094119453082952085005768838"
    "150682342462881473913110540827237163350510684586298239947245938479716304"
    "835356329624224137215";  // (2^8)^k_maxNumberOfDigits-1
constexpr const char* k_overflowedIntegerString =
    "179769313486231590772930519078902473361797697894230657273430081157732675"
    "805500963132708477322407536021120113879871393357658789768814416622492847"
    "430639474124377767893424865485276302219601246094119453082952085005768838"
    "150682342462881473913110540827237163350510684586298239947245938479716304"
    "835356329624224137216";  // (2^8)^k_maxNumberOfDigits
constexpr const char* k_bigOverflowedIntegerString =
    "279769313486231590772930519078902473361797697894230657273430081157732675"
    "805500963132708477322407536021120113879871393357658789768814416622492847"
    "430639474124377767893424865485276302219601246094119453082952085005768838"
    "150682342462881473913110540827237163350510684586298239947245938479716304"
    "835356329624224137216";  // k_overflowedIntegerString with a 2 on first
                              // digit
constexpr const char* k_maxParsedIntegerString =
    "999999999999999999999999999999";  // 10^k_maxNumberOfParsedDigitsBase10 - 1
constexpr const char* k_approximatedParsedIntegerString =
    "1000000000000000000000000000000";  // 10^k_maxNumberOfParsedDigitsBase10

// Parsing
using ParsingParameters = Poincare::ParserHelper::ParsingParameters;

Poincare::Internal::Tree* parse(const char* input,
                                const Poincare::SymbolContext& symbolContext =
                                    Poincare::EmptySymbolContext{},
                                ParsingParameters params = {});

Poincare::Internal::Tree* parse_and_reduce(const char* input,
                                           bool beautify = false);

void assert_parsed_expression_is(const char* expression,
                                 const Poincare::Internal::Tree* expected,
                                 ParsingParameters params = {});
void assert_parsed_expression_is(const char* expression,
                                 const Poincare::Internal::Tree* expected,
                                 const Poincare::SymbolContext& symbolContext,
                                 ParsingParameters params = {});
void assert_parse_to_same_expression(
    const char* expression1, const char* expression2,
    const Poincare::SymbolContext& symbolContext);

void assert_expression_serializes_and_parses_to(
    const Poincare::Internal::Tree* expression,
    const Poincare::Internal::Tree* result);

void assert_text_not_parsable(const char* input,
                              const Poincare::SymbolContext& symbolContext =
                                  Poincare::EmptySymbolContext{},
                              ParsingParameters params = {});
void assert_parse_to_integer_overflow(
    const char* input, const Poincare::SymbolContext& symbolContext =
                           Poincare::EmptySymbolContext{});

inline Poincare::Internal::Tree* parseAndPrepareForApproximation(
    const char* function, Poincare::ProjectionContext ctx = {}) {
  constexpr const char* k_symbol = "x";
  Poincare::Internal::Tree* e = parse(function);
  Poincare::Internal::Simplification::ToSystem(e, &ctx);
  Poincare::Internal::Approximation::PrepareFunctionForApproximation(
      e, k_symbol, ctx.m_complexFormat == Poincare::ComplexFormat::Real);
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
    const Poincare::SymbolContext& symbolContext =
        Poincare::EmptySymbolContext{},
    Poincare::Preferences::PrintFloatMode mode = ScientificMode,
    int numberOfSignificantDigits = 7, OMG::Base base = OMG::Base::Decimal);
void assert_expression_parses_and_serializes_to_itself(
    const char* expression, const Poincare::SymbolContext& symbolContext =
                                Poincare::EmptySymbolContext{});

void serialize_expression(const Poincare::Internal::Tree* expression,
                          std::span<char> buffer,
                          int numberOfSignificantDigits);

// Simplification
void simplify(Poincare::Internal::Tree* e,
              const Poincare::ProjectionContext& ctx, bool beautify = true);
