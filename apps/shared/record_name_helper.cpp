#include "record_name_helper.h"

namespace Shared {

size_t RecordNameHelper::precedenceScoreOfExtension(const char * extension) {
  for (int i = 0 ; i < k_numberOfCompetingExtensions ; i++) {
    if (strcmp(extension, k_competingExtensions[i]) == 0) {
      return k_competingExtensionsPrecedenceScore[i];
    }
  }
  return -1;
}

Ion::RecordNameHelper::OverrideStatus RecordNameHelper::shouldRecordBeOverridenWithNewExtension(Ion::Storage::Record previousRecord, const char * newExtension) {
  if (previousRecord.isNull()) {
    return Ion::RecordNameHelper::OverrideStatus::Allowed;
  }
  if (previousRecord.hasExtension(newExtension)) {
    return competingExtensionsOverrideThemselves() ? Ion::RecordNameHelper::OverrideStatus::Allowed : Ion::RecordNameHelper::OverrideStatus::Forbidden;
  }
  int newPrecedenceScore = precedenceScoreOfExtension(newExtension);
  int previousPrecedenceScore = precedenceScoreOfExtension(previousRecord.name().extension);
  // If at least one is not a competing extension, they can coexist.
  if (newPrecedenceScore == -1 || previousPrecedenceScore == -1) {
    return Ion::RecordNameHelper::OverrideStatus::CanCoexist;
  }
  Ion::Storage::Record::Name previousName = previousRecord.name();
  bool newIsReservedForAnotherExtension = isNameReservedForAnotherExtension(previousName.baseName, previousName.baseNameLength, newExtension);
  bool previousIsReservedForAnotherExtension = isNameReservedForAnotherExtension(previousName.baseName, previousName.baseNameLength, previousRecord.name().extension);
  if (newIsReservedForAnotherExtension && !previousIsReservedForAnotherExtension) {
    // Name is reserved for previousExtension.
    return Ion::RecordNameHelper::OverrideStatus::Forbidden;
  }
  if (!newIsReservedForAnotherExtension && previousIsReservedForAnotherExtension) {
    // Name is reserved for new extension.
    return Ion::RecordNameHelper::OverrideStatus::Allowed;
  }
  if (newPrecedenceScore > previousPrecedenceScore) {
    // Previous extension has precedence over new one.
    return Ion::RecordNameHelper::OverrideStatus::Forbidden;
  }
  return  Ion::RecordNameHelper::OverrideStatus::Allowed;
}

bool RecordNameHelper::isNameReservedForAnotherExtension(const char * name, int nameLength, const char * extension) {
  for (int i = 0 ; i < k_reservedExtensionsLength ; i++) {
    ReservedExtension reservedExtension = k_reservedExtensions[i];
    for (int j = 0 ; j < reservedExtension.numberOfElements ; j++) {
      int charIndex = 0;
      // For each reserved name, check if the record has the same base name.
      while (charIndex < nameLength) {
        if (name[charIndex] != reservedExtension.namePrefixes[j][charIndex]) {
          break;
        }
        charIndex++;
      }
      bool hasSameBaseName;
      if (reservedExtension.prefixRepetitions > 0) {
        // Check if the last char of the name is the suffix-digit
        hasSameBaseName = charIndex == nameLength - 1 && charIndex == strlen(reservedExtension.namePrefixes[j]) && name[charIndex] >= '1' && name[charIndex] < '1' + reservedExtension.prefixRepetitions;
      } else {
        hasSameBaseName = charIndex == nameLength && charIndex == strlen(reservedExtension.namePrefixes[j]);
      }
      // If it has the same base name but not the same extension, return true
      if (hasSameBaseName && strcmp(extension, reservedExtension.extension) != 0) {
        return true;
      }
    }
  }
  return false;
}

}
