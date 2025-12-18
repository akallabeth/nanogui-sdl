/*
    sdlgui/vscrollpanel.cpp -- Adds a vertical scrollbar around a widget
    that is too big to fit into a certain area

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/vscrollpanel.h>
#include <sdlgui/theme.h>
#include <cmath>

NAMESPACE_BEGIN(sdlgui)

VScrollPanel::VScrollPanel(Widget *parent)
    : Widget(parent), mChildPreferredHeight(0), mScroll(0.0f) 
{ }

void VScrollPanel::performLayout(SDL_Renderer *ctx) 
{
    Widget::performLayout(ctx);

    if (mChildren.empty())
        return;
    Widget *child = mChildren[0];
    mChildPreferredHeight = child->preferredSize(ctx).y;
    child->setPosition({ 0, 0 });
    child->setSize({ mSize.x - 12, mChildPreferredHeight });
}

Vector2f VScrollPanel::preferredSize(SDL_Renderer *ctx) const
{
    if (mChildren.empty())
      return{ 0, 0 };
    return mChildren[0]->preferredSize(ctx) + Vector2f(12, 0);
}

bool VScrollPanel::mouseDragEvent(const Vector2f &, const Vector2f &rel,  int, int)
{
    if (mChildren.empty())
        return false;

    float scrollh = height() *
        std::min(1.0f, height() / (float)mChildPreferredHeight);

    mScroll = std::max((float) 0.0f, std::min((float) 1.0f,
                 mScroll + rel.y / (float)(mSize.y - 8 - scrollh)));
    return true;
}

bool VScrollPanel::scrollEvent(const Vector2f &/* p */, const Vector2f &rel)
{
    float scrollAmount = rel.y * (mSize.y / 20.0f);
    float scrollh = height() *
        std::min(1.0f, height() / (float)mChildPreferredHeight);

    mScroll = std::max((float) 0.0f, std::min((float) 1.0f,
            mScroll - scrollAmount / (float)(mSize.y - 8 - scrollh)));
    return true;
}

bool VScrollPanel::mouseButtonEvent(const Vector2f &p, int button, bool down, int modifiers)
{
    if (mChildren.empty())
        return false;
    auto shift =  (mScroll*(mChildPreferredHeight - mSize.y));
    return mChildren[0]->mouseButtonEvent(p - _pos + Vector2f{ 0, shift }, button, down, modifiers);
}

bool VScrollPanel::mouseMotionEvent(const Vector2f &p, const Vector2f &rel, int button, int modifiers)
{
    if (mChildren.empty())
        return false;
    auto shift = (mScroll*(mChildPreferredHeight - mSize.y));
    return mChildren[0]->mouseMotionEvent(p - _pos + Vector2f{ 0, shift }, rel, button, modifiers);
}

void VScrollPanel::draw(SDL_Renderer *renderer) 
{
    if (mChildren.empty())
        return;

    auto child = mChildren[0];
    mChildPreferredHeight = child->preferredSize(nullptr).y;
    float scrollh = height() * std::min(1.0f, height() / (float) mChildPreferredHeight);

    auto ap = getAbsolutePos();
    SDL_FRect brect{ ap.x, ap.y, width(), height() };

    //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    //SDL_RenderRect(renderer, &brect);

    if (child->visible())
    {
      const Vector2f savepos = child->position();
      Vector2f npos = savepos;
      mDOffset = -mScroll*(mChildPreferredHeight - mSize.y);
      npos.y += mDOffset;
      child->setPosition(npos);
      child->draw(renderer);
      child->setPosition(savepos);
    }

    SDL_Color sc = mTheme->mBorderDark.toSdlColor();
    SDL_FRect srect{ ap.x + mSize.x - 12, ap.y + 4, 8, mSize.y - 8 };

    SDL_SetRenderDrawColor(renderer, sc.r, sc.g, sc.b, sc.a);
    SDL_RenderFillRect(renderer, &srect);
      
    SDL_Color ss = mTheme->mBorderLight.toSdlColor();
    SDL_FRect drect{
        std::round(ap.x + mSize.x - 12 + 1),
        std::round(ap.y + 4 + (mSize.y - 8 - scrollh) * mScroll + 1),
        6,
       std::round(scrollh - 1)
    };
    SDL_SetRenderDrawColor(renderer, ss.r, ss.g, ss.b, ss.a);
    SDL_RenderFillRect(renderer, &drect);
}


SDL_FPoint VScrollPanel::getAbsolutePos() const
{
  return Widget::getAbsolutePos();
}

PntFRect VScrollPanel::getAbsoluteCliprect() const
{
  return Widget::getAbsoluteCliprect();
}

float VScrollPanel::getAbsoluteTop() const
{
  return Widget::getAbsoluteTop();
}

NAMESPACE_END(sdlgui)
