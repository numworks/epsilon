#include "sequence_store.h"
extern "C" {
#include <assert.h>
#include <stddef.h>
}
#include <ion.h>

namespace Sequence {

constexpr KDColor SequenceStore::k_defaultColors[MaxNumberOfSequences];
constexpr const char * SequenceStore::k_sequenceNames[MaxNumberOfSequences];

uint32_t SequenceStore::storeChecksum() {
  size_t dataLengthInBytes = MaxNumberOfSequences*sizeof(uint32_t);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  uint32_t checksums[MaxNumberOfSequences];
  for (int i = 0; i < MaxNumberOfSequences; i++) {
    checksums[i] = m_sequences[i].checksum();
  }
  return Ion::crc32((uint32_t *)checksums, dataLengthInBytes/sizeof(uint32_t));
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
  assert(m_numberOfFunctions < MaxNumberOfSequences);
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
  return MaxNumberOfSequences;
}

char SequenceStore::symbol() const {
  return 'n';
}

const char *  SequenceStore::firstAvailableName() {
  for (int k = 0; k < MaxNumberOfSequences; k++) {
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
  for (int k = 0; k < MaxNumberOfSequences; k++) {
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
