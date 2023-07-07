#include <ion.h>
#include <ion/events.h>
#include <stdio.h>
#include <string.h>

#include "journal.h"

namespace Ion {
namespace Simulator {
namespace StateFile {

constexpr static const char* sMagic = "NWSF";
constexpr static size_t sMagicLength = 4;
constexpr static size_t sVersionLength = 8;
constexpr static const char* sWildcardVersion = "**.**.**";
constexpr static size_t sFormatVersionLength = 1;
constexpr static uint8_t sLatestFormatVersion = 1;
constexpr static size_t sLanguageLength =
    Ion::Events::Journal::k_languageSize - 1;
constexpr static const char* sWildcardLanguage = "**";
constexpr static size_t sHeaderLength =
    sMagicLength + sVersionLength + sFormatVersionLength + sLanguageLength;

/* File format:
 * Format version 0xFF (latest) :
 *   "NWSF" : Magic
 * + "XXXXXXXX" : Software version
 * + "0x01" : State file format version
 * + "XX" : Language code (en, fr, nl, pt, it, de, or es)
 * + EVENTS...
 */

static inline bool loadFileHeader(const char* header) {
  const char* magic = header;
  const char* version = magic + sMagicLength;
  const char* formatVersion = version + sVersionLength;
  const char* language = formatVersion + sFormatVersionLength;

  if (strncmp(magic, sMagic, sMagicLength) != 0) {
    return false;
  }
  if (strncmp(version, epsilonVersion(), sVersionLength) != 0 &&
      strncmp(version, sWildcardVersion, sVersionLength) != 0) {
    return false;
  }
  if (*formatVersion != sLatestFormatVersion) {
    return false;
  }
  if (strncmp(language, sWildcardLanguage, sLanguageLength) != 0) {
    Journal::replayJournal()->setStartingLanguage(language);
  }
  return true;
}

static inline void pushEvent(uint8_t c) {
  Ion::Events::Event e = Ion::Events::Event(c);
  if (!Events::isDefined(static_cast<uint8_t>(
          e))) {  // If not defined, fall back on a normal key event.
    e = Ion::Events::Event(static_cast<uint8_t>(
        Keyboard::ValidKeys[c % Ion::Keyboard::NumberOfValidKeys]));
  }
  assert(Events::isDefined(static_cast<uint8_t>(e)));

  if (e == Ion::Events::None || e == Ion::Events::Termination ||
      e == Ion::Events::TimerFire || e == Ion::Events::ExternalText) {
    return;
  }
  /* ExternalText is not yet handled by state files. */
  Journal::replayJournal()->pushEvent(e);
}

static inline bool loadFile(FILE* f, bool headlessStateFile) {
  if (!headlessStateFile) {
    char header[sHeaderLength + 1];
    header[sHeaderLength] = 0;
    if (fread(header, sHeaderLength, 1, f) != 1) {
      return false;
    }
    if (!loadFileHeader(header)) {
      return false;
    }
  }
  // Events
  int c = 0;
  while ((c = getc(f)) != EOF) {
    pushEvent(c);
  }

  Ion::Events::replayFrom(Journal::replayJournal());

  return true;
}

void load(const char* filename, bool headlessStateFile) {
  FILE* f = nullptr;
  if (strcmp(filename, "-") == 0) {
    f = stdin;
  } else {
    f = fopen(filename, "rb");
  }
  if (f == nullptr) {
    return;
  }
  loadFile(f, headlessStateFile);
  if (f != stdin) {
    fclose(f);
  }
}

void loadMemory(const char* buffer, size_t length, bool headlessStateFile) {
  const uint8_t* e;
  if (headlessStateFile) {
    e = reinterpret_cast<const uint8_t*>(buffer);
  } else {
    if (length < sHeaderLength) {
      return;
    }
    if (!loadFileHeader(buffer)) {
      return;
    }
    e = reinterpret_cast<const uint8_t*>(buffer + sHeaderLength);
  }
  const uint8_t* bufferEnd = reinterpret_cast<const uint8_t*>(buffer + length);
  while (e != bufferEnd) {
    pushEvent(*e++);
  }
  Ion::Events::replayFrom(Journal::replayJournal());
}

static inline bool save(FILE* f) {
  if (fwrite(sMagic, sMagicLength, 1, f) != 1) {
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
  Ion::Events::Journal* journal = Journal::logJournal();
  const char* logJournalLanguage = journal->startingLanguage()[0] != 0
                                       ? journal->startingLanguage()
                                       : sWildcardLanguage;
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

void save(const char* filename) {
  FILE* f = fopen(filename, "w");
  if (f == nullptr) {
    return;
  }
  save(f);
  fclose(f);
}

}  // namespace StateFile
}  // namespace Simulator
}  // namespace Ion
