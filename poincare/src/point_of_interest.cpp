#include <poincare/point_of_interest.h>
#include <poincare_junior/src/expression/point_of_interest.h>
#include <poincare_junior/src/memory/edition_pool.h>
#include <poincare_junior/src/memory/tree.h>

namespace Poincare {

// PointOfInterest

PointOfInterest PointOfInterest::Builder(
    double abscissa, double ordinate,
    typename Solver<double>::Interest interest, uint32_t data, bool inverted,
    int subCurveIndex) {
  JuniorExpression expr = JuniorExpression::Builder(
      PoincareJ::SharedTreeStack->push<PoincareJ::Type::PointOfInterest>(
          abscissa, ordinate, data, static_cast<uint8_t>(interest), inverted,
          static_cast<uint8_t>(subCurveIndex)));
  return static_cast<PointOfInterest &>(expr);
}

double PointOfInterest::abscissa() const {
  return PoincareJ::PointOfInterest::GetAbscissa(tree());
}

double PointOfInterest::ordinate() const {
  return PoincareJ::PointOfInterest::GetOrdinate(tree());
}

double PointOfInterest::x() const {
  return PoincareJ::PointOfInterest::IsInverted(tree()) ? ordinate()
                                                        : abscissa();
}

double PointOfInterest::y() const {
  return PoincareJ::PointOfInterest::IsInverted(tree()) ? abscissa()
                                                        : ordinate();
}

int PointOfInterest::subCurveIndex() const {
  return PoincareJ::PointOfInterest::GetSubCurveIndex(tree());
}

typename Solver<double>::Interest PointOfInterest::interest() const {
  return static_cast<Solver<double>::Interest>(
      PoincareJ::PointOfInterest::GetInterest(tree()));
}

Coordinate2D<double> PointOfInterest::xy() const {
  return isUninitialized() ? Coordinate2D<double>()
                           : Coordinate2D<double>(x(), y());
}

uint32_t PointOfInterest::data() const {
  return PoincareJ::PointOfInterest::GetData(tree());
}

// PointsOfInterestList

PointOfInterest PointsOfInterestList::pointAtIndex(int i) const {
  assert(!m_list.isUninitialized());
  assert(0 <= i && i < m_list.numberOfChildren());
  /* We need to call TreeHandle::childAtIndex instead of
   * OExpression::childAtIndex, since a PointOfInterest is not an OExpression.
   */
  TreeHandle h = static_cast<const TreeHandle &>(m_list).childAtIndex(i);
  return static_cast<PointOfInterest &>(h);
}

void PointsOfInterestList::append(double abscissa, double ordinate,
                                  uint32_t data,
                                  typename Solver<double>::Interest interest,
                                  bool inverted, int subCurveIndex) {
  assert(!m_list.isUninitialized());
  int n = m_list.numberOfChildren();
  if (interest == Solver<double>::Interest::Root) {
    // Sometimes the root is close to zero but not exactly zero
    ordinate = 0.0;
  }
  m_list.addChildAtIndexInPlace(
      PointOfInterest::Builder(abscissa, ordinate, interest, data, inverted,
                               subCurveIndex),
      n, n);
}

void PointsOfInterestList::sort() {
  Helpers::Sort(
      [](int i, int j, void *context, int numberOfElements) {
        List list = static_cast<PointsOfInterestList *>(context)->list();
        list.swapChildrenInPlace(i, j);
      },
      [](int i, int j, void *context, int numberOfElements) {
        PointsOfInterestList *pointsList =
            static_cast<PointsOfInterestList *>(context);
        return pointsList->pointAtIndex(i).abscissa() >
               pointsList->pointAtIndex(j).abscissa();
      },
      (void *)this, numberOfPoints());
}

}  // namespace Poincare
