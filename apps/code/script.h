#ifndef CODE_SCRIPT_H
#define CODE_SCRIPT_H

#include <escher.h>

namespace Code {

class Script {
public:
  Script(const char * marker = nullptr, const char * name = nullptr, size_t nameBufferSize = 0, const char * content = nullptr, size_t contentBufferSize = 0);
  bool isNull() const;
  bool autoImport() const;

  const char * name() const {
    assert(!isNull());
    assert(m_name != nullptr);
    return m_name;
  }

  char * editableName() {
    assert(!isNull());
    assert(m_name != nullptr);
    return const_cast<char *>(m_name);
  }

  /* nameBufferSize() might not be equal to strlen(name()): There might be free
   * space (chars equal to ScriptStore::FreeSpaceMarker), that is used to edit
   * the script name. */
  size_t nameBufferSize() const {
    assert(!isNull());
    return m_nameBufferSize;
  }

  const char * content() const {
    assert(!isNull());
    assert(m_content != nullptr);
    return m_content;
  }

  char * editableContent() {
    assert(!isNull());
    assert(m_content != nullptr);
    return const_cast<char *>(m_content);
  }

  /* contentBufferSize() might not be equal to strlen(name()): There might be
   * free space (chars equal to ScriptStore::FreeSpaceMarker), that is used to
   * edit the script content. */
  size_t contentBufferSize() const {
    assert(!isNull());
    return m_contentBufferSize;
  }

  static constexpr int NumberOfStringsPerScript = 3;
  static constexpr char AutoImportationMarker = 2;
  static constexpr char NoAutoImportationMarker = 3;
  static constexpr char DefaultAutoImportationMarker = AutoImportationMarker;
  /* We made sure that these chars are not used in ion/include/ion/charset.h,
   * nor used in the ScriptStore. */
private:
  const char * m_marker;
  const char * m_name;
  size_t m_nameBufferSize;
  const char * m_content;
  size_t m_contentBufferSize;
};

}

#endif
