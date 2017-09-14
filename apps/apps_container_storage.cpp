#include "apps_container_storage.h"

AppsContainerStorage::AppsContainerStorage() :
  AppsContainer(),
  m_calculationSnapshot(),
  m_graphSnapshot(),
  m_sequenceSnapshot(),
  m_settingsSnapshot(),
  m_statisticsSnapshot(),
  m_probabilitySnapshot(),
  m_regressionSnapshot(),
  m_codeSnapshot()
{
}

int AppsContainerStorage::numberOfApps() {
  return k_numberOfCommonApps;
}

App::Snapshot * AppsContainerStorage::appSnapshotAtIndex(int index) {
  if (index < 0) {
    return nullptr;
  }
  App::Snapshot * snapshots[] = {
    homeAppSnapshot(),
    &m_calculationSnapshot,
    &m_graphSnapshot,
    &m_sequenceSnapshot,
    &m_settingsSnapshot,
    &m_statisticsSnapshot,
    &m_probabilitySnapshot,
    &m_regressionSnapshot,
    &m_codeSnapshot
  };
  assert(sizeof(snapshots)/sizeof(snapshots[0]) == k_numberOfCommonApps);
  assert(index >= 0 && index < k_numberOfCommonApps);
  return snapshots[index];
}
