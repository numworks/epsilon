#pragma once

#include <assert.h>
#include <ion/storage/file_system.h>
#include <poincare/code_points.h>
#include <poincare/helpers/sequence.h>
#include <poincare/symbol_context.h>

#include "function.h"
#include "sequence_cache.h"

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Shared {

/* WARNING: after calling setType, setInitialRank, setContent,
 * setFirstInitialConditionContent or setSecondInitialConditionContent, the
 * sequence symbolContext needs to invalidate the cache because the sequences
 * evaluations might have changed. */

class SequenceContext;

class Sequence : public Function {
  friend class SequenceStore;

 public:
  constexpr static CodePoint k_sequenceSymbol =
      Poincare::CodePoints::k_sequenceSymbol;
  using Type = Poincare::SequenceHelper::Type;
  using RecursiveNotation = Poincare::SequenceHelper::RecursiveNotation;

  Sequence(Ion::Storage::Record record = Record()) : Function(record) {}

  CodePoint symbol() const override { return k_sequenceSymbol; }

  // MetaData getters
  Type type() const { return recordData()->type(); }
  int initialRank() const { return recordData()->initialRank(); }
  RecursiveNotation recursiveNotation() const {
    return recordData()->recursiveNotation();
  }
  bool displaySum() const { return recordData()->displaySum(); }

  // MetaData setters
  void setType(Type type);
  void setInitialRank(int rank);
  void setRecursiveNotation(RecursiveNotation notation);
  void setDisplaySum(bool display) { recordData()->setDisplaySum(display); }

  // Aggregated layout
  Poincare::Layout aggregatedLayout();
  Ion::Storage::Record::ErrorStatus setLayoutsForAggregated(
      Poincare::Layout l, const Poincare::SymbolContext& symbolContext);

  // Definition
  static Poincare::Layout DefinitionName(CodePoint name, Type type,
                                         RecursiveNotation notation);
  Poincare::Layout definitionName() const { return m_definition.name(this); }
  Poincare::UserExpression initialExpressionContentForTypeAndNotation() const;
  void updateMainExpressionForNotation();

  // First initial condition
  Poincare::Layout firstInitialConditionName() {
    return m_firstInitialCondition.name(this);
  }
  void firstInitialConditionText(char* buffer, size_t bufferSize) const {
    return m_firstInitialCondition.text(this, buffer, bufferSize);
  }
  Poincare::SystemExpression firstInitialConditionExpressionReduced() const {
    return m_firstInitialCondition.expressionReduced(this);
  }
  Poincare::UserExpression firstInitialConditionExpressionClone() const {
    return m_firstInitialCondition.expressionClone(this);
  }
  Poincare::Layout firstInitialConditionLayout() {
    return m_firstInitialCondition.layout(this);
  }
  Ion::Storage::Record::ErrorStatus setFirstInitialConditionContent(
      Poincare::Layout l, const Poincare::SymbolContext& symbolContext =
                              Poincare::EmptySymbolContext{}) {
    return m_firstInitialCondition.setContent(this, l, symbolContext);
  }
  // Second initial condition
  Poincare::Layout secondInitialConditionName() {
    return m_secondInitialCondition.name(this);
  }
  void secondInitialConditionText(char* buffer, size_t bufferSize) const {
    return m_secondInitialCondition.text(this, buffer, bufferSize);
  }
  Poincare::SystemExpression secondInitialConditionExpressionReduced() const {
    return m_secondInitialCondition.expressionReduced(this);
  }
  Poincare::UserExpression secondInitialConditionExpressionClone() const {
    return m_secondInitialCondition.expressionClone(this);
  }
  Poincare::Layout secondInitialConditionLayout() {
    return m_secondInitialCondition.layout(this);
  }
  Ion::Storage::Record::ErrorStatus setSecondInitialConditionContent(
      Poincare::Layout l, const Poincare::SymbolContext& symbolContext =
                              Poincare::EmptySymbolContext{}) {
    return m_secondInitialCondition.setContent(this, l, symbolContext);
  }
  void tidyDownstreamPoolFrom(
      const Poincare::PoolObject* treePoolCursor = nullptr) const override;

  // Sequence properties
  int numberOfElements() { return order() + 1; }
  Poincare::Layout nameLayout() const;
  bool isDefined() const override;
  bool isEmpty() const override;
  /* Sequence u (with initial rank i) is suitable for cobweb if:
   * - it is simply recursive
   * - u(n+1) depends only on u(n) or u(i) and not on n, on another sequence or
   *   on another rank of u
   * - u(i) has a non NAN value */
  bool isSuitableForCobweb() const;
  /* Sequence u (with initial rank i) can be handled as explicit if main
   * expression does not contains forbidden terms:
   * - explicit: any term of u
   * - simple recurrence: any term of u other than u(i)
   * - double recurrence: any term of u other than u(i+1), u(i) */
  bool canBeHandledAsExplicit() const {
    return !mainExpressionContainsForbiddenTerms(false, true, false);
  }
  /* Sequence u (with initial rank i) is not computable if main expression
   * contains another sequence or forbidden terms:
   * - explicit: any term of u
   * - simple recurrence: any term of u other than u(n) (or u(n-1) for shifted
   *   notation), u(i)
   * - double recurrence: any term of u other than u(n+1), u(n) (or u(n-1),
   *   u(n-2) for shifted notation), u(i+1), u(i) */
  bool mainExpressionIsNotComputable() const {
    return mainExpressionContainsForbiddenTerms(true, true, true);
  }
  int order() const { return static_cast<int>(type()); }
  int firstNonInitialRank() const { return initialRank() + order(); }

  // Approximation
  Poincare::Coordinate2D<float> evaluateXYAtParameter(
      float x, int subCurveIndex = 0) const override {
    return Poincare::Coordinate2D<float>(x, privateEvaluateYAtX(x));
  }
  Poincare::Coordinate2D<double> evaluateXYAtParameter(
      double x, int subCurveIndex = 0) const override {
    return Poincare::Coordinate2D<double>(x, privateEvaluateYAtX(x));
  }
  double approximateAtContextRank(int rank, bool intermediateComputation) const;
  double approximateAtRank(int rank, SequenceCache* sqctx) const;

  double sumBetweenBoundsValue(double start, double end) const;
  Poincare::SystemExpression sumBetweenBounds(double start,
                                              double end) const override;

  // m_initialRank is capped by 255
  constexpr static int k_maxInitialRank = 255;
  // 255 + 1 (to take into account a double recursive sequence) fits in 3 digits
  constexpr static int k_initialRankNumberOfDigits = 3;

 private:
  constexpr static KDFont::Size k_layoutFont = KDFont::Size::Large;
  constexpr static int k_maxNumberOfSteps = 10000;

  /* RecordDataBuffer is the layout of the data buffer of Record
   * representing a Sequence. See comment in
   * Function::RecordDataBuffer about packing. */
  class __attribute__((packed)) RecordDataBuffer
      : public Function::RecordDataBuffer {
   public:
    RecordDataBuffer(KDColor color)
        : Function::RecordDataBuffer(color),
          m_type(Type::Explicit),
          m_initialRank(0),
          m_initialConditionSizes{0, 0},
          m_recursiveNotation(RecursiveNotation::Default),
          m_displaySum(false) {}
    Type type() const { return m_type; }
    void setType(Type type) { m_type = type; }
    int initialRank() const {
      assert(m_initialRank >= 0);
      return m_initialRank;
    }
    void setInitialRank(int initialRank) { m_initialRank = initialRank; }
    size_t initialConditionSize(int conditionIndex) const {
      assert(conditionIndex >= 0 && conditionIndex < 2);
      return m_initialConditionSizes[conditionIndex];
    }
    void setInitialConditionSize(uint16_t size, int conditionIndex) {
      assert(conditionIndex >= 0 && conditionIndex < 2);
      m_initialConditionSizes[conditionIndex] = size;
    }
    RecursiveNotation recursiveNotation() const { return m_recursiveNotation; }
    void setRecursiveNotation(RecursiveNotation notation) {
      m_recursiveNotation = notation;
    }
    bool displaySum() const { return m_displaySum; }
    void setDisplaySum(bool display) { m_displaySum = display; }

   private:
    static_assert((1 << 8 * sizeof(uint16_t)) >=
                      Ion::Storage::FileSystem::k_totalSize,
                  "Potential overflows of Sequence initial condition sizes");
    Type m_type;
    uint8_t m_initialRank;
#if __EMSCRIPTEN__
    // See comment about emscripten alignment in Function::RecordDataBuffer
    static_assert(
        sizeof(emscripten_align1_short) == sizeof(uint16_t),
        "emscripten_align1_short should have the same size as uint16_t");
    emscripten_align1_short m_initialConditionSizes[2];
#else
    uint16_t m_initialConditionSizes[2];
#endif
    RecursiveNotation m_recursiveNotation;
    bool m_displaySum;
  };

  class SequenceModel : public ExpressionModel {
   public:
    using ExpressionModel::ExpressionModel;
    Poincare::Layout name(const Sequence* sequence) const;
    void tidyDownstreamPoolFrom(
        const Poincare::PoolObject* treePoolCursor) const override;
    void tidyName(const Poincare::PoolObject* treePoolCursor = nullptr) const;

   protected:
    virtual void buildName(const Sequence* sequence) const = 0;
    mutable Poincare::Layout m_name;

   private:
    void updateNewDataWithExpression(
        Ion::Storage::Record* record,
        const Poincare::UserExpression& expressionToStore,
        void* expressionAddress, size_t newExpressionSize,
        size_t previousExpressionSize) override;
    virtual void updateMetaData(const Ion::Storage::Record* record,
                                size_t newSize) {}

   private:
    void setStorageChangeFlag() const override;
  };

  class DefinitionModel : public SequenceModel {
   private:
    void* expressionAddress(const Ion::Storage::Record* record) const override;
    size_t expressionSize(const Ion::Storage::Record* record) const override;
    void buildName(const Sequence* sequence) const override;
  };

  class InitialConditionModel : public SequenceModel {
   private:
    void updateMetaData(const Ion::Storage::Record* record,
                        size_t newSize) override;
    void* expressionAddress(const Ion::Storage::Record* record) const override;
    size_t expressionSize(const Ion::Storage::Record* record) const override;
    void buildName(const Sequence* sequence) const override;
    virtual int conditionIndex() const = 0;
  };

  class FirstInitialConditionModel : public InitialConditionModel {
   private:
    int conditionIndex() const override { return 0; }
  };

  class SecondInitialConditionModel : public InitialConditionModel {
   private:
    int conditionIndex() const override { return 1; }
  };

  template <typename T>
  T privateEvaluateYAtX(T x) const;
  size_t metaDataSize() const override { return sizeof(RecordDataBuffer); }
  const ExpressionModel* model() const override { return &m_definition; }
  RecordDataBuffer* recordData() const;

  bool mainExpressionContainsForbiddenTerms(
      bool recursionIsAllowed, bool systemSymbolIsAllowed,
      bool otherSequencesAreAllowed) const;

  DefinitionModel m_definition;
  FirstInitialConditionModel m_firstInitialCondition;
  SecondInitialConditionModel m_secondInitialCondition;
};

}  // namespace Shared
