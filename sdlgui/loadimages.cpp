#include <sdlgui/common.h>

#include <filesystem>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

NAMESPACE_BEGIN(sdlgui)

ListImages loadImageDirectory(SDL_Renderer* renderer, const std::string &path) 
{
  ListImages result;
    
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
      const auto& p = entry.path();
      if (p.extension() != ".png")
            continue;
        std::string fullName = std::filesystem::canonical(p).native();
        auto tex = IMG_LoadTexture(renderer, fullName.c_str());
        if (!tex)
            throw std::runtime_error("Could not open image data!");
        ImageInfo iminfo;
        iminfo.tex = tex;
        iminfo.path = fullName;
        SDL_GetTextureSize(tex, &iminfo.w, &iminfo.h);
        
        result.push_back(iminfo);
    }
    return result;
}

NAMESPACE_END(sdlgui)
