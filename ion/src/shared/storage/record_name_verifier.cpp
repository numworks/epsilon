#include <ion/storage/record_name_verifier.h>

namespace Ion {


namespace Storage {

void RecordNameVerifier::registerRestrictiveExtensionWithPrecedence(const char * extension, int precedenceScore) {
  assert(m_numberOfRestrictiveExtensions < k_maxNumberOfRestrictiveExtensions);
  m_restrictiveExtensions[m_numberOfRestrictiveExtensions] = extension;
  m_restrictiveExtensionsPrecedenceScore[m_numberOfRestrictiveExtensions] = precedenceScore;
  m_numberOfRestrictiveExtensions++;
}

void RecordNameVerifier::registerArrayOfReservedNames(const char * const * namePrefixes, const char* extension, int prefixRepetitions, int numberOfElements) {
  assert(m_numberOfReservedNamesArrays < k_maxNumberOfReservedNamesArrays);
  ReservedNamesArray newArray = {namePrefixes, extension, prefixRepetitions, numberOfElements};
  m_reservedNamesArrays[m_numberOfReservedNamesArrays] = newArray;
  m_numberOfReservedNamesArrays++;
}

RecordNameVerifier::OverrideStatus RecordNameVerifier::canOverrideRecordWithNewExtension(Storage::Record previousRecord, const char * newExtension) const {
  if (previousRecord.isNull()) {
    return OverrideStatus::Allowed;
  }
  assert(!previousRecord.hasExtension(newExtension));
  int newPrecedenceScore = precedenceScoreOfExtension(newExtension);
  int previousPrecedenceScore = precedenceScoreOfExtension(previousRecord.name().extension);
  // If at least one is not a restrictive extension, they can coexist.
  if (newPrecedenceScore == -1 || previousPrecedenceScore == -1) {
    return OverrideStatus::CanCoexist;
  }
  Storage::Record::Name previousName = previousRecord.name();
  bool previousIsReservedForExtension = isNameReservedForExtension(previousName.baseName, previousName.baseNameLength, previousRecord.name().extension);
  if (previousIsReservedForExtension) {
    return OverrideStatus::Forbidden;
  }
  bool newIsReservedForExtension = isNameReservedForExtension(previousName.baseName, previousName.baseNameLength, newExtension);
  if (!newIsReservedForExtension && newPrecedenceScore > previousPrecedenceScore) {
      return  OverrideStatus::Forbidden;
  }
  return  OverrideStatus::Allowed;
}

int RecordNameVerifier::precedenceScoreOfExtension(const char * extension) const {
  for (int i = 0 ; i < m_numberOfRestrictiveExtensions ; i++) {
    if (strcmp(extension, m_restrictiveExtensions[i]) == 0) {
      return m_restrictiveExtensionsPrecedenceScore[i];
    }
  }
  return -1;
}

bool RecordNameVerifier::isNameReservedForExtension(const char * name, int nameLength, const char * extension) const {
  for (int i = 0 ; i < m_numberOfReservedNamesArrays ; i++) {
    ReservedNamesArray reservedNamesArray = m_reservedNamesArrays[i];
    if (strcmp(extension, reservedNamesArray.extension) != 0) {
      continue;
    }
    for (int j = 0 ; j < reservedNamesArray.numberOfElements ; j++) {
      // Check if the record has the same base name.
      bool hasSuffixDigit = reservedNamesArray.prefixRepetitions > 0;
      int namePrefixLength = hasSuffixDigit ? nameLength - 1 : nameLength;
      if (strlen(reservedNamesArray.namePrefixes[j]) == namePrefixLength              // Prefixes have same length
          && strncmp(name, reservedNamesArray.namePrefixes[j], namePrefixLength) == 0 // Prefixes are equal
          && (!hasSuffixDigit
            || (name[nameLength - 1] >= '1'                                           // Suffixes are equal
              && name[nameLength - 1] < '1' + reservedNamesArray.prefixRepetitions))) {
        return true;
      }
    }
  }
  return false;
}

}

}
