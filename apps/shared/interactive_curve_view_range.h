#ifndef SHARED_INTERACTIVE_CURVE_VIEW_RANGE_H
#define SHARED_INTERACTIVE_CURVE_VIEW_RANGE_H

#include <float.h>
#include <ion/display.h>
#include <poincare/range.h>
#include <stdint.h>

#include "grid_type_controller.h"
#include "interactive_curve_view_range_delegate.h"
#include "memoized_curve_view_range.h"

namespace Shared {

class InteractiveCurveViewRange : public MemoizedCurveViewRange {
 public:
  constexpr static float k_maxFloat = 1E+8f;
  using GridType = GridTypeController::GridType;

  InteractiveCurveViewRange(
      InteractiveCurveViewRangeDelegate* delegate = nullptr)
      : MemoizedCurveViewRange(),
        m_delegate(delegate),
        m_memoizedAutoRange(Poincare::Range1D<float>(),
                            Poincare::Range1D<float>()),
        m_checksumOfMemoizedAutoRange(0),
        m_offscreenYAxis(0.f),
        m_gridType(GridType::Cartesian),
        m_zoomAuto{true, true},
        m_zoomNormalize(false) {}

  constexpr static float NormalYXRatio() {
    return NormalizedYHalfRange(1.f) / NormalizedXHalfRange(1.f);
  }
  /* The method isOrthonormal takes the loss of significance when changing the
   * ratio into account. */
  bool isOrthonormal() const;

  void setDelegate(InteractiveCurveViewRangeDelegate* delegate);

  bool zoomAuto() const { return m_zoomAuto.x && m_zoomAuto.y; }
  void setZoomAuto(bool v) { privateSetZoomAuto(v, v); }
  bool zoomAuto(Axis axis) const { return m_zoomAuto(axis); }
  void setZoomAuto(Axis axis, bool v) {
    BoolPair newAuto = m_zoomAuto;
    newAuto.set(axis, v);
    privateSetZoomAuto(newAuto.x, newAuto.y);
  }
  bool zoomNormalize() const { return m_zoomNormalize; }
  void setZoomNormalize(bool v);
  float roundLimit(float y, float range, bool isMin);

  // MemoizedCurveViewRange
  float xGridUnit() const override;
  float yGridUnit() const override;

  // CurveViewWindow
  void setXRange(float min, float max) override;
  void setYRange(float min, float max) override;

  float offscreenYAxis() const override { return m_offscreenYAxis; }
  void setOffscreenYAxis(float f);

  // Window
  void zoom(float ratio, float x, float y);
  void panWithVector(float x, float y);
  void computeRanges() { privateComputeRanges(m_zoomAuto.x, m_zoomAuto.y); }
  void normalize();
  void centerAxisAround(Axis axis, float position);
  bool panToMakePointVisible(float x, float y, float topMarginRatio,
                             float rightMarginRatio, float bottomMarginRatio,
                             float leftMarginRatio, float pixelWidth);
  bool zoomOutToMakePointVisible(float x, float y, float topMarginRatio,
                                 float rightMarginRatio,
                                 float bottomMarginRatio,
                                 float leftMarginRatio);

  GridType gridType() const { return m_gridType; }
  void setGridType(GridType grid);

 protected:
  constexpr static float k_maxRatioPositionRange = 1E5f;
  /* In normalized settings, we put each axis so that 1cm = 2 units. For now,
   * the screen has size 43.2mm * 57.6mm.
   * We want:
   *   2 * normalizedXHalfRange -> 57.6mm
   *   2 * 1 unit -> 10.0mm
   * So normalizedXHalfRange = 57.6mm * 1 unit / 10.0mm */
  constexpr static float NormalizedXHalfRange(float unit) {
    return 5.76f * unit;
  }
  /* In normalized settings, we put each axis so that 1cm = 2 units. For now,
   * the screen has size 43.2mm * 57.6mm.
   * We want:
   *   2 * normalizedYHalfRange -> 43.2mm * 170/240
   *   2 * 1 unit -> 10.0mm
   * So normalizedYHalfRange = 43.2mm * 170/240 * 1 unit / 10.0mm */
  constexpr static float NormalizedYHalfRange(float unit) {
    return 3.06f * unit;
  }
  virtual bool hasDefaultRange() const {
    return (xMin() == std::round(xMin())) && (xMax() == std::round(xMax()));
  }
  /* This method only updates the zoomNormalize status, and does not change
   * either the auto statuses or the intrinsic Y range. */
  virtual void protectedNormalize(bool canChangeX, bool canChangeY,
                                  bool canShrink);

  InteractiveCurveViewRangeDelegate* m_delegate;

 private:
  void privateSetZoomAuto(bool xAuto, bool yAuto);
  void privateComputeRanges(bool computeX, bool computeY);

  Poincare::Range2D<float> m_memoizedAutoRange;
  uint64_t m_checksumOfMemoizedAutoRange;
  float m_offscreenYAxis;

  GridType m_gridType;

  struct BoolPair {
    bool x, y;
    bool operator()(Axis axis) const { return axis == Axis::X ? x : y; }
    void set(Axis axis, bool value) { (axis == Axis::X ? x : y) = value; }
  };
  BoolPair m_zoomAuto;
  bool m_zoomNormalize;
};

static_assert(Ion::Display::WidthInTenthOfMillimeter == 576,
              "Use the new screen width to compute "
              "Shared::InteractiveCurveViewRange::NormalizedXHalfRange");
static_assert(Ion::Display::HeightInTenthOfMillimeter == 432,
              "Use the new screen height to compute "
              "Shared::InteractiveCurveViewRange::NormalizedYHalfRange");

typedef void (InteractiveCurveViewRange::*ParameterSetterPointer)(float);
typedef float (InteractiveCurveViewRange::*ParameterGetterPointer)() const;
typedef void (InteractiveCurveViewRange::*RangeMethodPointer)();

}  // namespace Shared

#endif
