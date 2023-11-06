#include "storage_context.h"

#include <ion/storage/file_system.h>
#include <poincare_junior/src/expression/symbol.h>

namespace PoincareJ {

bool StorageContext::DeepReplaceIdentifiersWithTrees(Tree* tree) {
  bool changed = false;
  if (tree->isUserSymbol()) {
    const Tree* replacement = TreeForIdentifier(
        Symbol::NonNullTerminatedName(tree), Symbol::Length(tree));
    if (replacement) {
      tree->cloneTreeOverTree(replacement);
      // Nested UserSymbol shouldn't exist and are therefore not replaced.
      assert(!DeepReplaceIdentifiersWithTrees(tree));
      return true;
    }
    return false;
  }
  for (Tree* child : tree->children()) {
    changed = DeepReplaceIdentifiersWithTrees(child) || changed;
  }
  return changed;
}

const Tree* StorageContext::TreeForIdentifier(const char* identifier,
                                              size_t identifierLength) {
  // TODO: Use privateRecordBasedNamedWithExtensions directly.
  assert(identifierLength < 10);
  char buffer[10];
  memcpy(buffer, identifier, identifierLength);
  buffer[identifierLength] = 0;
  return TreeForIdentifier(buffer);
}

const Tree* StorageContext::TreeForIdentifier(const char* identifier) {
  Ion::Storage::Record r =
      Ion::Storage::FileSystem::sharedFileSystem->recordBaseNamedWithExtensions(
          identifier, k_extensions, k_numberOfExtensions);
  return Tree::FromBlocks(static_cast<const TypeBlock*>(r.value().buffer));
}

bool StorageContext::SetTreeForIdentifier(const Tree* tree,
                                          const char* identifier) {
  Ion::Storage::Record::ErrorStatus error =
      Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
          identifier, pcjExtension, tree->block(), tree->treeSize());
  return error == Ion::Storage::Record::ErrorStatus::None;
}

void StorageContext::DeleteTreeForIdentifier(const char* identifier) {
  Ion::Storage::Record r =
      Ion::Storage::FileSystem::sharedFileSystem->recordBaseNamedWithExtensions(
          identifier, k_extensions, k_numberOfExtensions);
  r.destroy();
}

}  // namespace PoincareJ
