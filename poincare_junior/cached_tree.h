#ifndef POINCARE_CACHED_TREE_H
#define POINCARE_CACHED_TREE_H

namespace Poincare {

class TypeTreeBlock;

typedef bool (*ActionWithContext)(void * subAction, void * data);

class CachedTree {
public:
  CachedTree() : m_id(-1) {}
  typedef bool (*Initializer)();
  CachedTree(Initializer initializer);

  typedef bool (*InitializerFromTree)(TypeTreeBlock *);
  CachedTree(InitializerFromTree initializer, TypeTreeBlock * tree);
  CachedTree(InitializerFromTree initializer, CachedTree * tree);

  typedef bool (*InitializerFromString)(char *);
  CachedTree(InitializerFromString initializer, char * string);

  typedef void (*FunctionOnConstTree)(const TypeTreeBlock * tree, void * resultAddress);
  void send(FunctionOnConstTree functionOnTree, void * resultAddress);

  void dumpAt(void * address);
  void log();

private:
  CachedTree(ActionWithContext initializer, void * subInitializer, void * data) :
    m_id(-1),
    m_initializer(initializer),
    m_subInitializer(subInitializer),
    m_data(data) {}
  int id();

  int m_id;
  ActionWithContext m_initializer;
  void * m_subInitializer;
  void * m_data;
};

}

#endif
