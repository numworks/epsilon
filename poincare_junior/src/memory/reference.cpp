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
  assert(!CachePool::SharedCachePool->mayContain(
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

Reference::Reference(const Tree *tree)
    : Reference(nullptr, nullptr, tree->block()
#if ASSERTIONS
                                      ,
                tree->treeSize()
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

Reference::Reference(InitializerFromData initializer, const void *data,
                     size_t dataSize)
    : Reference(
          [](void *initializer, const void *data) {
            return (reinterpret_cast<InitializerFromData>(initializer))(
                static_cast<const char *>(data));
          },
          reinterpret_cast<void *>(initializer), data
#if ASSERTIONS
          ,
          dataSize
#endif
      ) {
}

Reference::Reference(InitializerFromTreeInplace initializer, const Tree *tree)
    : Reference(
          [](void *initializer, const void *data) {
            Tree *editedTree = SharedEditionPool->initFromAddress(data);
            return (reinterpret_cast<InitializerFromTreeInplace>(initializer))(
                editedTree);
          },
          reinterpret_cast<void *>(initializer), tree->block()
#if ASSERTIONS
                                                     ,
          tree->treeSize()
#endif
      ) {
}

Reference::Reference(InitializerFromTreeInplace initializer,
                     const Reference *treeReference)
    : Reference(
          [](void *initializer, const void *data) {
            const Reference *treeReference =
                static_cast<const Reference *>(data);
            /* Copy the cached Tree* into the EditionPool for inplace editing.
             * We couldn't use tree in the initializer since it may be erased if
             * the SharedEditionPool needs space and flushes the CachePool. */
            return reinterpret_cast<InitializerFromTreeInplace>(initializer)(
                treeReference->getTree()->clone());
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
  assert(hasInitializers());
  return function(getTree(), context);
}

#if POINCARE_TREE_LOG
void Reference::log() {
  std::cout << "id: " << m_id;
  getTree()->log(std::cout);
}
#endif

uint16_t Reference::id() const {
  assert(isCacheReference());
  const Tree *tree = CachePool::SharedCachePool->nodeForIdentifier(m_id);
  if (!tree) {
    /* TODO: executeAndCache handle Relax operations which are not used in
     * References. This could be a Reference property like the initializer and
     * be passed here, which could be useful for simplification strategies. */
    m_id = SharedEditionPool->executeAndCache(m_initializer, m_subInitializer,
                                              m_data.data());
  }
  return m_id;
}

const Tree *Reference::getTree() const {
  assert(hasInitializers());
  return isCacheReference()
             ? CachePool::SharedCachePool->nodeForIdentifier(id())
             : Tree::FromBlocks(
                   reinterpret_cast<const TypeBlock *>(m_data.data()));
}

Reference::Reference(ActionWithContext initializer, void *subInitializer,
                     const void *data
#if ASSERTIONS
                     ,
                     size_t dataSize
#endif
                     )
    :  // TODO: maybe add a checksum if the m_id has potentially been
       // reallocated to another tree
      m_initializer(initializer),
      m_subInitializer(subInitializer),
#if ASSERTIONS
      m_data(data, dataSize),
#else
      m_data(data),
#endif
      m_id(CachePool::ReferenceTable::NoNodeIdentifier) {
}

}  // namespace PoincareJ
