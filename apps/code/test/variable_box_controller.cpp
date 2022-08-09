#include <quiz.h>
#include "../script_store.h"
#include "../variable_box_controller.h"
#include <string.h>

using namespace Code;

void assert_variables_are(const char * script, const size_t nameToCompleteOffsetInScript, const size_t nameToCompleteLength, const char * * expectedVariables, int expectedVariablesCount) {
  // Clean the store
  ScriptStore store;
  store.deleteAllScripts();

  // Add the script
  store.addNewScript();
  constexpr int dataBufferSize = 500;
  char dataBuffer[dataBufferSize];
  Ion::Storage::Record::Data data = {
    .buffer = &dataBuffer,
    .size = dataBufferSize
  };
  strlcpy(dataBuffer, script, dataBufferSize);
  constexpr int scriptIndex = 0;
  store.scriptAtIndex(scriptIndex).setValue(data);

  // Load the variable box
  VariableBoxController varBox(&store);

  const char * nameToComplete = script + nameToCompleteOffsetInScript;
  varBox.loadFunctionsAndVariables(scriptIndex, nameToComplete, nameToCompleteLength);

  // Compare the variables
  int index = 0; // Index to make sure we are not cycling through the results
  int textToInsertLength;
  bool addParentheses;
  for (int i = 0; i < expectedVariablesCount; i++) {
    quiz_assert(i == index);
    const char * autocompletionI = varBox.autocompletionAlternativeAtIndex(
        nameToCompleteLength,
        &textToInsertLength,
        &addParentheses,
        i,
        &index);
    // If false, the autocompletion has cycled: there are not as many results as expected
    quiz_assert(i == index);
    quiz_assert(strncmp(*(expectedVariables + i), autocompletionI - nameToCompleteLength, textToInsertLength + nameToCompleteLength) == 0);
    index++;
  }
  varBox.autocompletionAlternativeAtIndex(
      strlen(nameToComplete),
      &textToInsertLength,
      &addParentheses,
      index,
      &index);
  /* Assert the autocompletion has cycles: otherwise, there are more results
   * than expected. */
  quiz_assert(index == 0);
}

QUIZ_CASE(variable_box_controller) {
  const char * expectedVariables[] = {
    "froo",
    "from",
    "frozenset()"
  };
  // FIXME This test does not load imported variables for now
  assert_variables_are(
      "\x01 from math import *\nfroo=3",
      21,
      2,
      expectedVariables,
      sizeof(expectedVariables) / sizeof(const char *));
}
