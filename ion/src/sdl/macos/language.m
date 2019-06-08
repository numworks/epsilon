#include "../shared/platform.h"

#include <Foundation/Foundation.h>

char * IonSDLPlatformGetLanguageCode() {
  static char buffer[4] = {0};
  if (buffer[0] == 0) {
    NSString * preferredLanguage = [[NSLocale preferredLanguages] firstObject];
    NSString * languageName = [[NSLocale componentsFromLocaleIdentifier:preferredLanguage] objectForKey:@"kCFLocaleLanguageCodeKey"];
    const char * name = [languageName UTF8String];
    if (strlen(name) > 3) {
      return NULL;
    }
    strlcpy(buffer, name, sizeof(buffer));
  }
  return buffer;
}
