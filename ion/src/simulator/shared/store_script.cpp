#include "store_script.h"
#include <string.h>
#include <ion.h>
#include <stdio.h>
#include <ion/timing.h>
#include <ion/events.h>
#include <ion/storage.h>
#include <SDL.h>
#include <string>

namespace Ion {
namespace Simulator {
namespace StoreScript {

static char* pref_path = nullptr;
static char* file_buffer = nullptr;

void loadPython(Args * arguments) {
  pref_path = SDL_GetPrefPath("io.github.upsilon", "upsilon-simulator");
  std::string path(pref_path);
  printf("Loading from %s\n", (path + "python.dat").c_str());
  
  SDL_RWops* save_file = SDL_RWFromFile((path + "python.dat").c_str(), "rb");
  
  if (save_file != NULL) {
    // Calculate checksum
    uint64_t checksum = 0;
    uint64_t calc_checksum = 0;
    
    SDL_RWread(save_file, &checksum, sizeof(uint64_t), 1);
    
    uint8_t curr_check = 0;
    
    while(SDL_RWread(save_file, &curr_check, sizeof(uint8_t), 1)) {
      calc_checksum += curr_check;
    }
    
    if (checksum == calc_checksum) {
      arguments->push("--code-wipe", "true");
      
      uint64_t length = SDL_RWseek(save_file, 0, RW_SEEK_END) - sizeof(uint64_t);
      
      SDL_RWseek(save_file, sizeof(uint64_t), RW_SEEK_SET);
      
      file_buffer = (char*) SDL_malloc(length);
      SDL_RWread(save_file, file_buffer, length, 1);
      
      // printf("Length: %ld\n", length);
      size_t i = 0;
      while(i < length) {
        uint16_t size = *(uint16_t*)(file_buffer + i);
        arguments->push("--code-script", (char*)(file_buffer + i + sizeof(uint16_t)));
        // printf("Loaded size=%d i=%ld, %s\n", size, i+size, (char*)(file_buffer + i + sizeof(uint16_t)));
        i += size + sizeof(uint16_t);
      }
    }
  }
}

void savePython() {
  std::string path(pref_path);

  printf("Saving to %s\n", (path + "python.dat").c_str());
  
  SDL_RWops* save_file = SDL_RWFromFile((path + "python.dat").c_str(), "wb+");

  if (save_file != NULL) {
    
    // Placeholder for checksum
    uint64_t checksum = 0;
    SDL_RWwrite(save_file, &checksum, sizeof(uint64_t), 1);
    
    uint16_t num = (uint16_t) Ion::Storage::sharedStorage()->numberOfRecordsWithExtension("py");
    
    // Write all checksums
    for(uint16_t i = 0; i < num; i++) {
      Ion::Storage::Record record = Ion::Storage::sharedStorage()->recordWithExtensionAtIndex("py", i);
      
      const char* record_name = record.fullName();
      uint16_t record_name_len = strlen(record_name);
      
      Ion::Storage::Record::Data record_data = record.value();
      
      uint16_t total_length = record_name_len + record_data.size;
      
      SDL_RWwrite(save_file, &total_length, sizeof(uint16_t), 1);
      
      SDL_RWwrite(save_file, record_name, record_name_len, 1);
      SDL_RWwrite(save_file, ":", 1, 1);
      // Remove import status, keep trailing \x00
      SDL_RWwrite(save_file, ((char*)record_data.buffer + 1), record_data.size - 1, 1);
    }
    
    // Compute and write checksum
    
    SDL_RWseek(save_file, sizeof(uint64_t), RW_SEEK_SET);
    uint8_t curr_check = 0;
    
    while(SDL_RWread(save_file, &curr_check, sizeof(uint8_t), 1)) {
      checksum += curr_check;
    }
    
    SDL_RWseek(save_file, 0, RW_SEEK_SET);
    SDL_RWwrite(save_file, &checksum, sizeof(uint64_t), 1);
    
    SDL_RWclose(save_file);
  }
}

}
}
}
