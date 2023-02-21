#ifndef ION_AUTHENTICATION_H
#define ION_AUTHENTICATION_H

#include <stdint.h>

namespace Ion {
namespace Authentication {

enum class ClearanceLevel {
  NumWorks = 0,
  NumWorksAndThirdPartyApps,
  ThirdParty
};

ClearanceLevel clearanceLevel();

}  // namespace Authentication
}  // namespace Ion

#endif
