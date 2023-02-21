#ifndef ION_TELEMETRY_H
#define ION_TELEMETRY_H

namespace Ion {
namespace Telemetry {

void reportScreen(const char* screenName);
void reportEvent(const char* category, const char* action, const char* label);

}  // namespace Telemetry
}  // namespace Ion

#endif
