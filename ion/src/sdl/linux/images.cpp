#include <SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include "ion/src/external/stb_image.h"

struct Ressource {
  const char *name;
  const uint8_t *start;
  const uint8_t *end;
} ;

extern "C" const uint8_t _binary_ion_src_sdl_assets_background_jpg_start;
extern "C" const uint8_t _binary_ion_src_sdl_assets_background_jpg_end;

const static Ressource sRessources[] = {
  { "background.jpg", &_binary_ion_src_sdl_assets_background_jpg_start, &_binary_ion_src_sdl_assets_background_jpg_end },
  { nullptr, nullptr, nullptr }
};

extern "C" SDL_Texture * IonSDLPlatformLoadImage(SDL_Renderer * renderer, const char * identifier) {
  int width, height, channels;

  const Ressource *ressource = nullptr;
  for (int i = 0; sRessources[i].name; i++) {
    if (strcmp(identifier, sRessources[i].name) == 0) {
      ressource = &sRessources[i];
      break;
    }
  }

  unsigned char *image = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(ressource->start), ressource->end-ressource->start, &width, &height, &channels, STBI_rgb_alpha);

  SDL_Texture * texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ABGR8888,
    SDL_TEXTUREACCESS_STATIC,
    width,
    height
  );

  SDL_UpdateTexture(
    texture,
    NULL,
    image,
    4 * width
  );

  stbi_image_free(image);

  return texture;
}
