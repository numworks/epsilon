#include <ion.h>
#include <ion/events.h>
#include <stdio.h>
#include <string.h>
#include "journal.h"

namespace Ion {
namespace Simulator {
namespace StateFile {

constexpr static const char * sHeader = "NWSF";
constexpr static int sHeaderLength = 4;
constexpr static int sVersionLength = 8;
constexpr static const char * sWildcardVersion = "**.**.**";
constexpr static int sFormatVersionLength = 1;
constexpr static uint8_t sLatestFormatVersion = 1;
constexpr static int sLanguageLength = Ion::Events::Journal::k_languageSize-1;
constexpr static const char * sWildcardLanguage = "**";

/* File format:
 * Format version 0xFF (latest) :
 *   "NWSF" : Header
 * + "XXXXXXXX" : Software version
 * + "0x01" : State file format version
 * + "XX" : Language code (en, fr, nl, pt, it, de, or es)
 * + EVENTS...
 */

static inline bool load(FILE * f) {
  char buffer[sVersionLength+1];

  // Header
  buffer[sHeaderLength] = 0;
  if (fread(buffer, sHeaderLength, 1, f) != 1) {
    return false;
  }
  if (strcmp(buffer, sHeader) != 0) {
    return false;
  }

  // Software version
  buffer[sVersionLength] = 0;
  if (fread(buffer, sVersionLength, 1, f) != 1) {
    return false;
  }
  if (strcmp(buffer, epsilonVersion()) != 0 && strcmp(buffer, sWildcardVersion) != 0) {
    return false;
  }

  // Journal
  Ion::Events::Journal * journal = Journal::replayJournal();

  // Format version
  int c = 0;
  static_assert(sFormatVersionLength == 1, "sFormatVersionLength is incorrect");
  if ((c = getc(f)) == EOF || c != sLatestFormatVersion) {
    // Only the latest version is handled for now.
    return false;
  }

  // Language
  static_assert(sVersionLength + 1 > sLanguageLength, "Buffer isn't long enough for language");
  buffer[sLanguageLength] = 0;
  if (fread(buffer, sLanguageLength, 1, f) != 1) {
    return false;
  }
  if (strcmp(buffer, sWildcardLanguage) != 0) {
    journal->setStartingLanguage(buffer);
  }

  // Events
  while ((c = getc(f)) != EOF) {
    Ion::Events::Event e = Ion::Events::Event(c);
    if (Events::isDefined(static_cast<uint8_t>(e))
        && e != Ion::Events::None
        && e != Ion::Events::Termination
        && e != Ion::Events::TimerFire
        && e != Ion::Events::ExternalText) {
      /* Avoid pushing invalid events - useful when fuzzing.
       * ExternalText is not handled by state files. */
      journal->pushEvent(e);
    }
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
  if (f != stdin) {
    fclose(f);
  }
}

static inline bool save(FILE * f) {
  if (fwrite(sHeader, sHeaderLength, 1, f) != 1) {
    return false;
  }
#if ASSERTIONS
  if (fwrite(sWildcardVersion, sVersionLength, 1, f) != 1) {
    return false;
  }
#else
  if (fwrite(epsilonVersion(), sVersionLength, 1, f) != 1) {
    return false;
  }
#endif
  if (fwrite(&sLatestFormatVersion, sFormatVersionLength, 1, f) != 1) {
    return false;
  }
  Ion::Events::Journal * journal = Journal::logJournal();
  const char * logJournalLanguage = journal->startingLanguage()[0] != 0 ? journal->startingLanguage() : sWildcardLanguage;
  if (fwrite(logJournalLanguage, sLanguageLength, 1, f) != 1) {
    return false;
  }
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
