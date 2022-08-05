#ifndef POINCARE_CACHED_TREE_H
#define POINCARE_CACHED_TREE_H

#include "tree_block.h"
#include "tree_cache.h"

namespace Poincare {

class CachedTree {
public:
  typedef bool (*Initializer)();
  CachedTree(Initializer initializer);

  typedef bool (*InitializerFromTree)(TypeTreeBlock *);
  CachedTree(InitializerFromTree initializer, TypeTreeBlock * tree);
  CachedTree(InitializerFromTree initializer, CachedTree * tree);

  typedef bool (*InitializerFromString)(char *);
  CachedTree(InitializerFromString initializer, char * string);

  typedef void (*FunctionOnConstTree)(TypeTreeBlock * tree, void * resultAddress);
  void send(FunctionOnConstTree functionOnTree, void * resultAddress);

private:
  CachedTree(TreeCache::ActionWithContext initializer, void * subInitializer, void * data) :
    m_id(-1),
    m_initializer(initializer),
    m_subInitializer(subInitializer),
    m_data(data) {}
  int id();

  int m_id;
  TreeCache::ActionWithContext m_initializer;
  void * m_subInitializer;
  void * m_data;
};

}

#endif
