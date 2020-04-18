#include "platform.h"

#include <3ds.h>

const char* IonSimulatorGetLanguageCode() {
    u8 lang = 0;
    
    CFGU_GetSystemLanguage(&lang);
    
    switch(lang) {
        case CFG_LANGUAGE_FR:
            return "fr";
        case CFG_LANGUAGE_ES:
            return "es";
        case CFG_LANGUAGE_DE:
            return "de";
        case CFG_LANGUAGE_PT:
            return "pt";
            
        // en fr es de pt hu
        default:
            return "en";
    }
}

