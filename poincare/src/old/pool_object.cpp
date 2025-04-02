#include <poincare/old/pool.h>
#include <poincare/old/pool_handle.h>
#include <poincare/old/pool_object.h>

namespace Poincare {

// Node operations

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
     * if the node is a clone and still has the original node's identifier,
     * unregistering it would lose the access to the original node. */
    Pool::sharedPool->unregisterNode(this);
  }
  m_identifier = identifier;
  m_referenceCounter = 0;
  Pool::sharedPool->registerNode(this);
}

// Protected

#if POINCARE_TREE_LOG
void PoolObject::log(std::ostream &stream, int indentation, bool verbose) {
  stream << "\n";
  for (int i = 0; i < indentation; ++i) {
    stream << "  ";
  }
  stream << "<";
  logNodeName(stream);
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
    logNodeName(stream);
    stream << ">";
  } else {
    stream << "/>";
  }
}
#endif

size_t PoolObject::deepSize() const {
  return reinterpret_cast<char *>(next()) -
         reinterpret_cast<const char *>(this);
}

}  // namespace Poincare
