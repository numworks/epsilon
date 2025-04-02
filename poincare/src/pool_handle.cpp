#include <omg/memory.h>
#include <poincare/pool_handle.h>
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

/* Clone */

PoolHandle PoolHandle::clone() const {
  assert(!isUninitialized());
  PoolObject* objectCopy = Pool::sharedPool->deepCopy(object());
  return PoolHandle(objectCopy);
}

/* Hierarchy operations */
PoolObject* PoolHandle::object() const {
  assert(hasObject(m_identifier));
  return Pool::sharedPool->object(m_identifier);
}

size_t PoolHandle::size() const { return object()->deepSize(); }

#if POINCARE_TREE_LOG
void PoolHandle::log() const {
  if (!isUninitialized()) {
    return object()->log();
  }
  std::cout << "\n<Uninitialized PoolHandle/>" << std::endl;
}
#endif

/* Private */

PoolHandle::PoolHandle(const PoolObject* object) : PoolHandle() {
  if (object != nullptr) {
    setIdentifierAndRetain(object->identifier());
  }
}

template <class U>
PoolHandle PoolHandle::Builder() {
  void* bufferObject = Pool::sharedPool->alloc(sizeof(U));
  U* object = new (bufferObject) U();
  return PoolHandle::Build(object);
}

PoolHandle PoolHandle::Build(PoolObject* object) {
  assert(object != nullptr);
  uint16_t objectIdentifier = Pool::sharedPool->generateIdentifier();
  object->rename(objectIdentifier, false);
  return PoolHandle(object);
}

void PoolHandle::setIdentifierAndRetain(uint16_t newId) {
  m_identifier = newId;
  if (!isUninitialized()) {
    object()->retain();
  }
}

void PoolHandle::setTo(const PoolHandle& tr) {
  /* We cannot use (*this)==tr because tr would need to be casted to
   * PoolHandle, which calls setTo and triggers an infinite loop */
  if (identifier() == tr.identifier()) {
    return;
  }
  int currentId = identifier();
  setIdentifierAndRetain(tr.identifier());
  release(currentId);
}

void PoolHandle::release(uint16_t identifier) {
  if (!hasObject(identifier)) {
    return;
  }
  PoolObject* object = Pool::sharedPool->object(identifier);
  if (object == nullptr) {
    /* The identifier is valid, but not the object: there must have been an
     * exception that deleted the pool. */
    return;
  }
  assert(object->identifier() == identifier);
  object->release();
}

}  // namespace Poincare
