#ifndef POINCARE_POINT_EVALUATION_H
#define POINCARE_POINT_EVALUATION_H

#include "coordinate_2D.h"
#include "evaluation.h"

namespace Poincare {

template <typename T>
class PointEvaluationNode final : public EvaluationNode<T> {
 public:
  PointEvaluationNode(T x, T y) : m_x(x), m_y(y) {}

  Coordinate2D<T> xy() const { return Coordinate2D<T>(m_x, m_y); }

  // EvaluationNode
  typename EvaluationNode<T>::Type otype() const override {
    return EvaluationNode<T>::Type::PointEvaluation;
  }
  bool isUndefined() const override {
    return std::isnan(m_x) || std::isnan(m_y);
  }
  std::complex<T> complexAtIndex(int index) const override {
    return std::complex<T>(NAN, NAN);
  }
  OExpression complexToExpression(
      Preferences::ComplexFormat complexFormat) const override;

  // PoolHandle
  size_t size() const override { return sizeof(PointEvaluationNode); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "PointEvaluation";
  }
  void logAttributes(std::ostream& stream) const override {
    stream << " x=\"" << m_x << "\"";
    stream << " y=\"" << m_y << "\"";
  }
#endif

 private:
  T m_x;
  T m_y;
};

template <typename T>
class PointEvaluation final : public Evaluation<T> {
 public:
  PointEvaluation(PointEvaluation<T>* node) : Evaluation<T>(node) {}
  static PointEvaluation Builder(T x, T y);
  static PointEvaluation<T> Undefined() {
    return PointEvaluation<T>::Builder(NAN, NAN);
  }

  Coordinate2D<T> xy() const {
    return static_cast<PointEvaluationNode<T>*>(this->node())->xy();
  }
};

}  // namespace Poincare

#endif
