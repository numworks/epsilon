#include <ion.h>
#include <ion/events.h>
#include <stdio.h>
#include <string.h>
#include "journal.h"

namespace Ion {
namespace Simulator {
namespace StateFile {

static constexpr const char * sHeader = "NWSF";
static constexpr int sHeaderLength = 4;
static constexpr int sVersionLength = 8;

/* File format: * "NWSF" + "XXXXXXXX" (version) + EVENTS... */

static inline bool load(FILE * f) {
  char buffer[sVersionLength+1];

  // Header
  buffer[sHeaderLength] = 0;
  if (fread(buffer, sHeaderLength, 1, f) != 1) {
    return false;
  }
  printf("READ\n");
  if (strcmp(buffer, sHeader) != 0) {
    return false;
  }

  // Software version
  buffer[sVersionLength] = 0;
  if (fread(buffer, sVersionLength, 1, f) != 1) {
    return false;
  }
  if (strcmp(buffer, softwareVersion()) != 0) {
    return false;
  }

  // Events
  Ion::Events::Journal * journal = Journal::replayJournal();
  while (fread(buffer, 1, 1, f) == 1) {
    Ion::Events::Event e = Ion::Events::Event(buffer[0]);
    journal->pushEvent(e);
  }
  Ion::Events::replayFrom(journal);

  return true;
}

void load(const char * filename) {
  FILE * f = nullptr;
  if (strcmp(filename, "-") == 0) {
    f = stdin;
  } else {
    f = fopen(filename, "rb");
  }
  if (f == nullptr) {
    return;
  }
  load(f);
  fclose(f);
}

static inline bool save(FILE * f) {
  if (fwrite(sHeader, sHeaderLength, 1, f) != 1) {
    return false;
  }
  if (fwrite(softwareVersion(), sVersionLength, 1, f) != 1) {
    return false;
  }
  Ion::Events::Journal * journal = Journal::logJournal();
  Ion::Events::Event e;
  while (!journal->isEmpty()) {
    Ion::Events::Event e = journal->popEvent();
    uint8_t code = static_cast<uint8_t>(e);
    if (fwrite(&code, 1, 1, f) != 1) {
      return false;
    }
  }
  return true;
}

void save(const char * filename) {
  FILE * f = fopen(filename, "w");
  if (f == nullptr) {
    return;
  }
  save(f);
  fclose(f);
}

}
}
}
