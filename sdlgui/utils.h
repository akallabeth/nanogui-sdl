#pragma once

#include <SDL3/SDL.h>

namespace utils {
bool copy_to_texture(SDL_Texture *tex, int w, int h, unsigned char *data);
}
