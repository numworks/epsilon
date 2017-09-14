#ifndef APPS_CONTAINER_STORAGE_H
#define APPS_CONTAINER_STORAGE_H

#include "apps_container.h"
#include "graph/app.h"
#include "probability/app.h"
#include "calculation/app.h"
#include "regression/app.h"
#include "sequence/app.h"
#include "settings/app.h"
#include "statistics/app.h"
#include "code/app.h"

class AppsContainerStorage : public AppsContainer {
public:
  AppsContainerStorage();
  int numberOfApps() override;
  App::Snapshot * appSnapshotAtIndex(int index) override;
private:
  static constexpr int k_numberOfCommonApps = 9;
  static constexpr int k_totalNumberOfApps = 2+k_numberOfCommonApps;
  Calculation::App::Snapshot m_calculationSnapshot;
  Graph::App::Snapshot m_graphSnapshot;
  Sequence::App::Snapshot m_sequenceSnapshot;
  Settings::App::Snapshot m_settingsSnapshot;
  Statistics::App::Snapshot m_statisticsSnapshot;
  Probability::App::Snapshot m_probabilitySnapshot;
  Regression::App::Snapshot m_regressionSnapshot;
  Code::App::Snapshot m_codeSnapshot;
};

#endif
