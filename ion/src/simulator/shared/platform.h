#ifndef ION_SIMULATOR_PLATFORM_H
#define ION_SIMULATOR_PLATFORM_H

#include <SDL.h>
#include <kandinsky/color.h>

namespace Ion {
namespace Simulator {
namespace Platform {

SDL_Texture * loadImage(SDL_Renderer * renderer, const char * identifier);
const char * languageCode();
void copyImageToClipboard(const KDColor * pixels, int width, int height);
#if ION_SIMULATOR_FILES
const char * filePathForReading(const char * extension);
const char * filePathForWriting(const char * extension);
void saveImage(const KDColor * pixels, int width, int height, const char * path);
const char * filePathInTempDir(const char * filename);
const char * cacheWindowPositionFilePath();
#endif

}
}
}

#endif
