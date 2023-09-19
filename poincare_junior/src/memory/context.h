#ifndef POINCARE_JUNIOR_CONTEXT_H
#define POINCARE_JUNIOR_CONTEXT_H

#include "tree.h"

namespace PoincareJ {

constexpr static char pcjExtension[] = "pcj";

class Context {
 public:
  constexpr static char pcjExtension[] = "pcj";
  constexpr static const char* k_extensions[] = {pcjExtension};
  constexpr static int k_numberOfExtensions = std::size(k_extensions);

  static bool DeepReplaceIdentifiersWithTrees(Tree* tree);
  static const Tree* TreeForIdentifier(const char* identifier,
                                       size_t identifierLength);
  static const Tree* TreeForIdentifier(const char* identifier);
  static bool SetTreeForIdentifier(const Tree* tree, const char* identifier);
  static void DeleteTreeForIdentifier(const char* identifier);
};

}  // namespace PoincareJ

#endif
