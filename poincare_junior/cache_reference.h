#ifndef POINCARE_CACHE_REFERENCE_H
#define POINCARE_CACHE_REFERENCE_H

#include "node.h"

namespace Poincare {

class SharedPointer {
public:
#if ASSERTIONS
  SharedPointer(const void * data = nullptr, size_t dataSize = 0);
#else
  SharedPointer(const void * data = nullptr) : m_data(data) {}
#endif

  const void * data() const {
#if ASSERTIONS
    assert(checksum(m_data, m_size) == m_checksum);
#endif
    return m_data;
  }

private:
  const void * m_data;
#if ASSERTIONS
  uint32_t checksum(const void * data, size_t dataSize) const;
  uint32_t m_checksum;
  size_t m_size;
#endif
};

class TypeTreeBlock;

typedef void (*ActionWithContext)(void * subAction, const void * data);

class CacheReference {
public:
  CacheReference() : m_id(-1) {}
  typedef void (*Initializer)();
  CacheReference(Initializer initializer);

  typedef void (*InitializerFromTree)(Node);
  CacheReference(InitializerFromTree initializer, const void * treeAddress);
  CacheReference(InitializerFromTree initializer, const CacheReference * treeReference);

  typedef void (*InitializerFromString)(const char *);
  CacheReference(InitializerFromString initializer, const char * string);

  // TODO: find a way not to build the tree in cache if it's just a copy from another tree pointed by data
  typedef void (*FunctionOnConstTree)(const Node tree, void * context);
  void send(FunctionOnConstTree functionOnTree, void * context) const;

  void dumpAt(void * address);
  size_t treeSize() const;
#if POINCARE_TREE_LOG
  void log();
#endif

private:
  CacheReference(ActionWithContext initializer, void * subInitializer, const void * data
#if ASSERTIONS
      , size_t dataSize
#endif
      ) :
    // TODO: maybe add a checksum if the m_id has potentially been reallocated to another tree
    m_id(-1),
    m_initializer(initializer),
    m_subInitializer(subInitializer),
#if ASSERTIONS
    m_data(data, dataSize)
#else
      m_data(data)
#endif
    {}

  int id() const;

  mutable int m_id;
  ActionWithContext m_initializer;
  void * m_subInitializer;
  SharedPointer m_data;
};

}

#endif
