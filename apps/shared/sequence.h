#ifndef APPS_SHARED_SEQUENCE_H
#define APPS_SHARED_SEQUENCE_H

#include <assert.h>

#include "function.h"

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Shared {

/* WARNING: after calling setType, setInitialRank, setContent,
 * setFirstInitialConditionContent or setSecondInitialConditionContent, the
 * sequence context needs to invalidate the cache because the sequences
 * evaluations might have changed. */

class SequenceContext;

class Sequence : public Function {
  friend class SequenceStore;

 public:
  enum class Type : uint8_t {
    Explicit = 0,
    SingleRecurrence = 1,
    DoubleRecurrence = 2
  };
  Sequence(Ion::Storage::Record record = Record()) : Function(record) {}
  I18n::Message parameterMessageName() const override;
  CodePoint symbol() const override { return 'n'; }
  int nameWithArgumentAndType(char *buffer, size_t bufferSize);

  // MetaData getters
  Type type() const { return recordData()->type(); }
  int initialRank() const { return recordData()->initialRank(); }
  bool displaySum() const { return recordData()->displaySum(); }

  // MetaData setters
  void setType(Type type);
  void setInitialRank(int rank);
  void setDisplaySum(bool display) { recordData()->setDisplaySum(display); }

  // Definition
  Poincare::Layout definitionName() { return m_definition.name(this); }
  // First initial condition
  Poincare::Layout firstInitialConditionName() {
    return m_firstInitialCondition.name(this);
  }
  void firstInitialConditionText(char *buffer, size_t bufferSize) const {
    return m_firstInitialCondition.text(this, buffer, bufferSize);
  }
  Poincare::Expression firstInitialConditionExpressionReduced(
      Poincare::Context *context) const {
    return m_firstInitialCondition.expressionReduced(this, context);
  }
  Poincare::Expression firstInitialConditionExpressionClone() const {
    return m_firstInitialCondition.expressionClone(this);
  }
  Poincare::Layout firstInitialConditionLayout() {
    return m_firstInitialCondition.layout(this);
  }
  Ion::Storage::Record::ErrorStatus setFirstInitialConditionContent(
      const char *c, Poincare::Context *context) {
    return m_firstInitialCondition.setContent(this, c, context);
  }
  // Second initial condition
  Poincare::Layout secondInitialConditionName() {
    return m_secondInitialCondition.name(this);
  }
  void secondInitialConditionText(char *buffer, size_t bufferSize) const {
    return m_secondInitialCondition.text(this, buffer, bufferSize);
  }
  Poincare::Expression secondInitialConditionExpressionReduced(
      Poincare::Context *context) const {
    return m_secondInitialCondition.expressionReduced(this, context);
  }
  Poincare::Expression secondInitialConditionExpressionClone() const {
    return m_secondInitialCondition.expressionClone(this);
  }
  Poincare::Layout secondInitialConditionLayout() {
    return m_secondInitialCondition.layout(this);
  }
  Ion::Storage::Record::ErrorStatus setSecondInitialConditionContent(
      const char *c, Poincare::Context *context) {
    return m_secondInitialCondition.setContent(this, c, context);
  }
  void tidyDownstreamPoolFrom(char *treePoolCursor = nullptr) const override;

  // Sequence properties
  int numberOfElements() { return order() + 1; }
  Poincare::Layout nameLayout();
  bool isDefined() override;
  bool isEmpty() override;
  /* Sequence u is suitable for cobweb if it is simply recursive and if u(n+1)
   * depends only on u(n) or u(0) and not on n, on another sequence or on
   * another rank of u */
  bool isSuitableForCobweb(Poincare::Context *context) const {
    return type() == Shared::Sequence::Type::SingleRecurrence &&
           !mainExpressionContainsForbiddenTerms(context, true, true);
  }
  /* Sequence u can be handled as explicit if main expression does not contains
   * forbidden terms:
   * - explicit: any term of u
   * - simple recurrence: any term of u other than u(0)
   * - double recurrence: any term of u other than u(1), u(0) */
  bool canBeHandledAsExplicit(Poincare::Context *context) const {
    return !mainExpressionContainsForbiddenTerms(context, false, false);
  }
  int order() const { return static_cast<int>(type()); }
  int firstNonInitialRank() const { return initialRank() + order(); }

  // Approximation
  Poincare::Coordinate2D<float> evaluateXYAtParameter(
      float x, Poincare::Context *context,
      int subCurveIndex = 0) const override {
    return Poincare::Coordinate2D<float>(x, privateEvaluateYAtX(x, context));
  }
  Poincare::Coordinate2D<double> evaluateXYAtParameter(
      double x, Poincare::Context *context,
      int subCurveIndex = 0) const override {
    return Poincare::Coordinate2D<double>(x, privateEvaluateYAtX(x, context));
  }
  template <typename T>
  T approximateAtContextRank(SequenceContext *sqctx,
                             bool intermediateComputation) const;
  template <typename T>
  T approximateAtRank(int rank, SequenceContext *sqctx) const;

  Poincare::Expression sumBetweenBounds(
      double start, double end, Poincare::Context *context) const override;
  constexpr static int k_maxInitialRank =
      255;  // m_initialRank is capped by 255
  // 255 + 1 (to take into account a double recursive sequence) fits in 3 digits
  constexpr static int k_initialRankNumberOfDigits = 3;

  // Range
  bool basedOnCostlyAlgorithms(Poincare::Context *context) const override {
    return true;
  }

 private:
  constexpr static KDFont::Size k_layoutFont = KDFont::Size::Large;

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
          m_displaySum(false) {}
    Type type() const { return m_type; }
    void setType(Type type) { m_type = type; }
    int initialRank() const {
      assert(m_initialRank >= 0);
      return m_initialRank;
    }
    void setInitialRank(int initialRank) { m_initialRank = initialRank; }
    size_t initialConditionSize(int conditionIndex) {
      assert(conditionIndex >= 0 && conditionIndex < 2);
      return m_initialConditionSizes[conditionIndex];
    }
    void setInitialConditionSize(uint16_t size, int conditionIndex) {
      assert(conditionIndex >= 0 && conditionIndex < 2);
      m_initialConditionSizes[conditionIndex] = size;
    }
    bool displaySum() const { return m_displaySum; }
    void setDisplaySum(bool display) { m_displaySum = display; }

   private:
    static_assert((1 << 8 * sizeof(uint16_t)) >=
                      Ion::Storage::FileSystem::k_storageSize,
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
    bool m_displaySum;
  };

  class SequenceModel : public ExpressionModel {
   public:
    using ExpressionModel::ExpressionModel;
    Poincare::Layout name(Sequence *sequence);
    void tidyDownstreamPoolFrom(char *treePoolCursor) const override;
    void tidyName(char *treePoolCursor = nullptr) const;

   protected:
    virtual void buildName(Sequence *sequence) = 0;
    mutable Poincare::Layout m_name;

   private:
    void updateNewDataWithExpression(
        Ion::Storage::Record *record,
        const Poincare::Expression &expressionToStore, void *expressionAddress,
        size_t newExpressionSize, size_t previousExpressionSize) override;
    virtual void updateMetaData(const Ion::Storage::Record *record,
                                size_t newSize) {}
  };

  class DefinitionModel : public SequenceModel {
   private:
    void *expressionAddress(const Ion::Storage::Record *record) const override;
    size_t expressionSize(const Ion::Storage::Record *record) const override;
    void buildName(Sequence *sequence) override;
  };

  class InitialConditionModel : public SequenceModel {
   private:
    void updateMetaData(const Ion::Storage::Record *record,
                        size_t newSize) override;
    void *expressionAddress(const Ion::Storage::Record *record) const override;
    size_t expressionSize(const Ion::Storage::Record *record) const override;
    void buildName(Sequence *sequence) override;
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
  T privateEvaluateYAtX(T x, Poincare::Context *context) const;
  size_t metaDataSize() const override { return sizeof(RecordDataBuffer); }
  const ExpressionModel *model() const override { return &m_definition; }
  RecordDataBuffer *recordData() const;

  bool mainExpressionContainsForbiddenTerms(Poincare::Context *context,
                                            bool allowRecursion,
                                            bool forCobweb) const;
  /* Sequence u is not computable if main expression contains forbidden terms:
   * - explicit: any term of u
   * - simple recurrence: any term of u other than u(n), u(0)
   * - double recurrence: any term of u other than u(n+1), u(n), u(1), u(0) */
  bool mainExpressionIsNotComputable(Poincare::Context *context) const {
    return mainExpressionContainsForbiddenTerms(context, true, false);
  }

  DefinitionModel m_definition;
  FirstInitialConditionModel m_firstInitialCondition;
  SecondInitialConditionModel m_secondInitialCondition;
};

}  // namespace Shared

#endif
