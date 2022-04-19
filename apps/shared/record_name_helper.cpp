#include "record_name_helper.h"

namespace Shared {

bool RecordNameHelper::isNameReservedForAnotherExtension(const char * name, const char * extension) {
  for (int i = 0 ; i < k_reservedExtensionsLength ; i++) {
    ReservedExtension reservedExtension = k_reservedExtensions[i];
    for (int j = 0 ; j < reservedExtension.numberOfElements ; j++) {
      int charIndex = 0;
      // For each reserved name, check if the record has the same base name.
      while (name[charIndex] != '.' && name[charIndex] != 0) {
        if (name[charIndex] != reservedExtension.namePrefixes[j][charIndex]) {
          break;
        }
        charIndex++;
      }
      bool hasSameBaseName;
      if (reservedExtension.prefixRepetitions > 0) {
        // Check if the last char of the name is the suffix-digit
        hasSameBaseName =  (name[charIndex + 1] == '.' || name[charIndex + 1] == 0) && reservedExtension.namePrefixes[j][charIndex] == 0 && name[charIndex] >= '1' && name[charIndex] < '1' + reservedExtension.prefixRepetitions;
      } else {
        hasSameBaseName = (name[charIndex] == '.' || name[charIndex] == 0) && reservedExtension.namePrefixes[j][charIndex] == 0;
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
