#include <ion/storage/record_name_verifier.h>

namespace Ion {

namespace Storage {

void RecordNameVerifier::registerRestrictiveExtensionWithPrecedence(
    const char* extension, int precedenceScore) {
  m_restrictiveExtensions.push(extension);
  m_restrictiveExtensionsPrecedenceScore.push(precedenceScore);
}

void RecordNameVerifier::registerArrayOfReservedNames(
    std::span<const char* const> namePrefixes, const char* extension,
    int prefixRepetitions) {
  m_reservedNamesArrays.push(
      ReservedNamesArray{namePrefixes, extension, prefixRepetitions});
}

RecordNameVerifier::OverrideStatus
RecordNameVerifier::canOverrideRecordWithNewExtension(
    Storage::Record previousRecord, const char* newExtension) const {
  if (previousRecord.isNull()) {
    return OverrideStatus::Allowed;
  }
  assert(!previousRecord.hasExtension(newExtension));
  int newPrecedenceScore = precedenceScoreOfExtension(newExtension);
  int previousPrecedenceScore =
      precedenceScoreOfExtension(previousRecord.name().extension);
  // If at least one is not a restrictive extension, they can coexist.
  if (newPrecedenceScore == -1 || previousPrecedenceScore == -1) {
    return OverrideStatus::CanCoexist;
  }
  Storage::Record::Name previousName = previousRecord.name();
  bool previousIsReservedForExtension = isNameReservedForExtension(
      previousName.baseName, previousName.baseNameLength,
      previousRecord.name().extension);
  if (previousIsReservedForExtension) {
    return OverrideStatus::Forbidden;
  }
  bool newIsReservedForExtension = isNameReservedForExtension(
      previousName.baseName, previousName.baseNameLength, newExtension);
  if (!newIsReservedForExtension &&
      newPrecedenceScore > previousPrecedenceScore) {
    return OverrideStatus::Forbidden;
  }
  return OverrideStatus::Allowed;
}

int RecordNameVerifier::precedenceScoreOfExtension(
    const char* extension) const {
  assert(m_restrictiveExtensions.size() ==
         m_restrictiveExtensionsPrecedenceScore.size());
  for (size_t i = 0; i < m_restrictiveExtensions.size(); i++) {
    if (strcmp(extension, m_restrictiveExtensions[i]) == 0) {
      return m_restrictiveExtensionsPrecedenceScore[i];
    }
  }
  return -1;
}

bool RecordNameVerifier::isNameReservedForExtension(
    const char* name, int nameLength, const char* extension) const {
  for (const ReservedNamesArray& reservedNamesArray : m_reservedNamesArrays) {
    if (strcmp(extension, reservedNamesArray.extension) != 0) {
      continue;
    }
    for (const char* reservedNamePrefix : reservedNamesArray.namePrefixes) {
      // Check if the record has the same base name.
      bool hasSuffixDigit = reservedNamesArray.prefixRepetitions > 0;
      int namePrefixLength = hasSuffixDigit ? nameLength - 1 : nameLength;
      if (strlen(reservedNamePrefix) ==
              static_cast<size_t>(
                  namePrefixLength)  // Prefixes have same length
          && strncmp(name, reservedNamePrefix,
                     namePrefixLength) == 0  // Prefixes are equal
          && (!hasSuffixDigit ||
              (name[nameLength - 1] >= '1'  // Suffixes are equal
               && name[nameLength - 1] <
                      '1' + reservedNamesArray.prefixRepetitions))) {
        return true;
      }
    }
  }
  return false;
}

}  // namespace Storage

}  // namespace Ion
