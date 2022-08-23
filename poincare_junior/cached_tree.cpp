#include "cached_tree.h"

namespace Poincare {

CachedTree::CachedTree(Initializer initializer) :
  CachedTree(
    [](void * subInitializer, void * data) {
      return reinterpret_cast<Initializer>(subInitializer)();
    },
    reinterpret_cast<void *>(initializer),
    nullptr
  )
{}

CachedTree::CachedTree(InitializerFromTree initializer, TypeTreeBlock * tree) :
  CachedTree(
    [](void * subInitializer, void * data) {
      TypeTreeBlock * treeOnSandbox = TreeSandbox::sharedSandbox()->copyTreeFromAddress(static_cast<TypeTreeBlock *>(data));
      return (reinterpret_cast<InitializerFromTree>(subInitializer))(treeOnSandbox);
    },
    reinterpret_cast<void *>(initializer),
    tree
  )
{}

CachedTree::CachedTree(InitializerFromTree initializer, CachedTree * tree) :
  CachedTree(
    [](void * subInitializer, void * data) {
      CachedTree * cachedTree = static_cast<CachedTree *>(data);
      const TypeTreeBlock * tree = TreeCache::sharedCache()->treeForIdentifier(cachedTree->id());
      assert(tree);
      TypeTreeBlock * treeOnSandbox = TreeSandbox::sharedSandbox()->copyTreeFromAddress(tree);
      return (reinterpret_cast<InitializerFromTree>(subInitializer))(treeOnSandbox);
    },
    reinterpret_cast<void *>(initializer),
    tree
  )
{}

CachedTree::CachedTree(InitializerFromString initializer, char * string) :
  CachedTree(
    [](void * subInitializer, void * data) {
      return (reinterpret_cast<InitializerFromString>(subInitializer))(static_cast<char *>(data));
    },
    reinterpret_cast<void *>(initializer),
    string
  )
{}

void CachedTree::send(FunctionOnConstTree function, void * resultAddress) {
  const TypeTreeBlock * tree = TreeCache::sharedCache()->treeForIdentifier(id());
  return function(tree, resultAddress);
}

int CachedTree::id() {
  TreeCache * cache = TreeCache::sharedCache();
  const TypeTreeBlock * tree = cache->treeForIdentifier(m_id);
  if (!tree) {
    m_id = cache->execute(m_initializer, m_subInitializer, m_data);
  }
  return m_id;
}

}
