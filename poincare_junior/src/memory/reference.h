#ifndef POINCARE_MEMORY_CACHE_H
#define POINCARE_MEMORY_CACHE_H

#include "node.h"

namespace PoincareJ {

class SharedPointer {
public:
#if ASSERTIONS
  SharedPointer(const void * data = nullptr, size_t dataSize = 0);
#else
  SharedPointer(const void * data = nullptr);
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

class Reference {
public:
  // Uninitialized Reference constructor
  Reference();
  // Reference from a const tree.
  Reference(const Node tree);

  // These Initializers must push one tree on the EditionPool
  typedef void (*Initializer)();
  typedef void (*InitializerFromString)(const char *);
  Reference(Initializer initializer);
  Reference(InitializerFromString initializer, const char * string);

  // This initializer can edit the given EditionPool node inplace
  typedef void (*InitializerFromTreeInplace)(Node);
  Reference(InitializerFromTreeInplace initializer, const Node tree);
  Reference(InitializerFromTreeInplace initializer, const Reference * treeReference);

  typedef void (*FunctionOnConstTree)(const Node tree, void * context);
  void send(FunctionOnConstTree functionOnTree, void * context) const;

  void dumpAt(void * address) const;
  size_t treeSize() const;
  bool treeIsIdenticalTo(const Reference &other) const {
    return getTree().treeIsIdenticalTo(other.getTree());
  }
#if POINCARE_MEMORY_TREE_LOG
  void log();
#endif

  bool isCacheReference() const { return m_initializer != nullptr; }
  bool isInitialized() const { return isCacheReference() || m_data.data() != nullptr; }
  int id() const; // TODO: make private (public for tests)

private:
  Reference(ActionWithContext initializer, void * subInitializer, const void * data
#if ASSERTIONS
      , size_t dataSize
#endif
      ) :
    // TODO: maybe add a checksum if the m_id has potentially been reallocated to another tree
    m_initializer(initializer),
    m_subInitializer(subInitializer),
#if ASSERTIONS
    m_data(data, dataSize),
#else
    m_data(data),
#endif
    m_id(-1)
  {}

  const Node getTree() const;

  ActionWithContext m_initializer;
  void * m_subInitializer;
  SharedPointer m_data;
  // m_id must be the last member so that we can checksum non-mutable members
  mutable int m_id;
};

}

#endif
