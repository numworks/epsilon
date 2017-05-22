#include "sequence_store.h"
extern "C" {
#include <assert.h>
#include <stddef.h>
}
#include <ion.h>

namespace Sequence {

constexpr KDColor SequenceStore::k_defaultColors[k_maxNumberOfSequences];
constexpr const char * SequenceStore::k_sequenceNames[k_maxNumberOfSequences];

uint32_t SequenceStore::storeChecksum() {
  size_t dataLengthInBytes = k_maxNumberOfSequences*3*TextField::maxBufferSize()*sizeof(char);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  char data[3*k_maxNumberOfSequences*TextField::maxBufferSize()] = {};
  for (int i = 0; i < k_maxNumberOfSequences; i++) {
    strlcpy(data+i*3*TextField::maxBufferSize(), m_sequences[i].text(), TextField::maxBufferSize());
    strlcpy(data+i*3*TextField::maxBufferSize()+TextField::maxBufferSize(), m_sequences[i].firstInitialConditionText(), TextField::maxBufferSize());
    strlcpy(data+i*3*TextField::maxBufferSize()+2*TextField::maxBufferSize(), m_sequences[i].secondInitialConditionText(), TextField::maxBufferSize());
  }
  return Ion::crc32((uint32_t *)data, dataLengthInBytes>>2);
}

Sequence * SequenceStore::functionAtIndex(int i) {
  assert(i>=0 && i<m_numberOfFunctions);
  return &m_sequences[i];
}

Sequence * SequenceStore::activeFunctionAtIndex(int i) {
  return (Sequence *)Shared::FunctionStore::activeFunctionAtIndex(i);
}

Sequence * SequenceStore::definedFunctionAtIndex(int i) {
  return (Sequence *)Shared::FunctionStore::definedFunctionAtIndex(i);
}

Sequence * SequenceStore::addEmptyFunction() {
  assert(m_numberOfFunctions < k_maxNumberOfSequences);
  const char * name = firstAvailableName();
  KDColor color = firstAvailableColor();
  Sequence addedSequence(name, color);
  m_sequences[m_numberOfFunctions] = addedSequence;
  Sequence * result = &m_sequences[m_numberOfFunctions];
  m_numberOfFunctions++;
  return result;
}

void SequenceStore::removeFunction(Shared::Function * f) {
  int i = 0;
  while (&m_sequences[i] != f && i < m_numberOfFunctions) {
    i++;
  }
  assert(i>=0 && i<m_numberOfFunctions);
  m_numberOfFunctions--;
  for (int j = i; j<m_numberOfFunctions; j++) {
    m_sequences[j] = m_sequences[j+1];
  }
  Sequence emptySequence("", KDColorBlack);
  m_sequences[m_numberOfFunctions] = emptySequence;
}

int SequenceStore::maxNumberOfFunctions() {
  return k_maxNumberOfSequences;
}

char SequenceStore::symbol() const {
  return 'n';
}

const char *  SequenceStore::firstAvailableName() {
  for (int k = 0; k < k_maxNumberOfSequences; k++) {
    int j = 0;
    while  (j < m_numberOfFunctions) {
      if (m_sequences[j].name() == k_sequenceNames[k]) {
        break;
      }
      j++;
    }
    if (j == m_numberOfFunctions) {
      return k_sequenceNames[k];
    }
  }
  return k_sequenceNames[0];
}

const KDColor SequenceStore::firstAvailableColor() {
  for (int k = 0; k < k_maxNumberOfSequences; k++) {
    int j = 0;
    while  (j < m_numberOfFunctions) {
      if (m_sequences[j].color() == k_defaultColors[k]) {
        break;
      }
      j++;
    }
    if (j == m_numberOfFunctions) {
      return k_defaultColors[k];
    }
  }
  return k_defaultColors[0];
}

void SequenceStore::removeAll() {
  for (int i = 0; i < m_numberOfFunctions; i++) {
    Sequence emptySequence("", KDColorBlack);
    m_sequences[i] = emptySequence;
  }
  m_numberOfFunctions = 0;
}

}
