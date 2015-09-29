#include <poincare/variable.h>
#include <stdlib.h>
#include "layout/string_layout.h"

Variable ** sRegisteredVariables = NULL;
int16_t sRegisteredVariableCount = 0;
#define MAX_REGISTERED_VARIABLES 10

Variable::Variable(char * name) :
m_value(0.0f) {
  size_t length = strlen(name);
  m_name = (char *)malloc(sizeof(char)*length+1);
  memcpy(m_name, name, length);
  m_name[length] = 0;
  RegisterVariable(this);
}

Variable::~Variable() {
  UnregisterVariable(this);
  free(m_name);
}

float Variable::approximate() {
  return m_value;
}

ExpressionLayout * Variable::createLayout(ExpressionLayout * parent) {
  size_t length = strlen(m_name);
  return new StringLayout(parent, m_name, length);
}

Variable * Variable::VariableNamed(char * name) {
  for (int16_t i=0; i<sRegisteredVariableCount; i++) {
    Variable * currentVariable = sRegisteredVariables[i];
    if (strcmp(currentVariable->m_name, name) == 0) {
      return currentVariable;
    }
  }
  return NULL;
}

void Variable::setValue(float value) {
  m_value = value;
}

void Variable::RegisterVariable(Variable * v) {
  if (sRegisteredVariables == NULL) {
    sRegisteredVariables = (Variable **)malloc(MAX_REGISTERED_VARIABLES*sizeof(Variable *));
  }
  sRegisteredVariables[sRegisteredVariableCount++] = v;
}

void Variable::UnregisterVariable(Variable * v) {
  // TODO!
}
