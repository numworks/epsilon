#include "cache_reference.h"
#include "cache_pool.h"

namespace Poincare {

CacheReference::CacheReference(Initializer initializer) :
  CacheReference(
    [](void * initializer, void * data) {
      return reinterpret_cast<Initializer>(initializer)();
    },
    reinterpret_cast<void *>(initializer),
    nullptr
  )
{}

CacheReference::CacheReference(InitializerFromTree initializer, void * treeAddress) :
  CacheReference(
    [](void * initializer, void * data) {
      Node editedTree = EditionPool::sharedEditionPool()->initFromAddress(data);
      return (reinterpret_cast<InitializerFromTree>(initializer))(editedTree);
    },
    reinterpret_cast<void *>(initializer),
    treeAddress
  )
{}

CacheReference::CacheReference(InitializerFromTree initializer, CacheReference * tree) :
  CacheReference(
    [](void * initializer, void * data) {
      CacheReference * treeReference = static_cast<CacheReference *>(data);
      return treeReference->send(
          [](const Node tree, void * context) {
            Node editedTree = EditionPool::sharedEditionPool()->initFromTree(tree);
            return (reinterpret_cast<InitializerFromTree>(context))(editedTree);
          },
          initializer);
    },
    reinterpret_cast<void *>(initializer),
    tree
  )
{}

CacheReference::CacheReference(InitializerFromString initializer, char * string) :
  CacheReference(
    [](void * initializer, void * data) {
      return (reinterpret_cast<InitializerFromString>(initializer))(static_cast<char *>(data));
    },
    reinterpret_cast<void *>(initializer),
    string
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

#if POINCARE_TREE_LOG
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
    m_id = cache->execute(m_initializer, m_subInitializer, m_data);
  }
  return m_id;
}

}
