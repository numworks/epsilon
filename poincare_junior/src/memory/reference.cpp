#include "reference.h"

#include <ion.h>

#include "cache_pool.h"
#include "stddef.h"

namespace PoincareJ {

// SharedPointer

SharedPointer::SharedPointer(const void *data
#if ASSERTIONS
                             ,
                             size_t dataSize
#endif
                             )
    : m_data(data)
#if ASSERTIONS
      ,
      m_size(dataSize)
#endif
{
#if ASSERTIONS
  m_checksum = checksum(data, dataSize);
#endif
  // Reference data cannot live in the CachePool
  assert(!CachePool::sharedCachePool()->mayContain(
      static_cast<const TypeBlock *>(m_data)));
}

#if ASSERTIONS

uint32_t SharedPointer::checksum(const void *data, size_t dataSize) const {
  return Ion::crc32Byte(static_cast<const uint8_t *>(data), dataSize);
}

#endif

// Reference

Reference::Reference()
    : Reference(nullptr, nullptr, nullptr
#if ASSERTIONS
                ,
                0
#endif
      ) {
}

Reference::Reference(const Node tree)
    : Reference(nullptr, nullptr, tree.block()
#if ASSERTIONS
                                      ,
                tree.treeSize()
#endif
      ) {
}

Reference::Reference(Initializer initializer)
    : Reference(
          [](void *initializer, const void *data) {
            return reinterpret_cast<Initializer>(initializer)();
          },
          reinterpret_cast<void *>(initializer), nullptr
#if ASSERTIONS
          ,
          0
#endif
      ) {
}

Reference::Reference(InitializerFromString initializer, const char *string)
    : Reference(
          [](void *initializer, const void *data) {
            return (reinterpret_cast<InitializerFromString>(initializer))(
                static_cast<const char *>(data));
          },
          reinterpret_cast<void *>(initializer), string
#if ASSERTIONS
          ,
          strlen(string) + 1
#endif
      ) {
}

Reference::Reference(InitializerFromTreeInplace initializer, const Node tree)
    : Reference(
          [](void *initializer, const void *data) {
            Node editedTree =
                EditionPool::sharedEditionPool()->initFromAddress(data);
            return (reinterpret_cast<InitializerFromTreeInplace>(initializer))(
                editedTree);
          },
          reinterpret_cast<void *>(initializer), tree.block()
#if ASSERTIONS
                                                     ,
          tree.treeSize()
#endif
      ) {
}

Reference::Reference(InitializerFromTreeInplace initializer,
                     const Reference *treeReference)
    : Reference(
          [](void *initializer, const void *data) {
            const Reference *treeReference =
                static_cast<const Reference *>(data);
            return treeReference->send(
                [](const Node tree, void *context) {
                  /* Copy the cache Node into the EditionPool for inplace
                   * editing. We couldn't use tree in the initializer since it
                   * may be erased if the editionPool needs space and flushes
                   * the CachePool. */
                  Node editedTree =
                      EditionPool::sharedEditionPool()->clone(tree);
                  return (reinterpret_cast<InitializerFromTreeInplace>(
                      context))(editedTree);
                },
                initializer);
          },
          reinterpret_cast<void *>(initializer),
          treeReference
#if ASSERTIONS
          // Only checksum the non-mutable members of Reference
          ,
          offsetof(Reference, m_id)
#endif
      ) {
  static_assert(
      offsetof(Reference, m_id) == sizeof(Reference) - alignof(Reference),
      "Reference::m_id must be the last member.");
}

void Reference::send(FunctionOnConstTree function, void *context) const {
  assert(isInitialized());
  const Node tree = getTree();
  return function(tree, context);
}

void Reference::dumpAt(void *address) const {
  send([](const Node tree, void *buffer) { tree.copyTreeTo(buffer); }, address);
}

size_t Reference::treeSize() const {
  size_t result;
  send(
      [](const Node tree, void *result) {
        size_t *res = static_cast<size_t *>(result);
        *res = tree.treeSize();
      },
      &result);
  return result;
}

#if POINCARE_MEMORY_TREE_LOG
void Reference::log() {
  std::cout << "id: " << m_id;
  send([](const Node tree, void *result) { tree.log(std::cout); }, nullptr);
}
#endif

uint16_t Reference::id() const {
  assert(isCacheReference());
  const Node tree = CachePool::sharedCachePool()->nodeForIdentifier(m_id);
  if (tree.isUninitialized()) {
    m_id = EditionPool::sharedEditionPool()->executeAndCache(
        m_initializer, m_subInitializer, m_data.data());
  }
  return m_id;
}

const Node Reference::getTree() const {
  assert(isInitialized());
  return isCacheReference()
             ? CachePool::sharedCachePool()->nodeForIdentifier(id())
             : Node(reinterpret_cast<const TypeBlock *>(m_data.data()));
}

}  // namespace PoincareJ
