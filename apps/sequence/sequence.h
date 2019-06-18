#ifndef SEQUENCE_SEQUENCE_H
#define SEQUENCE_SEQUENCE_H

#include "../shared/function.h"
#include "sequence_context.h"
#include <assert.h>

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Sequence {

/* WARNING: after calling setType, setInitialRank, setContent, setFirstInitialConditionContent
 * or setSecondInitialConditionContent, the sequence context needs to
 * invalidate the cache because the sequences evaluations might have changed. */

class Sequence : public Shared::Function {
friend class SequenceStore;
public:
  enum class Type : uint8_t {
    Explicit = 0,
    SingleRecurrence = 1,
    DoubleRecurrence = 2
  };
  Sequence(Ion::Storage::Record record = Record()) :
    Function(record),
    m_nameLayout() {}
  static char Symbol() { return 'n'; }
  void tidy() override;
  // MetaData getters
  Type type() const;
  int initialRank() const;
  // MetaData setters
  void setType(Type type);
  void setInitialRank(int rank);
  // Definition
  Poincare::Layout definitionName() { return m_definition.name(this); }
  Ion::Storage::Record::ErrorStatus setContent(const char * c) override { return editableModel()->setContent(this, c, Symbol(), UCodePointUnknownN); }
  // First initial condition
  Poincare::Layout firstInitialConditionName() { return m_firstInitialCondition.name(this); }
  void firstInitialConditionText(char * buffer, size_t bufferSize) const { return m_firstInitialCondition.text(this, buffer, bufferSize); }
  Poincare::Expression firstInitialConditionExpressionReduced(Poincare::Context * context) const { return m_firstInitialCondition.expressionReduced(this, context); }
  Poincare::Expression firstInitialConditionExpressionClone() const { return m_firstInitialCondition.expressionClone(this); }
  Poincare::Layout firstInitialConditionLayout() { return m_firstInitialCondition.layout(this); }
  Ion::Storage::Record::ErrorStatus setFirstInitialConditionContent(const char * c) { return m_firstInitialCondition.setContent(this, c); }
  // Second initial condition
  Poincare::Layout secondInitialConditionName() { return m_secondInitialCondition.name(this); }
  void secondInitialConditionText(char * buffer, size_t bufferSize) const { return m_secondInitialCondition.text(this, buffer, bufferSize); }
  Poincare::Expression secondInitialConditionExpressionReduced(Poincare::Context * context) const { return m_secondInitialCondition.expressionReduced(this, context); }
  Poincare::Expression secondInitialConditionExpressionClone() const { return m_secondInitialCondition.expressionClone(this); }
  Poincare::Layout secondInitialConditionLayout() { return m_secondInitialCondition.layout(this); }
  Ion::Storage::Record::ErrorStatus setSecondInitialConditionContent(const char * c) { return m_secondInitialCondition.setContent(this, c); }

  // Sequence properties
  int numberOfElements() { return (int)type() + 1; }
  Poincare::Layout nameLayout();
  bool isDefined() override;
  bool isEmpty() override;
  // Approximation
  float evaluateAtAbscissa(float x, Poincare::Context * context) const override {
    return templatedApproximateAtAbscissa(x, static_cast<SequenceContext *>(context));
  }
  double evaluateAtAbscissa(double x, Poincare::Context * context) const override {
    return templatedApproximateAtAbscissa(x, static_cast<SequenceContext *>(context));
  }
  template<typename T> T approximateToNextRank(int n, SequenceContext * sqctx) const;
  // Integral
  double sumBetweenBounds(double start, double end, Poincare::Context * context) const override;

  constexpr static int k_initialRankNumberOfDigits = 3; // m_initialRank is capped by 999
private:
  constexpr static const KDFont * k_layoutFont = KDFont::LargeFont;
  constexpr static double k_maxNumberOfTermsInSum = 100000.0;

  /* SequenceRecordDataBuffer is the layout of the data buffer of Record
   * representing a Sequence. See comment in
   * Shared::Function::FunctionRecordDataBuffer about packing. */
#pragma pack(push,1)
  class SequenceRecordDataBuffer : public FunctionRecordDataBuffer {
  public:
    SequenceRecordDataBuffer(KDColor color) :
      FunctionRecordDataBuffer(color),
      m_type(Type::Explicit),
      m_initialRank(0),
      m_initialConditionSizes{0,0}
    {}
    Type type() const { return m_type; }
    void setType(Type type) { m_type = type; }
    int initialRank() const { return m_initialRank; }
    void setInitialRank(int initialRank) { m_initialRank = initialRank; }
    size_t initialConditionSize(int conditionIndex) {
      assert(conditionIndex >= 0 && conditionIndex < 2);
      return m_initialConditionSizes[conditionIndex];
    }
    void setInitialConditionSize(uint16_t size, int conditionIndex) {
      assert(conditionIndex >= 0 && conditionIndex < 2);
      m_initialConditionSizes[conditionIndex] = size;
    }
  private:
    static_assert((1 << 8*sizeof(uint16_t)) > Ion::Storage::k_storageSize, "Potential overflows of Sequence initial condition sizes");
    Type m_type;
    uint8_t m_initialRank;
#if __EMSCRIPTEN__
    // See comment about emscripten alignement in Shared::Function::FunctionRecordDataBuffer
    static_assert(sizeof(emscripten_align1_short) == sizeof(uint16_t), "emscripten_align1_short should have the same size as uint16_t");
    emscripten_align1_short m_initialConditionSizes[2];
#else
    uint16_t m_initialConditionSizes[2];
#endif
  };
#pragma pack(pop)

  class SequenceModel : public Shared::ExpressionModel {
  public:
    SequenceModel() : Shared::ExpressionModel(), m_name() {}
    void tidyName() { m_name = Poincare::Layout(); }
    virtual Poincare::Layout name(Sequence * sequence);
  protected:
    virtual void buildName(Sequence * sequence) = 0;
    Poincare::Layout m_name;
  private:
    void updateNewDataWithExpression(Ion::Storage::Record * record, Poincare::Expression & newExpression, void * expressionAddress, size_t newExpressionSize, size_t previousExpressionSize) override;
    virtual void updateMetaData(const Ion::Storage::Record * record, size_t newSize) {}
  };

  class DefinitionModel : public SequenceModel {
  public:
    void * expressionAddress(const Ion::Storage::Record * record) const override;
  private:
    size_t expressionSize(const Ion::Storage::Record * record) const override;
    void buildName(Sequence * sequence) override;
  };

  class InitialConditionModel : public SequenceModel {
  public:
    void * expressionAddress(const Ion::Storage::Record * record) const override;
  private:
    void updateMetaData(const Ion::Storage::Record * record, size_t newSize) override;
    size_t expressionSize(const Ion::Storage::Record * record) const override;
    void buildName(Sequence * sequence) override;
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

  template<typename T> T templatedApproximateAtAbscissa(T x, SequenceContext * sqctx) const;
  size_t metaDataSize() const override { return sizeof(SequenceRecordDataBuffer); }
  const Shared::ExpressionModel * model() const override { return &m_definition; }
  SequenceRecordDataBuffer * recordData() const;
  DefinitionModel m_definition;
  FirstInitialConditionModel m_firstInitialCondition;
  SecondInitialConditionModel m_secondInitialCondition;
  Poincare::Layout m_nameLayout;
};

}

#endif
