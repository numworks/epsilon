#include <quiz.h>
#include <string.h>
#include <assert.h>
#include "../model/model.h"
#include "../graph_controller.h"
#include "../store.h"

using namespace Poincare;
using namespace Regression;

void load_in_store(
    Regression::Store * store,
    double * x0, double * y0, int numberOfPoints0,
    double * x1 = nullptr, double * y1 = nullptr, int numberOfPoints1 = 0,
    double * x2 = nullptr, double * y2 = nullptr, int numberOfPoints2 = 0)
{
  // Set the points and the regression type
  if (numberOfPoints0 != 0 && x0 != nullptr && y0 != nullptr) {
    for (int i = 0; i < numberOfPoints0; i++) {
      store->set(x0[i], 0, 0, i);
      store->set(y0[i], 0, 1, i);
    }
  }
  if (numberOfPoints1 != 0 && x1 != nullptr && y1 != nullptr) {
    for (int i = 0; i < numberOfPoints1; i++) {
      store->set(x1[i], 1, 0, i);
      store->set(y1[i], 1, 1, i);
    }
  }
  if (numberOfPoints2 != 0 && x2 != nullptr && y2 != nullptr) {
    for (int i = 0; i < numberOfPoints2; i++) {
      store->set(x2[i], 2, 0, i);
      store->set(y2[i], 2, 1, i);
    }
  }
}

class NavigationEvent {
public:
  NavigationEvent(OMG::Direction d, int series, int dot) :
    direction(d),
    expectedSelectedSeries(series),
    expectedSelectedDot(dot)
  {}
  OMG::Direction direction;
  int expectedSelectedSeries;
  int expectedSelectedDot;
};

void assert_navigation_is(
    int numberOfEvents,
    NavigationEvent * events,
    int startingDotSelection,
    int startingSeriesSelection,
    double * x0, double * y0, int numberOfPoints0,
    double * x1 = nullptr, double * y1 = nullptr, int numberOfPoints1 = 0,
    double * x2 = nullptr, double * y2 = nullptr, int numberOfPoints2 = 0)
{
  assert(startingDotSelection >= 0);
  Store store;
  Shared::CurveViewCursor cursor;
  uint32_t dummy;

  int selectedDotIndex = startingDotSelection;
  int selectedSeriesIndex = startingSeriesSelection;

  load_in_store(&store, x0, y0, numberOfPoints0, x1, y1, numberOfPoints1, x2, y2, numberOfPoints2);

  if (selectedDotIndex < store.numberOfPairsOfSeries(selectedSeriesIndex)) {
    cursor.moveTo(
        store.get(selectedSeriesIndex, 0, selectedDotIndex),
        store.get(selectedSeriesIndex, 1, selectedDotIndex));
  } else {
    cursor.moveTo(
        store.meanOfColumn(selectedSeriesIndex, 0),
        store.meanOfColumn(selectedSeriesIndex, 1));
  }

  AppsContainerStorage container;
  // We do not really care about the snapshot
  App app(&container, container.appSnapshotAtIndex(0));

  GraphController graphController(
      &app, &app, nullptr,
      &store,
      &cursor,
      &dummy, &dummy,
      &selectedDotIndex,
      &selectedSeriesIndex);


  for (int i = 0; i < numberOfEvents; i++) {
    NavigationEvent event = events[i];
    if (event.direction.isVertical()) {
      graphController.moveCursorVertically(event.direction);
    } else {
      assert(event.direction.isHorizontal());
      graphController.moveCursorHorizontally(event.direction);
    }
    quiz_assert(event.expectedSelectedDot == selectedDotIndex);
    quiz_assert(event.expectedSelectedSeries == selectedSeriesIndex);
  }
}

QUIZ_CASE(regression_navigation_1) {
  constexpr int numberOfPoints0 = 4;
  double x0[numberOfPoints0] = {2.0, 3.0, 2.0, 1.0};
  double y0[numberOfPoints0] = {0.0, 1.0, 0.5, 5.0};

  constexpr int numberOfPoints1 = 3;
  double x1[numberOfPoints1] = {1.0, 10.0, 2.0};
  double y1[numberOfPoints1] = {0.0, 25.0, 0.5};

  constexpr int numberOfEvents = 7;
  NavigationEvent events[numberOfEvents] = {
    NavigationEvent(OMG::Direction::Down(), 0, -1),
    NavigationEvent(OMG::Direction::Down(), 0, 2),
    NavigationEvent(OMG::Direction::Down(), 1, 2),
    NavigationEvent(OMG::Direction::Down(), 0, 0),
    NavigationEvent(OMG::Direction::Down(), 1, 0),
    NavigationEvent(OMG::Direction::Down(), 1, -1),
    NavigationEvent(OMG::Direction::Down(), 1, -1)
  };
  assert_navigation_is(numberOfEvents, events, numberOfPoints0, 0, x0, y0, numberOfPoints0, x1, y1, numberOfPoints1);
}

QUIZ_CASE(regression_navigation_2) {
  constexpr int numberOfPoints0 = 4;
  double x0[numberOfPoints0] = {8.0, 9.0, 10.0, 10.0};
  double y0[numberOfPoints0] = {2.0, 5.0, 1.0, 2.0};

  constexpr int numberOfPoints1 = 3;
  double x1[numberOfPoints1] = {10.0, 12.0, 5.0};
  double y1[numberOfPoints1] = {2.0, 0.0, 8.0};

  constexpr int numberOfEvents = 6;
  NavigationEvent events[numberOfEvents] = {
    /* FIXME
     * Because of double computation error, the regression curve of the series 0
     * is above its mean point.
    NavigationEvent(OMG::Direction::Down, 1, -1),
     * */
    NavigationEvent(OMG::Direction::Down(), 0, -1),
    NavigationEvent(OMG::Direction::Down(), 0, numberOfPoints0),
    NavigationEvent(OMG::Direction::Down(), 0, -1),
    NavigationEvent(OMG::Direction::Down(), 0, 3),
    NavigationEvent(OMG::Direction::Down(), 1, 0),
    NavigationEvent(OMG::Direction::Down(), 0, 2)
  };
  assert_navigation_is(numberOfEvents, events, numberOfPoints1, 1, x0, y0, numberOfPoints0, x1, y1, numberOfPoints1);
}

QUIZ_CASE(regression_navigation_3) {
  constexpr int numberOfPoints = 4;
  double x[numberOfPoints] = {1.0, 2.0, 3.0, 4.0};
  double y[numberOfPoints] = {2.0, 2.0, 2.0, 2.0};

  constexpr int numberOfEvents = 3;
  NavigationEvent events[numberOfEvents] = {
    NavigationEvent(OMG::Direction::Down(), 0, -1),
    NavigationEvent(OMG::Direction::Down(), 1, -1),
    NavigationEvent(OMG::Direction::Down(), 2, -1),
  };
  assert_navigation_is(numberOfEvents, events, 2, 0, x, y, numberOfPoints, x, y, numberOfPoints, x, y, numberOfPoints);
}
