#include <escher/view_controller.h>
#include <escher/container.h>
#include <string.h>
#include <ion/telemetry.h>

void ViewController::viewWillAppear() {
#if EPSILON_TELEMETRY
  const char * screenId = telemetryId();
  if (screenId == nullptr) {
    return;
  }

  const char * appId = Container::activeApp()->telemetryId();
  if (appId == nullptr) {
    return;
  }

  char reportedName[64];
  assert(strlen(appId) + 1 + strlen(screenId) < sizeof(reportedName));

  reportedName[0] = 0;
  strlcat(reportedName, appId, sizeof(reportedName));
  if (strlen(screenId) > 0) {
    strlcat(reportedName, ".", sizeof(reportedName));
    strlcat(reportedName, screenId, sizeof(reportedName));
  }

  Ion::Telemetry::reportScreen(reportedName);
#endif
}

#if EPSILON_TELEMETRY
void ViewController::telemetryReportEvent(const char * action, const char * label) const {
  const char * category = Container::activeApp()->telemetryId();
  assert(category != nullptr);
  assert(action != nullptr);
  assert(label != nullptr);
  Ion::Telemetry::reportEvent(category, action, label);
}
#endif
