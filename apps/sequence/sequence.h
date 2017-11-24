#ifndef SEQUENCE_SEQUENCE_H
#define SEQUENCE_SEQUENCE_H

#include "../shared/function.h"
#include <assert.h>

namespace Sequence {

class Sequence : public Shared::Function {
public:
  enum class Type {
    Explicite = 0,
    SingleRecurrence = 1,
    DoubleRecurrence = 2
  };
  Sequence(const char * text = nullptr, KDColor color = KDColorBlack);
  ~Sequence();
  Sequence& operator=(const Sequence& other);
  Sequence& operator=(Sequence&& other) = delete;
  Sequence(const Sequence& other) = delete;
  Sequence(Sequence&& other) = delete;
  uint32_t checksum() override;
  Type type();
  void setType(Type type);
  const char * firstInitialConditionText();
  const char * secondInitialConditionText();
  Poincare::Expression * firstInitialConditionExpression() const;
  Poincare::Expression * secondInitialConditionExpression() const;
  Poincare::ExpressionLayout * firstInitialConditionLayout();
  Poincare::ExpressionLayout * secondInitialConditionLayout();
  void setContent(const char * c) override;
  void setFirstInitialConditionContent(const char * c);
  void setSecondInitialConditionContent(const char * c);
  int numberOfElements();
  Poincare::ExpressionLayout * nameLayout();
  Poincare::ExpressionLayout * definitionName();
  Poincare::ExpressionLayout * firstInitialConditionName();
  Poincare::ExpressionLayout * secondInitialConditionName();
  bool isDefined() override;
  bool isEmpty() override;
  float evaluateAtAbscissa(float x, Poincare::Context * context) const override {
    return templatedApproximateAtAbscissa(x, context);
  }
  double evaluateAtAbscissa(double x, Poincare::Context * context) const override {
    return templatedApproximateAtAbscissa(x, context);
  }
  double sumOfTermsBetweenAbscissa(double start, double end, Poincare::Context * context);
  void tidy() override;
private:
  constexpr static int k_maxRecurrentRank = 10000;
  constexpr static double k_maxNumberOfTermsInSum = 100000.0;
  constexpr static size_t k_dataLengthInBytes = (3*TextField::maxBufferSize()+3)*sizeof(char)+1;
  static_assert((k_dataLengthInBytes & 0x3) == 0, "The sequence data size is not a multiple of 4 bytes (cannot compute crc)"); // Assert that dataLengthInBytes is a multiple of 4
  char symbol() const override;
  template<typename T> T templatedApproximateAtAbscissa(T x, Poincare::Context * context) const;
  Type m_type;
  char m_firstInitialConditionText[TextField::maxBufferSize()];
  char m_secondInitialConditionText[TextField::maxBufferSize()];
  mutable Poincare::Expression * m_firstInitialConditionExpression;
  mutable Poincare::Expression * m_secondInitialConditionExpression;
  Poincare::ExpressionLayout * m_firstInitialConditionLayout;
  Poincare::ExpressionLayout * m_secondInitialConditionLayout;
  Poincare::ExpressionLayout * m_nameLayout;
  Poincare::ExpressionLayout * m_definitionName;
  Poincare::ExpressionLayout * m_firstInitialConditionName;
  Poincare::ExpressionLayout * m_secondInitialConditionName;
  /* In order to accelerate the computation of values of recurrent sequences,
   * we memoize the last computed values of the sequence and their associated
   * ranks (n and n+1 for instance). Thereby, when another evaluation at a
   * superior rank k > n+1 is called, we avoid iterating from 0 but can start
   * from n. */
  constexpr static int k_maxRecurrenceDepth = 2;
  mutable int m_indexBufferFloat[k_maxRecurrenceDepth];
  mutable int m_indexBufferDouble[k_maxRecurrenceDepth];
  mutable float m_bufferFloat[k_maxRecurrenceDepth];
  mutable double m_bufferDouble[k_maxRecurrenceDepth];
  void resetBuffer() const;
  template<typename T> void setBufferValue(T value, int i) const {
    assert(i >= 0 && i < k_maxRecurrentRank);
    if (sizeof(T) == sizeof(float)) {
      m_bufferFloat[i] = value;
    } else {
      m_bufferDouble[i] = value;
    }
  }
  template<typename T> void setBufferIndexValue(int index, int i) const {
    assert(i >= 0 && i < k_maxRecurrentRank);
    if (sizeof(T) == sizeof(float)) {
      m_indexBufferFloat[i] = index;
    } else {
      m_indexBufferDouble[i] = index;
    }
  }
  template<typename T> T bufferValue(int i) const {
    assert(i >= 0 && i < k_maxRecurrentRank);
    if (sizeof(T) == sizeof(float)) {
      return m_bufferFloat[i];
    } else {
      return m_bufferDouble[i];
    }
  }
  template<typename T> int indexBuffer(int i) const {
    assert(i >= 0 && i < k_maxRecurrentRank);
    if (sizeof(T) == sizeof(float)) {
      return m_indexBufferFloat[i];
    } else {
      return m_indexBufferDouble[i];
    }
  }
};

}

#endif
