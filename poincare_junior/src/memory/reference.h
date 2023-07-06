#ifndef POINCARE_MEMORY_CACHE_H
#define POINCARE_MEMORY_CACHE_H

#include "tree.h"

namespace PoincareJ {

class SharedPointer {
 public:
#if ASSERTIONS
  SharedPointer(const void *data = nullptr, size_t dataSize = 0);
#else
  SharedPointer(const void *data = nullptr);
#endif

  const void *data() const {
#if ASSERTIONS
    assert(checksum(m_data, m_size) == m_checksum);
#endif
    return m_data;
  }

 private:
  const void *m_data;
#if ASSERTIONS
  uint32_t checksum(const void *data, size_t dataSize) const;
  uint32_t m_checksum;
  size_t m_size;
#endif
};

class TypeTreeBlock;

typedef void (*ActionWithContext)(void *context, const void *data);

class Reference {
 public:
  // Reference constructor without initializers
  Reference();
  // Reference from a const tree.
  Reference(const Tree *tree);

  // These Initializers must push one tree on the EditionPool
  typedef void (*Initializer)();
  typedef void (*InitializerFromString)(const char *);
  Reference(Initializer initializer);
  Reference(InitializerFromString initializer, const char *string);

  // This initializer can edit the given EditionPool node inplace
  typedef void (*InitializerFromTreeInplace)(Tree *);
  Reference(InitializerFromTreeInplace initializer, const Tree *tree);
  Reference(InitializerFromTreeInplace initializer,
            const Reference *treeReference);

  typedef void (*FunctionOnConstTree)(const Tree *tree, void *context);
  void send(FunctionOnConstTree functionOnTree, void *context) const;

  void dumpAt(void *address) const;
  size_t treeSize() const;
  bool treeIsIdenticalTo(const Reference &other) const {
    // TODO: second getTree() can delete first tree.
    return (isUninitialized() == other.isUninitialized()) &&
           (isUninitialized() || getTree()->treeIsIdenticalTo(other.getTree()));
  }
#if POINCARE_MEMORY_TREE_LOG
  void log();
#endif

  bool isCacheReference() const { return m_initializer != nullptr; }
  // Return true if Reference has no initializers or if tree is uninitialized.
  bool isUninitialized() const { return !hasInitializers() || !getTree(); }
  uint16_t id() const;  // TODO: make private (public for tests)

 private:
  Reference(ActionWithContext initializer, void *subInitializer,
            const void *data
#if ASSERTIONS
            ,
            size_t dataSize
#endif
  );

  // Return true if Reference has neither data nor initializers to build a tree.
  bool hasInitializers() const {
    return isCacheReference() || m_data.data() != nullptr;
  }
  const Tree *getTree() const;

  ActionWithContext m_initializer;
  void *m_subInitializer;
  SharedPointer m_data;
  // m_id must be the last member so that we can checksum non-mutable members
  mutable uint16_t m_id;
};

}  // namespace PoincareJ

#endif
