#ifndef SHARED_RECORD_RESTRICTIVE_EXTENSIONS_HELPER_H
#define SHARED_RECORD_RESTRICTIVE_EXTENSIONS_HELPER_H

#include <ion/storage/file_system.h>

namespace Shared {

namespace RecordRestrictiveExtensions {

struct RestrictiveExtension {
  const char * extension;
  int precedenceScore;
};

constexpr static RestrictiveExtension k_restrictiveExtensions[] = {
  {Ion::Storage::regExtension, 1},
  {Ion::Storage::funcExtension, 2},
  {Ion::Storage::seqExtension, 2},
  {Ion::Storage::expExtension, 3},
  {Ion::Storage::lisExtension, 3},
  {Ion::Storage::matExtension, 3},
};
constexpr static int k_numberOfRestrictiveExtensions = sizeof(k_restrictiveExtensions) / sizeof(RestrictiveExtension);

void registerRestrictiveExtensionsToSharedStorage() {
  Ion::Storage::RecordNameVerifier * recordNameVerifier = Ion::Storage::FileSystem::sharedFileSystem->recordNameVerifier();
  for (int i = 0; i < k_numberOfRestrictiveExtensions; i++) {
    recordNameVerifier->registerRestrictiveExtensionWithPrecedence(k_restrictiveExtensions[i].extension, k_restrictiveExtensions[i].precedenceScore);
  }
}

}

}

#endif
