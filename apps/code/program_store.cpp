#include "program_store.h"
#include "string.h"
#include <stddef.h>

namespace Code {

ProgramStore::ProgramStore() :
  m_numberOfPrograms(0),
  m_lastProgramEdited(-1)
{
  for (int i = 0; i<k_historySize; i ++) {
    m_history[i] = FreeSpaceMarker;
  }
}

Program ProgramStore::editableProgram(int i) {
  assert(i >= 0 && i < numberOfPrograms());
  cleanAndMoveFreeSpaceAfterProgram(i);
  int beginningOfProgram = indexOfProgram(i);

  // Compute the size of the program, including the free space of m_history
  int sizeOfEditableProgram = 0;
  for (int j=beginningOfProgram; j<k_historySize-1; j++) {
    if (m_history[j] == FreeSpaceMarker && m_history[j+1] != FreeSpaceMarker) {
      break;
    }
    sizeOfEditableProgram++;
  }
  m_lastProgramEdited = i;
  return Program(&m_history[beginningOfProgram], sizeOfEditableProgram);
}

Program ProgramStore::program(int i) {
  assert(i >= 0 && i < numberOfPrograms());
  int beginningOfProgram = indexOfProgram(i);
  return Program(&m_history[beginningOfProgram], strlen(&m_history[beginningOfProgram] + 1));
}

int ProgramStore::numberOfPrograms() const {
  return m_numberOfPrograms;
}

Program ProgramStore::editableNewProgram() {
  cleanAndMoveFreeSpaceAfterProgram(numberOfPrograms()-1);
  m_numberOfPrograms++;
  return Program(&m_history[indexOfFirstFreeSpaceMarker()], sizeOfFreeSpace() - 1);
}

void ProgramStore::deleteProgram(int i){
  assert (i >= 0 && i < numberOfPrograms());
  cleanAndMoveFreeSpaceAfterProgram(i);
  int indexOfCharToDelete = indexOfProgram(i);
  while (m_history[indexOfCharToDelete] != FreeSpaceMarker) {
    m_history[indexOfCharToDelete] = FreeSpaceMarker;
    indexOfCharToDelete++;
  }
  m_numberOfPrograms--;
}

void ProgramStore::deleteAll(){
  for (int i = 0; i<numberOfPrograms(); i++){
    deleteProgram(i);
    //TODO moves the free space around, better to just put all the buffer to -1 ??
  }
  m_numberOfPrograms = 0;
}

int ProgramStore::indexOfProgram(int i) const {
  assert (i >= 0 && i < numberOfPrograms());
  int currentProgramNumber = 0;
  int beginningOfProgram = 0;
  while (m_history[beginningOfProgram] == FreeSpaceMarker) {
    beginningOfProgram++;
  }
  if (i == 0) {
    return beginningOfProgram;
  }
  for (int j=beginningOfProgram; j<k_historySize; j++) {
    if (m_history[j] == 0) {
      currentProgramNumber++;
      if (currentProgramNumber == i) {
        j++;
        while (m_history[j] == FreeSpaceMarker) {
          j++;
        }
        return j;
      }
    }
  }
  assert(false);
  return 0;
}

int ProgramStore::lastIndexOfProgram(int i) const {
  int indexOfPrgm = indexOfProgram(i);
  int lastIndexOfProgram = indexOfPrgm + strlen(&m_history[indexOfPrgm]);
  return lastIndexOfProgram;
}

int ProgramStore::indexOfFirstFreeSpaceMarker() const {
  for (int i=0; i<k_historySize; i++) {
    if (m_history[i] == FreeSpaceMarker) {
      return i;
    }
  }
  assert(false);
  return 0;
}

int ProgramStore::sizeOfFreeSpace() const {
  int sizeOfFreeSpace = 0;
  for (int i=0; i<k_historySize; i++) {
    if (m_history[i] == FreeSpaceMarker) {
      sizeOfFreeSpace++;
    } else {
      if (sizeOfFreeSpace > 0) {
        return sizeOfFreeSpace;
      }
    }
  }
  return sizeOfFreeSpace;
}

void ProgramStore::cleanFreeSpace() {
  if (m_lastProgramEdited < 0 || m_lastProgramEdited >= numberOfPrograms()) {
    return;
  }
  int indexOfCharToChangeIntoFreeSpaceMarker = lastIndexOfProgram(m_lastProgramEdited) + 1;
  while (m_history[indexOfCharToChangeIntoFreeSpaceMarker] != FreeSpaceMarker) {
    m_history[indexOfCharToChangeIntoFreeSpaceMarker] = FreeSpaceMarker;
    indexOfCharToChangeIntoFreeSpaceMarker ++;
  }
}

void ProgramStore::moveFreeSpaceAfterProgram(int i) {
  m_lastProgramEdited = i;

  // If the free space is already just after the program, return
  int indexOfFreeSpace = indexOfFirstFreeSpaceMarker();
  int lastIndexOfPrgm = lastIndexOfProgram(i);
  if (indexOfFreeSpace == lastIndexOfPrgm + 1){
    return;
  }
  // Else move the free space depending on its relative position with the program
  int freeSpaceSize = sizeOfFreeSpace();
  if (indexOfFreeSpace > lastIndexOfPrgm) {
    int indexOfNextProgram = lastIndexOfPrgm+1;
    int len = indexOfFreeSpace - indexOfNextProgram;
    // Use memmove to avoid overwriting
    memmove(&m_history[indexOfNextProgram + freeSpaceSize], &m_history[indexOfNextProgram], len);
    for (int j = indexOfNextProgram; j<indexOfNextProgram+freeSpaceSize; j++) {
      m_history[j] = FreeSpaceMarker;
    }
  } else {
    int indexOfPrgm = indexOfProgram(i);
    int len = lastIndexOfPrgm - (indexOfFreeSpace + freeSpaceSize) + 1;
    // Use memcopy to avoid overwriting
    memcpy(&m_history[indexOfFreeSpace], &m_history[indexOfPrgm], len);
    for (int j = lastIndexOfPrgm - freeSpaceSize + 1 ; j<=lastIndexOfPrgm; j++) {
      m_history[j] = FreeSpaceMarker;
    }
  }
}

void ProgramStore::cleanAndMoveFreeSpaceAfterProgram(int i) {
  if (i >= 0 && i<numberOfPrograms()){
    cleanFreeSpace();
    moveFreeSpaceAfterProgram(i);
  }
}

bool ProgramStore::addDefaultProgram() {
  const char program[] = R"(# This program draws a Mandelbrot fractal set
# N_iteration: degree of precision
import kandinsky
N_iteration = 10
for x in range(320):
  for y in range(240):
# Compute the mandelbrot sequence for the point c = (c_r, c_i) with start value z = (z_r, z_i)
    z_r = 0
    z_i = 0
# Rescale to fit the drawing screen 320x222
    c_r = 2.7*x/319-2.1
    c_i = -1.87*y/221+0.935
    i = 0
    while (i < N_iteration) and ((z_r * z_r) + (z_i * z_i) < 4):
      i = i + 1
      stock = z_r
      z_r = z_r * z_r - z_i * z_i + c_r
      z_i = 2 * stock * z_i + c_i
# Choose the color of the dot from the Mandelbrot sequence
    rgb = int(255*i/N_iteration)
    col = kandinsky.color(int(rgb),int(rgb*0.75),int(rgb*0.25))
# Draw a pixel colored in 'col' at position (x,y)
    kandinsky.set_pixel(x,y,col))";

  int len = strlen(program);
  if (len + 1 > sizeOfFreeSpace() - 1) { // We keep at keast one free char.
    return false;
  }
  cleanAndMoveFreeSpaceAfterProgram(numberOfPrograms()-1);
  memcpy(&m_history[indexOfFirstFreeSpaceMarker()], program, len+1);
  m_numberOfPrograms++;
  m_lastProgramEdited = m_numberOfPrograms-1;
  return true;
}

}
