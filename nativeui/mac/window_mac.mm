// Copyright 2016 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the license that can be found in the
// LICENSE file.

#include "nativeui/window.h"

#import <Cocoa/Cocoa.h>

#include "ui/gfx/mac/coordinate_conversion.h"

namespace nu {

namespace {

gfx::Rect ContentToWindowBounds(NSWindow* window, const gfx::Rect& bounds) {
  gfx::Rect window_bounds([window frameRectForContentRect:bounds.ToCGRect()]);
  int frame_height = window_bounds.height() - bounds.height();
  window_bounds.set_y(window_bounds.y() - frame_height);
  return window_bounds;
}

gfx::Rect WindowToContentBounds(NSWindow* window, const gfx::Rect& bounds) {
  gfx::Rect content_bounds([window contentRectForFrameRect:bounds.ToCGRect()]);
  int frame_height = bounds.height() - content_bounds.height();
  content_bounds.set_y(content_bounds.y() + frame_height);
  return content_bounds;
}

}  // namespace

Window::~Window() {
  [window_ release];
}

void Window::PlatformInit(const Options& options) {
  NSUInteger styleMask = NSTitledWindowMask | NSMiniaturizableWindowMask |
                         NSClosableWindowMask | NSResizableWindowMask |
                         NSTexturedBackgroundWindowMask;
  window_ = [[NSWindow alloc]
      initWithContentRect:gfx::ScreenRectToNSRect(options.bounds)
                styleMask:styleMask
                  backing:NSBackingStoreBuffered
                    defer:YES];
}

void Window::PlatformSetContentView(Container* container) {
  [window_ setContentView:container->view()];
  container->Layout();
}

void Window::SetContentBounds(const gfx::Rect& bounds) {
  SetBounds(ContentToWindowBounds(window_, bounds));
}

gfx::Rect Window::GetContentBounds() const {
  return WindowToContentBounds(window_, GetBounds());
}

void Window::SetBounds(const gfx::Rect& bounds) {
  [window_ setFrame:gfx::ScreenRectToNSRect(bounds) display:YES animate:NO];
}

gfx::Rect Window::GetBounds() const {
  return gfx::ScreenRectFromNSRect(NSRectToCGRect([window_ frame]));
}

void Window::SetVisible(bool visible) {
  if (visible)
    [window_ orderFrontRegardless];
  else
    [window_ orderOut:nil];
}

bool Window::IsVisible() const {
  return [window_ isVisible];
}

}  // namespace nu