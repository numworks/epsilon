#ifndef STATISTICS_MULTIPLE_DATA_VIEW_CONTROLLER_H
#define STATISTICS_MULTIPLE_DATA_VIEW_CONTROLLER_H

#include "data_view_controller.h"
#include "multiple_data_view.h"

namespace Statistics {

class MultipleDataViewController : public DataViewController {

public:
  using DataViewController::DataViewController;
  virtual MultipleDataView * multipleDataView() = 0;

  // DataViewController
  DataView * dataView() override { return multipleDataView(); }
protected:
  /* Warning: If this method is ever overridden, there are multiple store
   * methods used in the derived classes as well as in views and ranges that
   * should be re-implemented to use MultipleDataViewController::seriesIsValid
   * instead of Store::seriesIsValid. For example : maxValueForAllSeries,
   * numberOfValidSeries, or any other store method using seriesIsValid. */
  bool seriesIsValid(int series) const override final { return m_store->seriesIsValid(series); }
};

}


#endif
