/*
    sdlgui/theme.h -- Storage class for basic theme-related properties

    The text box widget was contributed by Christian Schueller.

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <sdlgui/common.h>
#include <mutex>

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_FRect;

NAMESPACE_BEGIN(sdlgui)

struct Texture
{
  SDL_Texture* tex = nullptr;
  SDL_FRect rrect;
  bool dirty = false;

  inline int w() const { return rrect.w; }
  inline int h() const { return rrect.h; }
};

void SDL_RenderTexture(SDL_Renderer* renderer, Texture& tex, const Vector2f& pos);
/**
 * \class Theme theme.h sdlgui/theme.h
 *
 * \brief Storage class for basic theme-related properties.
 */

class  Theme : public Object 
{
public:
    Theme(SDL_Renderer *ctx);

    /* Spacing-related parameters */
    int mStandardFontSize;
    int mButtonFontSize;
    int mTextBoxFontSize;
    float mWindowCornerRadius;
    float mWindowHeaderHeight;
    float mWindowDropShadowSize;
    float mButtonCornerRadius;
    float mTabBorderWidth;
    float mTabInnerMargin;
    float mTabMinButtonWidth;
    float mTabMaxButtonWidth;
    float mTabControlWidth;
    float mTabButtonHorizontalPadding;
    float mTabButtonVerticalPadding;

    std::mutex loadMutex;

    /* Generic colors */
    Color mDropShadow;
    Color mTransparent;
    Color mBorderDark;
    Color mBorderLight;
    Color mBorderMedium;
    Color mTextColor;
    Color mDisabledTextColor;
    Color mTextColorShadow;
    Color mIconColor;

    /* Button colors */
    Color mButtonGradientTopFocused;
    Color mButtonGradientBotFocused;
    Color mButtonGradientTopUnfocused;
    Color mButtonGradientBotUnfocused;
    Color mButtonGradientTopPushed;
    Color mButtonGradientBotPushed;

    /* Window colors */
    Color mWindowFillUnfocused;
    Color mWindowFillFocused;
    Color mWindowTitleUnfocused;
    Color mWindowTitleFocused;

    /* Slider coloes */
    Color mSliderKnobOuter;
    Color mSliderKnobInner;

    Color mWindowHeaderGradientTop;
    Color mWindowHeaderGradientBot;
    Color mWindowHeaderSepTop;
    Color mWindowHeaderSepBot;

    Color mWindowPopup;
    Color mWindowPopupTransparent;

    void getTexAndRect(SDL_Renderer *renderer, int x, int y, const std::string& text,
      const std::string& fontname, size_t ptsize, SDL_Texture **texture, SDL_FRect *rect, SDL_Color *textColor);

    void getTexAndRectUtf8(SDL_Renderer *renderer, int x, int y, const std::string& text,
      const std::string& fontname, size_t ptsize, SDL_Texture **texture, SDL_FRect *rect, SDL_Color *textColor);

    std::string breakText(SDL_Renderer* renderer, const std::string& string, const std::string& fontname, int ptsize,
                       float breakRowWidth);

    int getTextWidth(const std::string& fontname, size_t ptsize, const std::string& text);
    int getUtf8Width(const std::string& fontname, size_t ptsize, const std::string& text);
    int getTextBounds(const std::string& fontname, size_t ptsize, const std::string& text, int *w, int *h);
    int getUtf8Bounds(const std::string& fontname, size_t ptsize, const std::string& text, int *w, int *h);

    void getTexAndRectUtf8(SDL_Renderer *renderer, Texture& tx, int x, int y, const std::string& text,
                           const std::string& fontname, size_t ptsize, const Color& textColor);
};

NAMESPACE_END(sdlgui)
