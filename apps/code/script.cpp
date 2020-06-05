#include "script.h"
#include "script_store.h"

namespace Code {

static inline void intToText(int i, char * buffer, int bufferSize) {
  // We only support integers from 0 to 99.
  assert(i >= 0);
  assert(i < 100);
  assert(bufferSize >= 3);
  if (i/10 == 0) {
    buffer[0] = i+'0';
    buffer[1] = 0;
    return;
  }
  buffer[0] = i/10+'0';
  buffer[1] = i-10*(i/10)+'0';
  buffer[2] = 0;
}

bool Script::DefaultName(char buffer[], size_t bufferSize) {
  assert(bufferSize >= k_defaultScriptNameMaxSize);
  static constexpr char defaultScriptName[] = "script";
  static constexpr int defaultScriptNameLength = 6;
  strlcpy(buffer, defaultScriptName, bufferSize);

  int currentScriptNumber = 1;
  while (currentScriptNumber <= k_maxNumberOfDefaultScriptNames) {
    // Change the number in the script name.
    intToText(currentScriptNumber, &buffer[defaultScriptNameLength], bufferSize - defaultScriptNameLength );
    if (ScriptStore::ScriptNameIsFree(buffer)) {
      return true;
    }
    currentScriptNumber++;
  }
  // We did not find a new script name
  return false;
}

bool Script::nameCompliant(const char * name) {
  /* We allow here the empty script name ".py", because it is the name used to
   * create a new empty script. When naming or renaming a script, we check
   * elsewhere that the name is no longer empty.
   * The name format is ([a-z_][a-z0-9_]*)*\.py
   *
   * We do not allow upper cases in the script names because script names are
   * used in the URLs of the NumWorks workshop website and we do not want
   * problems with case sensitivity. */
  UTF8Decoder decoder(name);
  CodePoint c = decoder.nextCodePoint();
  if (c == UCodePointNull || !(c.isLatinSmallLetter() || c == '_' || c == '.')) {
    /* The name cannot be empty. Its first letter must be in [a-z_] or the
     * extension dot. */
    return false;
  }
  while (c != UCodePointNull) {
    if (c == '.' && strcmp(decoder.stringPosition(), ScriptStore::k_scriptExtension) == 0) {
      return true;
    }
    if (!(c.isLatinSmallLetter() || c == '_' || c.isDecimalDigit())) {
      return false;
    }
    c = decoder.nextCodePoint();
  }
  return false;
}

bool Script::autoImportationStatus() const {
  assert(!isNull());
  Data d = value();
  return (*statusFromData(d) & k_autoImportationStatusMask) == 1;
}

void Script::toggleAutoimportationStatus() {
  assert(!isNull());
  Data d = value();
  *statusFromData(d) ^= k_autoImportationStatusMask;
  setValue(d);
}

const char * Script::content() const {
  Data d = value();
  return ((const char *)d.buffer) + StatusSize();
}

bool Script::contentFetchedFromConsole() const {
  return fetchedStatus() == FetchedStatus::FromConsole;
}

bool Script::contentFetchedForVariableBox() const {
  return fetchedStatus() == FetchedStatus::ForVariableBox;
}

void Script::setContentFetchedFromConsole() {
  setFetchedStatus(FetchedStatus::FromConsole);
}

void Script::setContentFetchedForVariableBox() {
  setFetchedStatus(FetchedStatus::ForVariableBox);
}

void Script::resetContentFetchedStatus() {
  setFetchedStatus(FetchedStatus::None);
}

Script::FetchedStatus Script::fetchedStatus() const {
  assert(!isNull());
  Data d = value();
  uint8_t status = (*statusFromData(d)) >> k_fetchedStatusOffset;
  assert(status == static_cast<uint8_t>(FetchedStatus::None)
      || status == static_cast<uint8_t>(FetchedStatus::FromConsole)
      || status == static_cast<uint8_t>(FetchedStatus::ForVariableBox));
  return static_cast<FetchedStatus>(status);
}

void Script::setFetchedStatus(FetchedStatus fetchedStatus) {
  assert(!isNull());
  Data d = value();
  uint8_t * status = statusFromData(d);
  *status = ((*status) & ~k_fetchedStatusMask) | (static_cast<uint8_t>(fetchedStatus) << k_fetchedStatusOffset); //TODO Create and use a bit operations library
  setValue(d);
}

uint8_t * Script::statusFromData(Data d) const {
  return const_cast<uint8_t *>(static_cast<const uint8_t *>(d.buffer));
}

}
