#include <poincare/pool.h>
#include <poincare/pool_handle.h>
#include <poincare/pool_object.h>

namespace Poincare {

// Object operations

void PoolObject::release() {
  m_referenceCounter--;
  if (m_referenceCounter == 0) {
    Pool::sharedPool->discardPoolObject(this);
  }
}

void PoolObject::rename(uint16_t identifier) {
  m_identifier = identifier;
  m_referenceCounter = 0;
  assert(!Pool::sharedPool->isRegistered(identifier));
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
