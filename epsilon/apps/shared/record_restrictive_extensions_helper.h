#ifndef SHARED_RECORD_RESTRICTIVE_EXTENSIONS_HELPER_H
#define SHARED_RECORD_RESTRICTIVE_EXTENSIONS_HELPER_H

#include <ion/storage/file_system.h>

#include <array>

namespace Shared {

namespace RecordRestrictiveExtensions {

struct RestrictiveExtension {
  const char* extension;
  int precedenceScore;
};

constexpr static RestrictiveExtension k_restrictiveExtensions[] = {
    {Ion::Storage::regressionExtension, 1},
    {Ion::Storage::parametricComponentExtension, 1},
    {Ion::Storage::functionExtension, 2},
    {Ion::Storage::sequenceExtension, 2},
    {Ion::Storage::expressionExtension, 3},
    {Ion::Storage::listExtension, 3},
    {Ion::Storage::matrixExtension, 3},
};
constexpr static int k_numberOfRestrictiveExtensions =
    std::size(k_restrictiveExtensions);

void registerRestrictiveExtensionsToSharedStorage() {
  Ion::Storage::RecordNameVerifier* recordNameVerifier =
      Ion::Storage::FileSystem::sharedFileSystem->recordNameVerifier();
  for (int i = 0; i < k_numberOfRestrictiveExtensions; i++) {
    recordNameVerifier->registerRestrictiveExtensionWithPrecedence(
        k_restrictiveExtensions[i].extension,
        k_restrictiveExtensions[i].precedenceScore);
  }
}

}  // namespace RecordRestrictiveExtensions

}  // namespace Shared

#endif
