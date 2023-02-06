#include "script.h"
#include "script_store.h"

namespace Code {

bool Script::DefaultName(char buffer[], size_t bufferSize) {
  assert(bufferSize >= k_defaultScriptNameMaxSize);
  constexpr static char k_defaultScriptName[] = "script";
  constexpr static int k_defaultScriptNameLength = sizeof(k_defaultScriptName) - 1;
  strlcpy(buffer, k_defaultScriptName, bufferSize);
  const char * const extensions[1] = { ScriptStore::k_scriptExtension };
  return Ion::Storage::FileSystem::sharedFileSystem->firstAvailableNameFromPrefix(buffer, k_defaultScriptNameLength, bufferSize, extensions, 1, k_maxNumberOfDefaultScriptNames) > k_defaultScriptNameLength;
}

bool Script::NameCompliant(const char * name) {
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

Script::ErrorStatus Script::Create(const char * name, const char * content) {
  assert(NameCompliant(name));
  Status status;
  const void * dataChunks[] = {&status, content};
  size_t sizeChunks[] = {sizeof(status), strlen(content)+1};
  ErrorStatus err = Ion::Storage::FileSystem::sharedFileSystem->createRecordWithFullNameAndDataChunks(name, dataChunks, sizeChunks, 2);
  assert(err != ErrorStatus::NonCompliantName);
  return err;
}

}
