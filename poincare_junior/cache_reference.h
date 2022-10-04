#ifndef POINCARE_CACHE_REFERENCE_H
#define POINCARE_CACHE_REFERENCE_H

#include "node.h"

namespace Poincare {

class TypeTreeBlock;

typedef void (*ActionWithContext)(void * subAction, void * data);

class CacheReference {
public:
  CacheReference() : m_id(-1) {}
  typedef void (*Initializer)();
  CacheReference(Initializer initializer);

  typedef void (*InitializerFromTree)(Node);
  CacheReference(InitializerFromTree initializer, void * treeAddress);
  CacheReference(InitializerFromTree initializer, CacheReference * treeReference);

  typedef void (*InitializerFromString)(char *);
  CacheReference(InitializerFromString initializer, char * string);

  typedef void (*FunctionOnConstTree)(const Node tree, void * context);
  void send(FunctionOnConstTree functionOnTree, void * context) const;

  void dumpAt(void * address);
#if POINCARE_TREE_LOG
  void log();
#endif

private:
  CacheReference(ActionWithContext initializer, void * subInitializer, void * data) :
    // TODO: maybe add a checksum if the m_id has potentially been reallocated to another tree
    m_id(-1),
    m_initializer(initializer),
    m_subInitializer(subInitializer),
    m_data(data) {}
  int id() const;

  mutable int m_id;
  ActionWithContext m_initializer;
  void * m_subInitializer;
  void * m_data;
};

}

#endif
