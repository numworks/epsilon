#include <python/test/execution_environment.h>
#include <quiz.h>
#include <string.h>

#include <array>

#include "../python_variable_box_controller.h"
#include "../script_store.h"

using namespace Code;

void assert_variables_are(const char* script,
                          const size_t nameToCompleteOffsetInScript,
                          const size_t nameToCompleteLength,
                          const char** expectedVariables,
                          int expectedVariablesCount) {
  // Clean the store
  ScriptStore::DeleteAllScripts();

  // Add the script
  ScriptStore::AddNewScript();
  constexpr int dataBufferSize = 500;
  char dataBuffer[dataBufferSize];
  Ion::Storage::Record::Data data = {.buffer = &dataBuffer,
                                     .size = dataBufferSize};
  strlcpy(dataBuffer, script, dataBufferSize);
  constexpr int scriptIndex = 0;
  ScriptStore::ScriptAtIndex(scriptIndex).setValue(data);

  // Load the variable box
  PythonVariableBoxController varBox;

  const char* nameToComplete = script + nameToCompleteOffsetInScript;
  varBox.loadFunctionsAndVariables(scriptIndex, nameToComplete,
                                   nameToCompleteLength);

  // Compare the variables
  int index = 0;  // Index to make sure we are not cycling through the results
  int textToInsertLength;
  bool addParentheses;
  for (int i = 0; i < expectedVariablesCount; i++) {
    quiz_assert(i == index);
    const char* autocompletionI = varBox.autocompletionAlternativeAtIndex(
        nameToCompleteLength, &textToInsertLength, &addParentheses, i, &index);
    /* If false, the autocompletion has cycled: there are not as many results as
     * expected */
    quiz_assert(i == index);
    quiz_assert(strncmp(*(expectedVariables + i),
                        autocompletionI - nameToCompleteLength,
                        textToInsertLength + nameToCompleteLength) == 0);
    index++;
  }
  varBox.autocompletionAlternativeAtIndex(strlen(nameToComplete),
                                          &textToInsertLength, &addParentheses,
                                          index, &index);
  /* Assert the autocompletion has cycles: otherwise, there are more results
   * than expected. */
  quiz_assert(index == 0);
}

QUIZ_CASE(code_variable_box) {
  init_environment();

  const char* expectedVariables[] = {"froo", "from", "frozenset()"};
  // FIXME This test does not load imported variables for now
  assert_variables_are("\x01 from math import *\nfroo=3", 21, 2,
                       expectedVariables, std::size(expectedVariables));

  deinit_environment();
}
