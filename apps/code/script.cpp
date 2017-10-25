#include "script.h"

namespace Code {

Script::Script(const char * marker, const char * name, size_t nameBufferSize, const char * content, size_t contentBufferSize) :
  m_marker(marker),
  m_name(name),
  m_nameBufferSize(nameBufferSize),
  m_content(content),
  m_contentBufferSize(contentBufferSize)
{
}

bool Script::isNull() const {
  if (m_marker == nullptr) {
    assert(m_name == nullptr);
    assert(m_nameBufferSize == 0);
    assert(m_content == nullptr);
    assert(m_contentBufferSize == 0);
    return true;
  }
  return false;
}

bool Script::autoImport() const {
  assert(!isNull());
  assert(m_marker != nullptr);
  if (m_marker[0] == AutoImportationMarker) {
    return true;
  }
  assert (m_marker[0] == NoAutoImportationMarker);
  return false;
}

}

