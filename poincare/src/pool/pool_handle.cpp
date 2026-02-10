#include <omg/memory.h>
#include <poincare/pool_handle.h>
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

/* Clone */

PoolHandle PoolHandle::clone() const {
  assert(!isUninitialized());
  PoolObject* objectSource = object();
  PoolObject* objectCopy = Pool::sharedPool->copyObjectFromAddress(
      static_cast<void*>(objectSource), objectSource->size());
  return PoolHandle(objectCopy);
}

/* Hierarchy operations */
PoolObject* PoolHandle::object() const {
  assert(HasObject(m_identifier));
  return Pool::sharedPool->object(m_identifier);
}

size_t PoolHandle::size() const { return Pool::AlignedSize(object()->size()); }

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
  Pool::sharedPool->renamePoolObject(object);
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
  release();
  setIdentifierAndRetain(tr.identifier());
}

void PoolHandle::transferFrom(PoolHandle& tr) {
  setTo(tr);
  tr.release();
}

void PoolHandle::release() {
  if (!HasObject(m_identifier)) {
    return;
  }
  PoolObject* object = Pool::sharedPool->object(m_identifier);
  assert(!object || object->identifier() == m_identifier);
  m_identifier = PoolObject::NoObjectIdentifier;
  /* The object could be nullptr even if the identifier is valid: there could
   * have been an exception that deleted the pool. */
  if (object) {
    object->release();
  }
}

}  // namespace Poincare
