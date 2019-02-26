#ifndef SEQUENCE_SEQUENCE_H
#define SEQUENCE_SEQUENCE_H

#include "../shared/storage_function.h"
#include "sequence_context.h"
#include <assert.h>

namespace Sequence {

/* WARNING: after calling setType, setInitialRank, setContent, setFirstInitialConditionContent
 * or setSecondInitialConditionContent, the sequence context needs to
 * invalidate the cache because the sequences evaluations might have changed. */

class Sequence : public Shared::StorageFunction {
friend class SequenceStore;
public:
  static constexpr char extension[] = "seq"; // TODO: store this elsewhere?


  enum class Type {
    Explicit = 0,
    SingleRecurrence = 1,
    DoubleRecurrence = 2
  };
  Sequence(Ion::Storage::Record record = Record()) :
    StorageFunction(record),
    m_nameLayout() {}
  static char Symbol() { return 'n'; }
  char symbol() const override { return Symbol(); }
  void tidy() override;
  // MetaData getters
  Type type() const;
  int initialRank() const;
  // MetaData setters
  void setType(Type type);
  void setInitialRank(int rank);
  // Definition
  Poincare::Layout definitionName() { return m_definitionHandle.name(this); }
  // First initial condition
  Poincare::Layout firstInitialConditionName() { return m_firstInitialConditionHandle.name(this); }
  void firstInitialConditionText(char * buffer, size_t bufferSize) const { return m_firstInitialConditionHandle.text(this, buffer, bufferSize); }
  Poincare::Expression firstInitialConditionExpressionReduced(Poincare::Context * context) const { return m_firstInitialConditionHandle.expressionReduced(this, context); }
  Poincare::Expression firstInitialConditionExpressionClone() const { return m_firstInitialConditionHandle.expressionClone(this); }
  Poincare::Layout firstInitialConditionLayout() { return m_firstInitialConditionHandle.layout(this); }
  Ion::Storage::Record::ErrorStatus setFirstInitialConditionContent(const char * c) { return m_firstInitialConditionHandle.setContent(this, c); }
  // Second initial condition
  Poincare::Layout secondInitialConditionName() { return m_secondInitialConditionHandle.name(this); }
  void secondInitialConditionText(char * buffer, size_t bufferSize) const { return m_secondInitialConditionHandle.text(this, buffer, bufferSize); }
  Poincare::Expression secondInitialConditionExpressionReduced(Poincare::Context * context) const { return m_secondInitialConditionHandle.expressionReduced(this, context); }
  Poincare::Expression secondInitialConditionExpressionClone() const { return m_secondInitialConditionHandle.expressionClone(this); }
  Poincare::Layout secondInitialConditionLayout() { return m_secondInitialConditionHandle.layout(this); }
  Ion::Storage::Record::ErrorStatus setSecondInitialConditionContent(const char * c) { return m_secondInitialConditionHandle.setContent(this, c); }

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

  class SequenceRecordData : public FunctionRecordData {
  public:
    SequenceRecordData(KDColor color) :
      FunctionRecordData(color),
      m_type(Type::Explicit),
      m_initialRank(0),
      m_firstInitialConditionSize(0),
      m_secondInitialConditionSize(0)
    {}
    Type type() const { return m_type; }
    void setType(Type type) { m_type = type; }
    int initialRank() const { return m_initialRank; }
    void setInitialRank(int initialRank) { m_initialRank = initialRank; }
    size_t firstInitialConditionSize() const { return m_firstInitialConditionSize; }
    void setFirstInitialConditionSize(size_t firstInitialConditionSize) { m_firstInitialConditionSize = firstInitialConditionSize; }
    size_t secondInitialConditionSize() const { return m_secondInitialConditionSize; }
    void setSecondInitialConditionSize(size_t secondInitialConditionSize) { m_secondInitialConditionSize = secondInitialConditionSize; }
  private:
    Type m_type;
    int m_initialRank;
    size_t m_firstInitialConditionSize;
    size_t m_secondInitialConditionSize;
  };

  class SequenceHandle : public Shared::ExpressionModelHandle {
  public:
    SequenceHandle() : Shared::ExpressionModelHandle(), m_name() {}
    void tidyName() { m_name = Poincare::Layout(); }
    virtual Poincare::Layout name(Sequence * sequence);
  protected:
    virtual void buildName(Sequence * sequence) = 0;
    Poincare::Layout m_name;
  };

  class DefinitionHandle : public SequenceHandle {
  public:
    void * expressionAddress(const Ion::Storage::Record * record) const override;
  private:
    size_t expressionSize(const Ion::Storage::Record * record) const override;
    void buildName(Sequence * sequence) override;
  };

  class FirstInitialConditionHandle : public SequenceHandle {
  public:
    void * expressionAddress(const Ion::Storage::Record * record) const override;
  private:
    size_t expressionSize(const Ion::Storage::Record * record) const override;
    void buildName(Sequence * sequence) override;
  };

  class SecondInitialConditionHandle : public SequenceHandle {
  public:
    void * expressionAddress(const Ion::Storage::Record * record) const override;
  private:
    size_t expressionSize(const Ion::Storage::Record * record) const override;
    void buildName(Sequence * sequence) override;
  };

  template<typename T> T templatedApproximateAtAbscissa(T x, SequenceContext * sqctx) const;
  size_t metaDataSize() const override { return sizeof(SequenceRecordData); }
  const Shared::ExpressionModelHandle * handle() const override { return &m_definitionHandle; }
  SequenceRecordData * recordData() const;
  DefinitionHandle m_definitionHandle;
  FirstInitialConditionHandle m_firstInitialConditionHandle;
  SecondInitialConditionHandle m_secondInitialConditionHandle;
  Poincare::Layout m_nameLayout;
};

}

#endif
