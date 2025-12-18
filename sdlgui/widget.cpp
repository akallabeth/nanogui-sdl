/*
    sdlgui/widget.cpp -- Base class of all widgets

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/widget.h>
#include <sdlgui/layout.h>
#include <sdlgui/theme.h>
#include <sdlgui/window.h>
#include <sdlgui/screen.h>
#include <SDL3/SDL.h>

NAMESPACE_BEGIN(sdlgui)

Widget::Widget(Widget *parent)
    : mParent(nullptr), mTheme(nullptr), mLayout(nullptr),
      _pos(Vector2f::Zero()), mSize(Vector2f::Zero()),
      mFixedSize(Vector2f::Zero()), mVisible(true), mEnabled(true),
      mFocused(false), mMouseFocus(false), mTooltip(""), mFontSize(-1.0f),
      mCursor(Cursor::Arrow) 
{
    if (parent)
        parent->addChild(this);
}

Widget::~Widget() 
{
    for (auto child : mChildren) 
    {
        if (child)
            child->decRef();
    }
}

void Widget::setTheme(Theme *theme) 
{
    if (mTheme.get() == theme)
        return;
    mTheme = theme;
    for (auto child : mChildren)
        child->setTheme(theme);
}

int Widget::fontSize() const 
{
    return (mFontSize < 0 && mTheme) 
                      ? mTheme->mStandardFontSize 
                      : mFontSize;
}

Vector2f Widget::preferredSize(SDL_Renderer *ctx) const 
{
    if (mLayout)
        return mLayout->preferredSize(ctx, this);
    else
        return mSize;
}

void Widget::performLayout(SDL_Renderer *ctx) 
{
    if (mLayout) 
    {
        mLayout->performLayout(ctx, this);
    } 
    else 
    {
        for (auto c : mChildren) 
        {
          Vector2f pref = c->preferredSize(ctx), fix = c->fixedSize();
            c->setSize(Vector2f(
                fix[0] ? fix[0] : pref[0],
                fix[1] ? fix[1] : pref[1]
            ));
            c->performLayout(ctx);
        }
    }
}

Widget* Widget::find(const std::string& id, bool inchildren)
{
  if (mId == id)
    return this;

  if (inchildren)
  {
    for (auto* child : mChildren)
    {
      Widget* w = child->find(id, inchildren);
      if (w)
        return w;
    }
  }

  return nullptr;
}

Widget *Widget::findWidget(const Vector2f &p)
{
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) 
    {
        Widget *child = *it;
        if (child->visible() && child->contains(p - _pos))
            return child->findWidget(p - _pos);
    }
    return contains(p) ? this : nullptr;
}

bool Widget::mouseButtonEvent(const Vector2f &p, int button, bool down, int modifiers)
{
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) 
    {
        Widget *child = *it;
        if (child->visible() && child->contains(p - _pos) &&
            child->mouseButtonEvent(p - _pos, button, down, modifiers))
            return true;
    }
    
    if (button == SDL_BUTTON_LEFT && down && !mFocused)
        requestFocus();
    return false;
}

bool Widget::mouseMotionEvent(const Vector2f &p, const Vector2f &rel, int button, int modifiers)
{
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) 
    {
        Widget *child = *it;
        if (!child->visible())
            continue;
        bool contained = child->contains(p - _pos);
        bool prevContained = child->contains(p - _pos - rel);
        if (contained != prevContained)
            child->mouseEnterEvent(p, contained);
        if ((contained || prevContained) &&
            child->mouseMotionEvent(p - _pos, rel, button, modifiers))
            return true;
    }
    return false;
}

bool Widget::scrollEvent(const Vector2f &p, const Vector2f &rel)
{
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) 
    {
        Widget *child = *it;
        if (!child->visible())
            continue;
        if (child->contains(p - _pos) && child->scrollEvent(p - _pos, rel))
            return true;
    }
    return false;
}

bool Widget::mouseDragEvent(const Vector2f &, const Vector2f &, int, int)
{
    return false;
}

bool Widget::mouseEnterEvent(const Vector2f &, bool enter)
{
    mMouseFocus = enter;
    return false;
}

bool Widget::focusEvent(bool focused) 
{
    mFocused = focused;
    return false;
}

bool Widget::keyboardEvent(int, int, bool, uint16_t) 
{
    return false;
}

bool Widget::keyboardCharacterEvent(unsigned int) 
{
    return false;
}

void Widget::addChild(int index, Widget * widget) 
{
    assert(index <= childCount());
    mChildren.insert(mChildren.begin() + index, widget);
    widget->incRef();
    widget->setParent(this);
    widget->setTheme(mTheme);
}

void Widget::addChild(Widget * widget) 
{
    addChild(childCount(), widget);
}

void Widget::removeChild(const Widget *widget) 
{
    mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), widget), mChildren.end());
    widget->decRef();
}

void Widget::removeChild(int index) 
{
    Widget *widget = mChildren[index];
    mChildren.erase(mChildren.begin() + index);
    widget->decRef();
}

int Widget::childIndex(Widget *widget) const 
{
    auto it = std::find(mChildren.begin(), mChildren.end(), widget);
    if (it == mChildren.end())
        return -1;
    return it - mChildren.begin();
}

Window *Widget::window() 
{
    Widget *widget = this;
    while (true) 
    {
        if (!widget)
            throw std::runtime_error(
                "Widget:internal error (could not find parent window)");
        Window *window = dynamic_cast<Window *>(widget);
        if (window)
            return window;
        widget = widget->parent();
    }
}

float Widget::getAbsoluteLeft() const
{
  return mParent
    ? mParent->getAbsoluteLeft() + _pos.x
    : _pos.x;
}

SDL_FPoint Widget::getAbsolutePos() const
{
  if (mParent)
  {
    auto p = mParent->getAbsolutePos();
    return SDL_FPoint{ p.x + _pos.x, p.y + _pos.y };
  }
  else
    return SDL_FPoint{ _pos.x, _pos.y };
}

PntFRect Widget::getAbsoluteCliprect() const
{
  if (mParent)
  {
    auto pclip = mParent->getAbsoluteCliprect();
    auto pp = getAbsolutePos();
    PntFRect mclip{ pp.x, pp.y, pp.x + width(), pp.y + height() };
    if (pclip.x1 < mclip.x1)
      pclip.x1 = mclip.x1;
    if (pclip.y1 < mclip.y1)
      pclip.y1 = mclip.y1;
    if (mclip.x2 < pclip.x2)
      pclip.x2 = mclip.x2;
    if (mclip.y2 < pclip.y2)
      pclip.y2 = mclip.y2;

    return pclip;
  }
  else
  {
    return PntFRect{ _pos.x, _pos.y, _pos.x + width(), _pos.y + height()};
  }
}

float Widget::getAbsoluteTop() const
{
  return mParent
    ? mParent->getAbsoluteTop() + _pos.y
    : _pos.y;
}

void Widget::requestFocus() 
{
    Widget *widget = this;
    while (widget->parent())
        widget = widget->parent();
    ((Screen *) widget)->updateFocus(this);
}

void Widget::draw(SDL_Renderer* renderer)
{
  for (auto child : mChildren)
    if (child->visible())
      child->draw(renderer);
}

NAMESPACE_END(sdlgui)
