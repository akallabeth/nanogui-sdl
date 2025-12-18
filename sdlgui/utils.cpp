#include "utils.h"

bool utils::copy_to_texture(SDL_Texture *tex, int w, int h,
                            unsigned char *data) {
    int pitch = 0;
    uint8_t *pixels = nullptr;
    auto ok = SDL_LockTexture(tex, nullptr, (void **)&pixels, &pitch);
    if (ok) {
        if ((sizeof(Uint32) * w) != pitch) {
            SDL_Log("pitch %d != %zu * %d", pitch, sizeof(Uint32), w);
        }
        memcpy(pixels, data, pitch * h);
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        SDL_UnlockTexture(tex);
    }
    return ok;
}
