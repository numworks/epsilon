#include "script_store.h"

namespace Code {

constexpr char ScriptStore::k_scriptExtension[];

bool ScriptStore::ScriptNameIsFree(const char * baseName) {
  return ScriptBaseNamed(baseName).isNull();
}

ScriptStore::ScriptStore() {
  addScriptFromTemplate(ScriptTemplate::Squares());
  addScriptFromTemplate(ScriptTemplate::Parabola());
  addScriptFromTemplate(ScriptTemplate::Mandelbrot());
  addScriptFromTemplate(ScriptTemplate::Polynomial());
}

void ScriptStore::deleteAllScripts() {
  for (int i = numberOfScripts() - 1; i >= 0; i--) {
    scriptAtIndex(i).destroy();
  }
}

bool ScriptStore::isFull() {
  return Ion::Storage::sharedStorage()->availableSize() < k_fullFreeSpaceSizeLimit;
}

const char * ScriptStore::contentOfScript(const char * name) {
  Script script = ScriptNamed(name);
  if (script.isNull()) {
    return nullptr;
  }
  return script.scriptContent();
}

Script::ErrorStatus ScriptStore::addScriptFromTemplate(const ScriptTemplate * scriptTemplate) {
  size_t valueSize = strlen(scriptTemplate->content())+1+1;// scriptcontent size + 1 char for the importation status
  assert(Script::nameCompliant(scriptTemplate->name()));
  Script::ErrorStatus err = Ion::Storage::sharedStorage()->createRecordWithFullName(scriptTemplate->name(), scriptTemplate->value(), valueSize);
  assert(err != Script::ErrorStatus::NonCompliantName);
  return err;
}

}
