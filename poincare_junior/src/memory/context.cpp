#include "context.h"

#include <ion/storage/file_system.h>
#include <poincare_junior/src/expression/symbol.h>

namespace PoincareJ {

bool Context::DeepReplaceIdentifiersWithTrees(Tree* tree) {
  bool changed = false;
  if (tree->type() == BlockType::UserSymbol) {
    const Tree* replacement = TreeForIdentifier(
        Symbol::NonNullTerminatedName(tree), Symbol::Length(tree));
    if (replacement) {
      // TODO: Prevent infinite replacement loops in case of cycles.
      tree->cloneTreeOverTree(replacement);
      changed = true;
    }
  }
  Tree* child = tree->nextNode();
  for (int i = 0; i < tree->numberOfChildren(); i++) {
    changed = DeepReplaceIdentifiersWithTrees(child) || changed;
    child = child->nextTree();
  }
  return changed;
}

const Tree* Context::TreeForIdentifier(const char* identifier,
                                       size_t identifierLength) {
  // TODO: Use privateRecordBasedNamedWithExtensions directly.
  assert(identifierLength < 10);
  char buffer[10];
  memcpy(buffer, identifier, identifierLength);
  buffer[identifierLength] = 0;
  return TreeForIdentifier(buffer);
}

const Tree* Context::TreeForIdentifier(const char* identifier) {
  Ion::Storage::Record r =
      Ion::Storage::FileSystem::sharedFileSystem->recordBaseNamedWithExtensions(
          identifier, k_extensions, k_numberOfExtensions);
  return Tree::FromBlocks(static_cast<const TypeBlock*>(r.value().buffer));
}

bool Context::SetTreeForIdentifier(const Tree* tree, const char* identifier) {
  Ion::Storage::Record::ErrorStatus error =
      Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
          identifier, pcjExtension, tree->block(), tree->treeSize());
  return error == Ion::Storage::Record::ErrorStatus::None;
}

void Context::DeleteTreeForIdentifier(const char* identifier) {
  Ion::Storage::Record r =
      Ion::Storage::FileSystem::sharedFileSystem->recordBaseNamedWithExtensions(
          identifier, k_extensions, k_numberOfExtensions);
  r.destroy();
}

}  // namespace PoincareJ
