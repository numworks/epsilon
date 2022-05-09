#ifndef SHARED_RECORD_RESTRICTIVE_EXTENSIONS_HELPER_H
#define SHARED_RECORD_RESTRICTIVE_EXTENSIONS_HELPER_H

#include <ion/storage/container.h>

namespace Shared {

namespace RecordRestrictiveExtensions {

struct RestrictiveExtension {
  const char * extension;
  int precedenceScore;
};

constexpr static RestrictiveExtension k_restrictiveExtensions[] = {
  {Ion::Storage::funcExtension, 1},
  {Ion::Storage::seqExtension, 1},
  {Ion::Storage::expExtension, 2},
  {Ion::Storage::lisExtension, 2},
  {Ion::Storage::matExtension, 2}
};
constexpr static int k_numberOfRestrictiveExtensions = sizeof(k_restrictiveExtensions) / sizeof(RestrictiveExtension);

void registerRestrictiveExtensionsToSharedStorage() {
  Ion::Storage::RecordNameVerifier * recordNameVerifier = Ion::Storage::Container::sharedStorage()->recordNameVerifier();
  for (int i = 0; i < k_numberOfRestrictiveExtensions; i++) {
    recordNameVerifier->registerRestrictiveExtensionWithPrecedence(k_restrictiveExtensions[i].extension, k_restrictiveExtensions[i].precedenceScore);
  }
}

}

}

#endif
