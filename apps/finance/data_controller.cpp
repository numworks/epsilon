#include "app.h"

using namespace Finance;

InterestData * DataController::interestData() const {
  return App::app()->snapshot()->data()->interestData();
}

void DataController::setModel(bool selectedModel) {
  App::app()->snapshot()->data()->setModel(selectedModel);
}
