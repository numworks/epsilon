#include "context.h"

#include <ion/storage/file_system.h>

namespace PoincareJ {

const Tree* Context::TreeForIdentifier(const char* identifier) {
  Ion::Storage::Record r =
      Ion::Storage::FileSystem::sharedFileSystem->recordBaseNamedWithExtensions(
          identifier, k_extensions, k_numberOfExtensions);
  return Tree::FromBlocks(static_cast<const TypeBlock*>(r.value().buffer));
}

bool Context::SetTreeForIdentifier(const Tree* node, const char* identifier) {
  Ion::Storage::Record::ErrorStatus error =
      Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
          identifier, pcjExtension, node->block(), node->treeSize());
  return error == Ion::Storage::Record::ErrorStatus::None;
}

void Context::DeleteTreeForIdentifier(const char* identifier) {
  Ion::Storage::Record r =
      Ion::Storage::FileSystem::sharedFileSystem->recordBaseNamedWithExtensions(
          identifier, k_extensions, k_numberOfExtensions);
  r.destroy();
}

}  // namespace PoincareJ
