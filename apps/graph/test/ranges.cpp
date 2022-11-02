#include <quiz.h>
#include "helper.h"
#include <apps/shared/global_context.h>

using namespace Poincare;
using namespace Shared;

namespace Graph {

class AdHocGraphController : public InteractiveCurveViewRangeDelegate {
public:
  /* These margins are obtained from instance methods of the various derived
   * class of SimpleInteractiveCurveViewController. As we cannot create an
   * instance of this class here, we define those directly. */
  constexpr static float k_topMargin = 0.068f;
  constexpr static float k_bottomMargin = 0.132948f;
  constexpr static float k_leftMargin = 0.04f;
  constexpr static float k_rightMargin = 0.04f;

  static float Ratio() { return InteractiveCurveViewRange::NormalYXRatio() / (1.f + k_topMargin + k_bottomMargin); }

  Context * context() { return &m_context; }
  ContinuousFunctionStore * functionStore() const { return &m_store; }

  // InteractiveCurveViewRangeDelegate
  bool defaultRangeIsNormalized() const override { return functionStore()->displaysFunctionsToNormalize(); }
  void computeXRange(float xMinLimit, float xMaxLimit, float * xMin, float * xMax, float * yMinIntrinsic, float * yMaxIntrinsic) override { DefaultComputeXRange(xMinLimit, xMaxLimit, xMin, xMax, yMinIntrinsic, yMaxIntrinsic, context(), functionStore()); }
  void computeYRange(float xMin, float xMax, float yMinIntrinsic, float yMaxIntrinsic, float * yMin, float * yMax, bool optimizeRange) override { DefaultComputeYRange(xMin, xMax, yMinIntrinsic, yMaxIntrinsic, Ratio(), yMin, yMax, context(), functionStore(), optimizeRange); }
  void improveFullRange(float * xMin, float * xMax, float * yMin, float * yMax) override { DefaultImproveFullRange(xMin, xMax, yMin, yMax, context(), functionStore()); }
  bool canShrinkWhenNormalizing() const override { return true; }

  float addMargin(float x, float range, bool isVertical, bool isMin) override { return DefaultAddMargin(x, range, isVertical, isMin, k_topMargin, k_bottomMargin, k_leftMargin, k_rightMargin); }
  void updateZoomButtons() override {}
  void updateBottomMargin() override {}
  void tidyModels() override {}

private:
  mutable GlobalContext m_context;
  mutable ContinuousFunctionStore m_store;
};

template <size_t N>
void assert_best_range_is(const char * const (&definitions)[N], float targetXMin, float targetXMax, float targetYMin, float targetYMax, Poincare::Preferences::AngleUnit angleUnit = Radian) {
  assert(std::isfinite(targetXMin) && std::isfinite(targetXMax) && std::isfinite(targetYMin) && std::isfinite(targetYMax)
      && targetXMin < targetXMax && targetYMin < targetYMax);

  Preferences::sharedPreferences()->setAngleUnit(angleUnit);

  AdHocGraphController graphController;
  InteractiveCurveViewRange graphRange(&graphController);

  for (size_t i = 0; i < N; i++) {
    addFunction(definitions[i], graphController.functionStore(), graphController.context());
  }
  graphRange.setZoomAuto(true);
  graphRange.computeRanges();
  float xMin = graphRange.xMin();
  float xMax = graphRange.xMax();
  float yMin = graphRange.yMin();
  float yMax = graphRange.yMax();
  assert_roughly_equal(xMin, targetXMin, 10.0f * FLT_EPSILON);
  assert_roughly_equal(xMax, targetXMax, 10.0f * FLT_EPSILON);
  assert_roughly_equal(yMin, targetYMin, 10.0f * FLT_EPSILON);
  assert_roughly_equal(yMax, targetYMax, 10.0f * FLT_EPSILON);

  graphController.functionStore()->removeAll();
}

void assert_best_range_is(const char * definition, float targetXMin, float targetXMax, float targetYMin, float targetYMax, Poincare::Preferences::AngleUnit angleUnit = Radian) {
  const char * definitionArray[1] = { definition };
  assert_best_range_is(definitionArray, targetXMin, targetXMax, targetYMin, targetYMax, angleUnit);
}

QUIZ_CASE(graph_ranges_single_function) {
  Preferences::ComplexFormat previousComplexFormat = Preferences::sharedPreferences()->complexFormat();
  Preferences::sharedPreferences()->setComplexFormat(Preferences::ComplexFormat::Cartesian);

  assert_best_range_is("f(x)=undef", -10, 10, -5.66249943, 4.96249962, Radian);
  assert_best_range_is("f(x)=x!", -10, 10, -5.66249943, 4.96249962);
  assert_best_range_is("f(x)=abs(x)", -10, 10, -1.702784, 8.922215);

  assert_best_range_is("f(x)=0", -10, 10, -5.66249943, 4.96249962);
  assert_best_range_is("f(x)=1", -10, 10, -4.66249943, 5.96249962);
  assert_best_range_is("f(x)=-100", -10, 10, -105.662506, -95.0375061);
  assert_best_range_is("f(x)=0.01", -10, 10, -5.66249943, 4.96249962);
  assert_best_range_is("f(x)=x", -10, 10, -5.66249943, 4.96249962);
  assert_best_range_is("f(x)=x+1", -12, 10, -6.14374924, 5.54374981);
  assert_best_range_is("f(x)=-x+5", -7, 16, -6.10937452, 6.10937452);
  assert_best_range_is("f(x)=x/2+2", -15, 8, -6.10937452, 6.10937452);
  assert_best_range_is("f(x)=x^2", -10, 10, -7, 40);
  assert_best_range_is("f(x)=x^3", -10, 10, -5.16249943, 5.46249962);
  assert_best_range_is("f(x)=-2x^6", -10, 10, -16000, 2000);
  assert_best_range_is("f(x)=3x^2+x+10", -12, 11, -50, 260);

  assert_best_range_is("f(x)=1/x", -4, 4, -2.27499962, 1.97499979);
  assert_best_range_is("f(x)=1/(1-x)", -2.9000001, 4.9000001, -2.12187481, 2.0218749);
  assert_best_range_is("f(x)=1/(x^2+1)", -3.10000014, 3.10000014, -1.24687481, 2.046875);

  assert_best_range_is("f(x)=sin(x)", -15, 15, -1.39999998, 1.20000005, Radian);
  assert_best_range_is("f(x)=cos(x)", -1000, 1000, -1.39999998, 1.20000005, Degree);
  assert_best_range_is("f(x)=tan(x)", -900, 900, -3.5, 3.10000014, Gradian);
  assert_best_range_is("f(x)=tan(x-100)", -1100, 1100, -3.9000001, 3.4000001, Gradian);

  assert_best_range_is("f(x)=e^x", -10, 10, -1.71249962, 8.91249943);
  assert_best_range_is("f(x)=e^x+4", -10, 10, 2.28750038, 12.9124994);
  assert_best_range_is("f(x)=e^(-x)", -10, 10, -1.71249962, 8.91249943);
  assert_best_range_is("f(x)=(1-x)e^(1/(1-x))", -1.62682521, 2.726825, -3, 5.0999999);
  assert_best_range_is("f(x)=(e^x-1)/(e^x+1)", -3.7, 3.7, -2.115625, 1.815625);

  assert_best_range_is("f(x)=ln(x)", -1.9, 6.8, -2.36093712, 2.26093721);
  assert_best_range_is("f(x)=log(x)", -0.9, 3.1, -1.5125, 0.6125);

  assert_best_range_is("f(x)=√(x)", -3, 9, -1.83749962, 4.53749943);
  assert_best_range_is("f(x)=√(x-1)", -2, 10, -1.8875, 4.4875);
  assert_best_range_is("f(x)=√(x^2+1)-x", -10, 10, -1.26249981, 9.36249924);
  assert_best_range_is("f(x)=root(x,3)", -0.8, 2.9, -0.332812428, 1.63281238);
  assert_best_range_is("f(x)=root(x^3+1,3)-x", -2, 2.29999995, -0.392187476, 1.89218748);
  assert_best_range_is("f(x)=x^x", -0.9, 3.1, -1.3, 7.7);
  // TODO : Fix ranges for (x-1)^(x-1) with any complex format
  assert_best_range_is("f(x)=(x-1)^(x-1)", -66.4319, 37.4319458, -23.0888424, 32.0888443);

  // Ranges with Real complex format
  Preferences::sharedPreferences()->setComplexFormat(Preferences::ComplexFormat::Real);
  assert_best_range_is("f(x)=root(x,3)", -3.7, 3.7, -2.1156249, 1.81562483);
  // TODO : Fix ranges for x^x with real complex format
  assert_best_range_is("f(x)=x^x", -11, 6, -1.71562457, 7.31562424);

  // Vertical line
  assert_best_range_is("x=-1", -11, 9, -5.66249943, 4.962499);

  // Restore previous complex format
  Preferences::sharedPreferences()->setComplexFormat(previousComplexFormat);
}

QUIZ_CASE(graph_ranges_several_functions) {
  {
    const char * definitions[] = {"f(x)=e^x", "g(x)=ln(x)"};
    assert_best_range_is(definitions, -1.9, 6.8, -8, 35);
  }
  {
    const char * definitions[] = {"f(x)=x/2+2", "g(x)=-x+5"};
    assert_best_range_is(definitions, -16, 17, -5.76562405, 11.765624);
  }
  {
    const char * definitions[] = {"r=sin(θ)", "r=cos(θ)"};
    assert_best_range_is(definitions, -1.63235319, 2.13235331, -0.800000011, 1.20000005);
  }
  {
    const char * definitions[] = {"f(x)=e^(i×x)", "g(t)=[[re(f(t))][im(f(t))]]"};
    assert_best_range_is(definitions, -2.44705892, 2.44705892, -1.4, 1.2);
  }
}

void assert_zooms_to(float xMin, float xMax, float yMin, float yMax, float targetXMin, float targetXMax, float targetYMin, float targetYMax, bool conserveRatio, bool zoomIn) {
  float ratio = zoomIn ? 1.f / ZoomCurveViewController::k_zoomOutRatio : ZoomCurveViewController::k_zoomOutRatio;

  InteractiveCurveViewRange graphRange;
  graphRange.setXMin(xMin);
  graphRange.setXMax(xMax);
  graphRange.setYMin(yMin);
  graphRange.setYMax(yMax);

  float xCenter = (xMax + xMin) / 2.f;
  float yCenter = (yMax + yMin) / 2.f;

  graphRange.zoom(ratio, xCenter, yCenter);

  assert_roughly_equal(graphRange.xMin(), targetXMin);
  assert_roughly_equal(graphRange.xMax(), targetXMax);
  assert_roughly_equal(graphRange.yMin(), targetYMin);
  assert_roughly_equal(graphRange.yMax(), targetYMax);
  quiz_assert(roughly_equal((yMax - yMin) / (xMax - xMin), (targetYMax - targetYMin) / (targetXMax - targetXMin)) == conserveRatio);
}

void assert_zooms_in_to(float xMin, float xMax, float yMin, float yMax, float targetXMin, float targetXMax, float targetYMin, float targetYMax, bool conserveRatio) {
  assert_zooms_to(xMin, xMax, yMin, yMax, targetXMin, targetXMax, targetYMin, targetYMax, conserveRatio, true);
}

void assert_zooms_out_to(float xMin, float xMax, float yMin, float yMax, float targetXMin, float targetXMax, float targetYMin, float targetYMax, bool conserveRatio) {
  assert_zooms_to(xMin, xMax, yMin, yMax, targetXMin, targetXMax, targetYMin, targetYMax, conserveRatio, false);
}

QUIZ_CASE(graph_ranges_zoom) {
  assert_zooms_in_to(
      -12, 12, -12, 12,
      -8, 8, -8, 8,
      true);

  assert_zooms_in_to(
      -3, 3, 0, 1e-4,
      -3, 3, 0, 1e-4,
      true);

  assert_zooms_out_to(
      -10, 10, -10, 10,
      -15, 15, -15, 15,
      true);

  assert_zooms_out_to(
      -1, 1, 9e7, 1e8,
      -1.5, 1.5, 87500000, 1e8,
      false);
}

void assert_orthonormality(float xMin, float xMax, float yMin, float yMax, bool orthonormal) {
  InteractiveCurveViewRange graphRange;
  graphRange.setXMin(xMin);
  graphRange.setXMax(xMax);
  graphRange.setYMin(yMin);
  graphRange.setYMax(yMax);

  quiz_assert(graphRange.isOrthonormal() == orthonormal);
}

void assert_is_orthonormal(float xMin, float xMax, float yMin, float yMax) {
  assert_orthonormality(xMin, xMax, yMin, yMax, true);
}

void assert_is_not_orthonormal(float xMin, float xMax, float yMin, float yMax) {
  assert_orthonormality(xMin, xMax, yMin, yMax, false);
}

QUIZ_CASE(graph_ranges_orthonormal) {
  assert_is_orthonormal(-10, 10, -5.8125, 4.8125);
  assert_is_orthonormal(11.37037, 17.2963, 7.529894, 10.67804);
  assert_is_orthonormal(-1.94574, -1.165371, -2.476379, -2.061809);
  assert_is_orthonormal(0, 1000000, 0, 531250);
  assert_is_orthonormal(-3.2e-3f, 3.2e-3f, -1.7e-3f, 1.7e-3f);
  assert_is_not_orthonormal(-10, 10, -10, 10);
  assert_is_not_orthonormal(-10, 10, -5.8125, 4.8126);
  assert_is_not_orthonormal(1234548, 1234568, 1234556, 1234568);

  /* The ratio is 0.55 instead of 0.53125, but depending on the magnitude of
   * the bounds, it can land inside the margin of error. */
  assert_is_not_orthonormal(0, 20, 0, 11);
  assert_is_orthonormal(1e6, 1e6 + 20, 1e6, 1e6 + 11);

  /* The ratio is the desired 0.53125, but if the bounds are near equal
   * numbers of large magnitude, the loss of precision leaves us without any
   * significant bits. */
  assert_is_orthonormal(0, 3.2, 0, 1.7);
  assert_is_not_orthonormal(1e7, 1e7 + 3.2, 0, 1.7);
}

}
