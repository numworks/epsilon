#include <ion.h>
#include <ion/events.h>
#include <ion/keyboard/layout_events.h>
#include <stdio.h>
#include <string.h>

#include <string>

#include "events.h"
#include "journal.h"

namespace Ion {
namespace Simulator {
namespace StateFile {

constexpr static const char* sMagic = "NWSF";
constexpr static size_t sMagicLength = 4;
constexpr static size_t sVersionLength = 8;
constexpr static const char* sWildcardVersion = "**.**.**";
constexpr static size_t sProjectIdLength = 1;
#ifndef NWS_PROJECT_ID
#error NWS_PROJECT_ID must be defined
#endif
constexpr static uint8_t sProjectId = NWS_PROJECT_ID;
constexpr static size_t sLanguageLength =
    Ion::Events::Journal::k_languageSize - 1;
constexpr static const char* sWildcardLanguage = "**";
constexpr static size_t sHeaderLength =
    sMagicLength + sVersionLength + sProjectIdLength + sLanguageLength;

/* File format:
 *   "NWSF" : Magic
 * + "XXXXXXXX" : Software version
 * + 0x01 or 0x02 : Project ID (1 = epsilon, 2 = scandium)
 * + "XX" : Language code (en, fr, nl, pt, it, de, or es)
 * + EVENTS...
 */

static inline bool loadHeaderAfterMagic(const char* afterMagic) {
  const char* version = afterMagic;
  const char* projectId = version + sVersionLength;
  const char* language = projectId + sProjectIdLength;

  if (strncmp(version, softwareVersion(), sVersionLength) != 0 &&
      strncmp(version, sWildcardVersion, sVersionLength) != 0) {
    return false;
  }
  if (*projectId != sProjectId) {
    return false;
  }
  if (strncmp(language, sWildcardLanguage, sLanguageLength) != 0) {
    Journal::replayJournal()->setStartingLanguage(language);
  }
  return true;
}

static inline Ion::Events::Event reconstructEvent(uint8_t c) {
  Ion::Events::Event e = Ion::Events::Event(c);
  if (!Events::isDefined(static_cast<uint8_t>(
          e))) {  // If not defined, fall back on a normal key event.
    e = Ion::Events::Event(static_cast<uint8_t>(
        Keyboard::ValidKeys[c % Ion::Keyboard::NumberOfValidKeys]));
  }
  assert(Events::isDefined(static_cast<uint8_t>(e)));

  if (e == Ion::Events::Termination || e == Ion::Events::TimerFire ||
      e == Ion::Events::DeprecatedExternalText) {
    return Ion::Events::None;
  }
  return e;
}

void pushEventFromMemory(uint8_t c, const uint8_t** ptr,
                         const uint8_t* bufferEnd) {
  Ion::Events::Event e = reconstructEvent(c);
  if (e == Ion::Events::None) {
    return;
  } else if (e == Ion::Events::ExternalText) {
    // Read and load external text into sharedExternalTextBuffer
    char* buffer = Ion::Events::sharedExternalTextBuffer();
    int i = 0;
    char ch;
    while (true) {
      ch = **ptr;
      buffer[i++] = ch;
      (*ptr)++;
      if (ch == 0 || *ptr >= bufferEnd) {
        break;
      }
      if (i >=
          static_cast<int>(Ion::Events::sharedExternalTextBufferSize) - 1) {
        // Buffer is full, truncate and stop reading
        buffer[Ion::Events::sharedExternalTextBufferSize - 1] = 0;
        break;
      }
    }
    if (buffer[0] == 0 || buffer[0] == EOF) {
      // Do not push an empty ExternalText event
      return;
    }
  }
  Journal::replayJournal()->pushEvent(e);
}

#if ION_SIMULATOR_FILES

constexpr static const char* sTextMagic = "NWST";

static inline void pushEventFromFile(uint8_t c, FILE* f) {
  Ion::Events::Event e = reconstructEvent(c);
  if (e == Ion::Events::None) {
    return;
  } else if (e == Ion::Events::ExternalText) {
    // Read and load external text into sharedExternalTextBuffer
    char* buffer = Ion::Events::sharedExternalTextBuffer();
    int i = 0;
    char ch;
    while (true) {
      ch = getc(f);
      buffer[i++] = ch;
      if (ch == 0 || ch == EOF) {
        break;
      }
      if (i >=
          static_cast<int>(Ion::Events::sharedExternalTextBufferSize) - 1) {
        // Buffer is full, truncate and stop reading
        buffer[Ion::Events::sharedExternalTextBufferSize - 1] = 0;
        break;
      }
    }
    if (buffer[0] == 0 || buffer[0] == EOF) {
      // Do not push an empty ExternalText event
      return;
    }
  }
  Journal::replayJournal()->pushEvent(e);
}

static inline void unescapeText(char* text) {
  // Unescape: \\ -> \, \n -> newline. Text is modified in place.
  const char* src = text;
  char* dst = text;
  while (*src) {
    if (*src == '\\' && *(src + 1) != '\0') {
      if (*(src + 1) == '\\') {
        *dst++ = '\\';
        src += 2;
      } else if (*(src + 1) == 'n') {
        *dst++ = '\n';
        src += 2;
      } else {
        *dst++ = *src++;
      }
    } else {
      *dst++ = *src++;
    }
  }
  *dst = 0;
}

static inline void stripNewline(char* s) {
  char* nl = strchr(s, '\n');
  if (nl) {
    *nl = 0;
  }
}

static bool loadTextFile(FILE* f) {
  // Consume rest of magic line (the newline after "NWST")
  int ch;
  while ((ch = getc(f)) != '\n' && ch != EOF) {
  }

  /* The longuest possible line is an external text where every
   * character is escaped. */
  static char line[sizeof("ExternalText ") +
                   2 * Ion::Events::sharedExternalTextBufferSize];

  // Version
  if (fgets(line, sizeof(line), f) == nullptr) {
    return false;
  }
  stripNewline(line);
  if (strncmp(line, softwareVersion(), sVersionLength) != 0 &&
      strncmp(line, sWildcardVersion, sVersionLength) != 0) {
    return false;
  }

  // Project ID
  if (fgets(line, sizeof(line), f) == nullptr) {
    return false;
  }
  if (atoi(line) != sProjectId) {
    return false;
  }

  // Language
  if (fgets(line, sizeof(line), f) == nullptr) {
    return false;
  }
  stripNewline(line);
  if (strncmp(line, sWildcardLanguage, sLanguageLength) != 0) {
    Journal::replayJournal()->setStartingLanguage(line);
  }

  // Events
  while (fgets(line, sizeof(line), f) != nullptr) {
    stripNewline(line);
    if (line[0] == 0) {
      continue;
    }

    // Split on first space (for ExternalText)
    char* space = strchr(line, ' ');
    if (space != nullptr) {
      *space = 0;
    }

    // Name to ID lookup
    bool found = false;
    Ion::Events::Event e = Ion::Events::None;
    for (int i = 0; i < UINT8_MAX; i++) {
      if (Ion::Events::s_nameForEvent[i][0] != 0 &&
          strcmp(Ion::Events::s_nameForEvent[i], line) == 0) {
        e = Ion::Events::Event(i);
        found = true;
        break;
      }
    }
    if (!found) {
      continue;
    }
    if (e == Ion::Events::None || e == Ion::Events::Termination ||
        e == Ion::Events::TimerFire ||
        e == Ion::Events::DeprecatedExternalText) {
      continue;
    }

    if (e == Ion::Events::ExternalText && space != nullptr) {
      strlcpy(Ion::Events::sharedExternalTextBuffer(), space + 1,
              Ion::Events::sharedExternalTextBufferSize);
      unescapeText(Ion::Events::sharedExternalTextBuffer());
    }
    Journal::replayJournal()->pushEvent(e);
  }

  Ion::Events::replayFrom(Journal::replayJournal());
  return true;
}

static inline bool loadFile(FILE* f, bool headlessStateFile) {
  if (!headlessStateFile) {
    char magic[sMagicLength];
    if (fread(magic, sMagicLength, 1, f) != 1) {
      return false;
    }
    if (memcmp(magic, sTextMagic, sMagicLength) == 0) {
      return loadTextFile(f);
    }
    if (memcmp(magic, sMagic, sMagicLength) != 0) {
      return false;
    }
    // Binary header: read remaining bytes after magic
    constexpr size_t sRemainingHeaderLength =
        sVersionLength + sProjectIdLength + sLanguageLength;
    char header[sRemainingHeaderLength + 1];
    header[sRemainingHeaderLength] = 0;
    if (fread(header, sRemainingHeaderLength, 1, f) != 1) {
      return false;
    }
    if (!loadHeaderAfterMagic(header)) {
      return false;
    }
  }
  // Binary events
  int c = 0;
  while ((c = getc(f)) != EOF) {
    pushEventFromFile(c, f);
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

#endif  // ION_SIMULATOR_FILES

void loadMemory(const char* buffer, size_t length, bool headlessStateFile) {
  const uint8_t* e;
  if (headlessStateFile) {
    e = reinterpret_cast<const uint8_t*>(buffer);
  } else {
    if (length < sHeaderLength) {
      return;
    }
    if (memcmp(buffer, sMagic, sMagicLength) != 0) {
      return;
    }
    if (!loadHeaderAfterMagic(buffer + sMagicLength)) {
      return;
    }
    e = reinterpret_cast<const uint8_t*>(buffer + sHeaderLength);
  }
  const uint8_t* bufferEnd = reinterpret_cast<const uint8_t*>(buffer + length);
  while (e < bufferEnd) {
    uint8_t ch = *e;
    e++;
    pushEventFromMemory(ch, &e, bufferEnd);
  }
  Ion::Events::replayFrom(Journal::replayJournal());
}

#if ION_SIMULATOR_FILES

static inline std::string escapeText(const char* text) {
  // Escape: \ -> \\, newline -> \n
  std::string result;
  for (const char* p = text; *p; p++) {
    if (*p == '\\') {
      result += "\\\\";
    } else if (*p == '\n') {
      result += "\\n";
    } else {
      result += *p;
    }
  }
  return result;
}

static inline bool save(FILE* f, bool toText) {
  const char* version =
#if ASSERTIONS
      sWildcardVersion;
#else
      softwareVersion();
#endif

  // Magic + version + project ID
  if (toText) {
    fprintf(f, "%s\n", sTextMagic);
    fwrite(version, sVersionLength, 1, f);
    fprintf(f, "\n%d\n", sProjectId);
  } else {
    if (fwrite(sMagic, sMagicLength, 1, f) != 1 ||
        fwrite(version, sVersionLength, 1, f) != 1 ||
        fwrite(&sProjectId, sProjectIdLength, 1, f) != 1) {
      return false;
    }
  }

  // Language
  Ion::Events::Journal* journal = Journal::logJournal();
  const char* lang = journal->startingLanguage()[0] != 0
                         ? journal->startingLanguage()
                         : sWildcardLanguage;
  if (toText) {
    fprintf(f, "%s\n", lang);
  } else {
    if (fwrite(lang, sLanguageLength, 1, f) != 1) {
      return false;
    }
  }

  // Events
  while (!journal->isEmpty()) {
    Ion::Events::Event e = journal->popEvent();
    uint8_t code = static_cast<uint8_t>(e);
    if (toText) {
      const char* name = Ion::Events::s_nameForEvent[code];
      if (name[0] == 0) {
        continue;
      }
      fprintf(f, "%s", name);
    } else {
      if (fwrite(&code, 1, 1, f) != 1) {
        return false;
      }
    }
    if (e == Ion::Events::ExternalText) {
      if (toText) {
        fprintf(f, " %s",
                escapeText(Ion::Events::sharedExternalTextBuffer()).c_str());
      } else {
        const char* text = Ion::Events::sharedExternalTextBuffer();
        if (fwrite(text, strlen(text) + 1, 1, f) != 1) {
          return false;
        }
      }
    }
    if (toText) {
      fprintf(f, "\n");
    }
  }
  return true;
}

void save(const char* filename, bool toText) {
  FILE* f = fopen(filename, "w");
  if (f == nullptr) {
    return;
  }
  save(f, toText);
  fclose(f);
}

#endif  // ION_SIMULATOR_FILES

}  // namespace StateFile
}  // namespace Simulator
}  // namespace Ion
