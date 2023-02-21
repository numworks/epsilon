#ifndef CODE_SCRIPT_H
#define CODE_SCRIPT_H

#include <ion.h>

namespace Code {

/* Record: | Size |  Name |            Body                |
 * Script: |      |       | Status |       Content         | */

class Script : public Ion::Storage::Record {
 public:
  constexpr static int k_defaultScriptNameMaxSize = 6 + 2 + 1;
  /* 6 = strlen("script")
   * 2 = max length of integers between 1 and 99
   * 1 = null-terminating char */

  static bool DefaultName(char buffer[], size_t bufferSize);
  static bool NameCompliant(const char *name);
  static ErrorStatus Create(const char *name, const char *content);

  // Status accessors
  bool fetchedFromConsole() const { return status()->fetchedFromConsole(); }
  void setFetchedFromConsole(bool v) { status()->setFetchedFromConsole(v); }
  bool fetchedForVariableBox() const {
    return status()->fetchedForVariableBox();
  }
  void setFetchedForVariableBox(bool v) {
    status()->setFetchedForVariableBox(v);
  }
  bool autoImportation() const { return status()->autoImportation(); }
  void toggleAutoImportation() {
    status()->setAutoImportation(!status()->autoImportation());
  }

  Script(Ion::Storage::Record r = Ion::Storage::Record()) : Record(r) {}
  const char *content() const {
    return static_cast<const char *>(value().buffer) + sizeof(Status);
  }
  size_t contentSize() const { return value().size - sizeof(Status); }
  size_t usedSize() const { return contentSize() - (strlen(content()) + 1); };

 private:
  // Default script names are chosen between script1 and script99
  constexpr static int k_maxNumberOfDefaultScriptNames = 99;

  class Status {
   public:
    Status() : m_status(0) { setAutoImportation(true); }

    /* FetchFromConsole is set if this script's content has been fetched from
     * the console, so we can retrieve the correct variables afterwards in the
     * variable box. */
    bool fetchedFromConsole() const { return getBit(k_fetchedFromConsoleBit); }
    void setFetchedFromConsole(bool v) { setBit(k_fetchedFromConsoleBit, v); }

    /* FetchedForVariableBox is used to prevent circular importation problems,
     * such as ScriptA importing ScriptB then importing ScriptA. Once we get the
     * variables from a script to put them in the variable box, we switch the
     * bit to 1 and won't reload it afterwards. */
    bool fetchedForVariableBox() const {
      return getBit(k_fetchedForVariableBoxBit);
    }
    void setFetchedForVariableBox(bool v) {
      setBit(k_fetchedForVariableBoxBit, v);
    }

    /* AutoImportation is set if the script should be auto imported when the
     * console opens. */
    bool autoImportation() const { return getBit(k_autoImportationBit); }
    void setAutoImportation(bool v) { setBit(k_autoImportationBit, v); }

   private:
    constexpr static uint8_t k_autoImportationBit = 0;
    constexpr static uint8_t k_fetchedForVariableBoxBit = 6;
    constexpr static uint8_t k_fetchedFromConsoleBit = 7;
    void setBit(uint8_t i, bool value) {
      m_status = (m_status & ~(1 << i)) | (value << i);
    }
    bool getBit(uint8_t i) const { return (m_status >> i) & 1; }
    uint8_t m_status;
  };
  static_assert(sizeof(Status) == 1, "Unexpected status size");

  Status *status() const {
    return const_cast<Status *>(
        reinterpret_cast<const Status *>(value().buffer));
  }
};

}  // namespace Code

#endif
