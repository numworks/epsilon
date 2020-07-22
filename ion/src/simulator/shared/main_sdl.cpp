#include "main.h"
#include "display.h"
#include "platform.h"
#include "layout.h"
#include "random.h"

#include <assert.h>
#include <string.h>
#include <ion.h>
#include <stdio.h>
#include <ion/timing.h>
#include <ion/events.h>
#include <ion/storage.h>
#include <SDL.h>
#include <vector>
#include <string>

static bool argument_screen_only = false;
static bool argument_fullscreen = false;
static bool argument_unresizable = false;
static bool argument_volatile = false;
static char* pref_path = nullptr;
static char* file_buffer = nullptr;

static void loadPython(std::vector<const char *>* arguments);
static void savePython();

void Ion::Timing::msleep(uint32_t ms) {
  SDL_Delay(ms);
}

void print_help(char * program_name) {
  printf("Usage: %s [options]\n", program_name);
  printf("Options:\n");
  printf("  -f, --fullscreen          Starts the emulator in fullscreen\n");
  printf("  -s, --screen-only         Disable the keyboard.\n");
  printf("  -v, --volatile            Disable saving and loading python scripts from file.\n");
  printf("  -u, --unresizable         Disable resizing the window.\n");
  printf("  -h, --help                Show this help menu.\n");
}

int event_filter(void* userdata, SDL_Event* e) {
  if (e->type == SDL_APP_TERMINATING || e->type == SDL_APP_WILLENTERBACKGROUND) {
    savePython();
  }

  return 1;
}



int main(int argc, char * argv[]) {
  std::vector<const char *> arguments(argv, argv + argc);

  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i], "-h")==0 || strcmp(argv[i], "--help")==0) {
      print_help(argv[0]);
      return 0;
    } else if(strcmp(argv[i], "-s")==0 || strcmp(argv[i], "--screen-only")==0) {
      argument_screen_only = true;
    } else if(strcmp(argv[i], "-f")==0 || strcmp(argv[i], "--fullscreen")==0) {
      argument_fullscreen = true;
    } else if(strcmp(argv[i], "-u")==0 || strcmp(argv[i], "--unresizable")==0) {
      argument_unresizable = true;
    } else if(strcmp(argv[i], "-v")==0 || strcmp(argv[i], "--volatile")==0) {
      argument_volatile = true;
    }
  }

#if EPSILON_SDL_SCREEN_ONLY
  // Still allow the use of EPSILON_SDL_SCREEN_ONLY.
  argument_screen_only = true;
#endif

  char * language = IonSimulatorGetLanguageCode();
  if (language != nullptr) {
    arguments.push_back("--language");
    arguments.push_back(language);
  }

  if (!argument_volatile) {
    loadPython(&arguments);
    SDL_SetEventFilter(event_filter, NULL);
  }

  Ion::Simulator::Main::init();

  ion_main(arguments.size(), &arguments[0]);

  if (!argument_volatile) {
    savePython();
  }
  
  Ion::Simulator::Main::quit();

  if (file_buffer != nullptr)
    SDL_free(file_buffer);
  if (pref_path != nullptr)
    SDL_free(pref_path);

  return 0;
}

static void loadPython(std::vector<const char *>* arguments) {
  pref_path = SDL_GetPrefPath("io.github.omega", "omega-simulator");
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
      arguments->push_back("--code-wipe");
      arguments->push_back("true");
      
      uint64_t length = SDL_RWseek(save_file, 0, RW_SEEK_END) - sizeof(uint64_t);
      
      SDL_RWseek(save_file, sizeof(uint64_t), RW_SEEK_SET);
      
      file_buffer = (char*) SDL_malloc(length);
      SDL_RWread(save_file, file_buffer, length, 1);
      
      // printf("Length: %ld\n", length);
      size_t i = 0;
      while(i < length) {
        uint16_t size = *(uint16_t*)(file_buffer + i);
        arguments->push_back("--code-script");
        arguments->push_back((char*)(file_buffer + i + sizeof(uint16_t)));
        // printf("Loaded size=%d i=%ld, %s\n", size, i+size, (char*)(file_buffer + i + sizeof(uint16_t)));
        i += size + sizeof(uint16_t);
      }
    }
  }
}

static void savePython() {
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

namespace Ion {
namespace Simulator {
namespace Main {

static SDL_Window * sWindow = nullptr;
static SDL_Renderer * sRenderer = nullptr;
static SDL_Texture * sBackgroundTexture = nullptr;
static bool sNeedsRefresh = false;
static SDL_Rect sScreenRect;

void init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_Log("Could not init video");
    return;
  }

  Random::init();

  uint32_t sdl_window_args = SDL_WINDOW_ALLOW_HIGHDPI | (argument_unresizable ? 0 : SDL_WINDOW_RESIZABLE);

  if (argument_fullscreen) {
    sdl_window_args = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN;
  }

  if (argument_screen_only) {
    sWindow = SDL_CreateWindow(
      "Omega",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      Ion::Display::Width, Ion::Display::Height,
#if EPSILON_SDL_SCREEN_ONLY
      0
#else
      sdl_window_args
#endif
    );
  } else {
    sWindow = SDL_CreateWindow(
      "Omega",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      458, 888,
      sdl_window_args
    );
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  // Try creating a hardware-accelerated renderer.
  sRenderer = SDL_CreateRenderer(sWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!sRenderer) {
    // Try creating a software renderer.
    sRenderer = SDL_CreateRenderer(sWindow, -1, 0);
  }
  assert(sRenderer);

  Display::init(sRenderer);

  // No need to load background in web simulator.
  #ifndef __EMSCRIPTEN__
  if (!argument_screen_only) {
    sBackgroundTexture = IonSimulatorLoadImage(sRenderer, "background.jpg");
  }
  #endif

  relayout();
}

void relayout() {
  int windowWidth = 0;
  int windowHeight = 0;
  SDL_GetWindowSize(sWindow, &windowWidth, &windowHeight);
  SDL_RenderSetLogicalSize(sRenderer, windowWidth, windowHeight);

  if (argument_screen_only) {
    // Keep original aspect ration in screen_only mode.
    float scale = (float)(Ion::Display::Width) / (float)(Ion::Display::Height);
    if ((float)(windowHeight) * scale > float(windowWidth)) {
      sScreenRect.w = windowWidth;
      sScreenRect.h = (int)((float)(windowWidth) / scale);
    } else {
      sScreenRect.w = (int)((float)(windowHeight) * scale);
      sScreenRect.h = windowHeight;
    }

    sScreenRect.x = (windowWidth - sScreenRect.w) / 2;
    sScreenRect.y = (windowHeight - sScreenRect.h) / 2;
  } else {
    Layout::recompute(windowWidth, windowHeight);
  }

  setNeedsRefresh();
}

void setNeedsRefresh() {
  sNeedsRefresh = true;
}

void refresh() {
  if (!sNeedsRefresh) {
    return;
  }

  if (argument_screen_only) {
    Display::draw(sRenderer, &sScreenRect);
  } else {
    SDL_Rect screenRect;
    Layout::getScreenRect(&screenRect);
    SDL_Rect backgroundRect;
    Layout::getBackgroundRect(&backgroundRect);

    SDL_SetRenderDrawColor(sRenderer, 194, 194, 194, 255);
    SDL_RenderClear(sRenderer);
    SDL_RenderCopy(sRenderer, sBackgroundTexture, nullptr, &backgroundRect);
    Display::draw(sRenderer, &screenRect);
  }

  SDL_RenderPresent(sRenderer);
  sNeedsRefresh = false;

  IonSimulatorCallbackDidRefresh();
}

void quit() {
  SDL_DestroyWindow(sWindow);
  SDL_Quit();
}

}
}
}
