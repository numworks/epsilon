#include <poincare/old/float.h>
#include <poincare/old/old_expression.h>
#include <poincare/old/point.h>
#include <poincare/old/point_evaluation.h>

namespace Poincare {

template <typename T>
OExpression PointEvaluationNode<T>::complexToExpression(
    Preferences::ComplexFormat complexFormat) const {
  return OPoint::Builder(Float<T>::Builder(m_x), Float<T>::Builder(m_y));
}

template <typename T>
PointEvaluation<T> PointEvaluation<T>::Builder(T x, T y) {
  void *bufferNode = Pool::sharedPool->alloc(sizeof(PointEvaluationNode<T>));
  PointEvaluationNode<T> *node = new (bufferNode) PointEvaluationNode<T>(x, y);
  PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
  return static_cast<PointEvaluation<T> &>(h);
}

template PointEvaluation<float> PointEvaluation<float>::Builder(float x,
                                                                float y);
template PointEvaluation<double> PointEvaluation<double>::Builder(double x,
                                                                  double y);

}  // namespace Poincare
