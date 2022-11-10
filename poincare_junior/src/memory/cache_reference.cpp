#include "cache_reference.h"
#include "cache_pool.h"
#include <ion.h>

namespace Poincare {

// SharedPointer

#if ASSERTIONS

SharedPointer::SharedPointer(const void * data, size_t dataSize) :
  m_data(data),
  m_size(dataSize)
{
  m_checksum = checksum(data, dataSize);
}

uint32_t SharedPointer::checksum(const void * data, size_t dataSize) const {
  return Ion::crc32Byte(static_cast<const uint8_t *>(data), dataSize);
}

#endif

// CacheReference

CacheReference::CacheReference(Initializer initializer) :
  CacheReference(
    [](void * initializer, const void * data) {
      return reinterpret_cast<Initializer>(initializer)();
    },
    reinterpret_cast<void *>(initializer),
    nullptr
#if ASSERTIONS
    , 0
#endif
  )
{}

CacheReference::CacheReference(InitializerFromTree initializer, const void * treeAddress) :
  CacheReference(
    [](void * initializer, const void * data) {
      Node editedTree = EditionPool::sharedEditionPool()->initFromAddress(data);
      return (reinterpret_cast<InitializerFromTree>(initializer))(editedTree);
    },
    reinterpret_cast<void *>(initializer),
    treeAddress
#if ASSERTIONS
    , Node(static_cast<const TypeBlock *>(treeAddress)).treeSize()
#endif
  )
{
  // Do something
}

CacheReference::CacheReference(InitializerFromTree initializer, const CacheReference * tree) :
  CacheReference(
    [](void * initializer, const void * data) {
      const CacheReference * treeReference = static_cast<const CacheReference *>(data);
      return treeReference->send(
          [](const Node tree, void * context) {
            Node editedTree = EditionPool::sharedEditionPool()->initFromTree(tree);
            return (reinterpret_cast<InitializerFromTree>(context))(editedTree);
          },
          initializer);
    },
    reinterpret_cast<void *>(initializer),
    tree
#if ASSERTIONS
 , tree->treeSize()
#endif
  )
{}

CacheReference::CacheReference(InitializerFromString initializer, const char * string) :
  CacheReference(
    [](void * initializer, const void * data) {
      return (reinterpret_cast<InitializerFromString>(initializer))(static_cast<const char *>(data));
    },
    reinterpret_cast<void *>(initializer),
    string
#if ASSERTIONS
    , strlen(string) + 1
#endif
  )
{}

void CacheReference::send(FunctionOnConstTree function, void * context) const {
  const Node tree = CachePool::sharedCachePool()->nodeForIdentifier(id());
  return function(tree, context);
}

void CacheReference::dumpAt(void * address) {
  send(
    [](const Node tree, void * buffer) {
      memcpy(buffer, tree.block(), tree.treeSize());
    },
    address
  );
}

size_t CacheReference::treeSize() const {
  size_t result;
  send(
    [](const Node tree, void * result) {
      size_t * res = static_cast<size_t *>(result);
      *res = tree.treeSize();
    },
    &result);
  return result;
}

#if POINCARE_MEMORY_TREE_LOG
void CacheReference::log() {
  std::cout << "id: " << m_id;
  send(
     [](const Node tree, void * result) {
        tree.log(std::cout);
      },
      nullptr
    );
}
#endif

int CacheReference::id() const {
  CachePool * cache = CachePool::sharedCachePool();
  const Node tree = cache->nodeForIdentifier(m_id);
  if (tree.isUninitialized()) {
    m_id = cache->execute(m_initializer, m_subInitializer, m_data.data());
  }
  return m_id;
}

}
