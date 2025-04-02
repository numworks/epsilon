#include <poincare/pool.h>
#include <poincare/pool_handle.h>
#include <poincare/pool_object.h>

namespace Poincare {

// Object operations

void PoolObject::release() {
  if (!isAfterTopmostCheckpoint()) {
    /* Do not decrease reference counters outside of the current checkpoint
     * since they were not increased. */
    return;
  }
  m_referenceCounter--;
  if (m_referenceCounter == 0) {
    Pool::sharedPool->discardPoolObject(this);
  }
}

void PoolObject::rename(uint16_t identifier,
                        bool unregisterPreviousIdentifier) {
  if (unregisterPreviousIdentifier) {
    /* The previous identifier should not always be unregistered. For instance,
     * if the object is a clone and still has the original object's identifier,
     * unregistering it would lose the access to the original object. */
    Pool::sharedPool->unregisterObject(this);
  }
  m_identifier = identifier;
  m_referenceCounter = 0;
  Pool::sharedPool->registerObject(this);
}

// Protected

#if POINCARE_TREE_LOG
void PoolObject::log(std::ostream& stream, int indentation, bool verbose) {
  stream << "\n";
  for (int i = 0; i < indentation; ++i) {
    stream << "  ";
  }
  stream << "<";
  logObjectName(stream);
  if (verbose) {
    stream << " id=\"" << m_identifier << "\"";
    stream << " refCount=\"" << (int16_t)m_referenceCounter << "\"";
    stream << " size=\"" << size() << "\"";
  }
  logAttributes(stream);
  bool tagIsClosed = false;
  if (tagIsClosed) {
    stream << "\n";
    for (int i = 0; i < indentation; ++i) {
      stream << "  ";
    }
    stream << "</";
    logObjectName(stream);
    stream << ">";
  } else {
    stream << "/>";
  }
}
#endif

}  // namespace Poincare
