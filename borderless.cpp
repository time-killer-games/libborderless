/*

 MIT License

 Copyright © 2021 Samuel Venable
 Copyright © 2021 babyjeans <twitter.com/babyj3ans>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

*/

#if defined(_WIN32)
#include <map>
#include <windows.h>
#define EXPORTED_FUNCTION extern "C" __declspec(dllexport)
std::map<HWND, int> dwid;
std::map<HWND, int> dhgt;
std::map<HWND, bool> fxd;
std::map<HWND, bool> mxm;
#else
#include <cstdint>
#include <X11/Xlib.h>
#define EXPORTED_FUNCTION extern "C" __attribute__((visibility("default")))
#endif
EXPORTED_FUNCTION double window_set_showborder(void *window, double showborder) {
  #if defined(_WIN32)
  HWND w = (HWND)window;
  RECT rc, rw;
  GetClientRect(w, &rc);
  GetWindowRect(w, &rw);
  if (!showborder && (GetWindowLongPtr(w, GWL_STYLE) & WS_OVERLAPPEDWINDOW) &&
    (GetWindowLongPtr(w, GWL_STYLE) & WS_POPUP) != WS_POPUP) {
    if (dwid.find(w) != dwid.end() && dhgt.find(w) != dhgt.end() && fxd.find(w) != fxd.end()) {
      dwid.insert(std::make_pair(w, (rw.right - rw.left) - rc.right));
      dhgt.insert(std::make_pair(w, (rw.bottom - rw.top) - rc.bottom));
      fxd.insert(std::make_pair(w, (GetWindowLongPtr(w, GWL_STYLE) & WS_SIZEBOX) != WS_SIZEBOX));
      fxd.insert(std::make_pair(w, (GetWindowLongPtr(w, GWL_STYLE) & WS_MAXIMIZEBOX) != WS_MAXIMIZEBOX));
    } else {
      dwid[w] = (rw.right - rw.left) - rc.right;
      dhgt[w] = (rw.bottom - rw.top) - rc.bottom;
      fxd[w]  = ((GetWindowLongPtr(w, GWL_STYLE) & WS_SIZEBOX) != WS_SIZEBOX);
      mxm[w]  = ((GetWindowLongPtr(w, GWL_STYLE) & WS_MAXIMIZEBOX) != WS_MAXIMIZEBOX);
    }
    if (!IsZoomed(w)) {
      MoveWindow(w, rw.left, rw.top, rc.right, rc.bottom, true);
    }
    SetWindowLongPtr(w, GWL_STYLE, (GetWindowLongPtr(w, GWL_STYLE) | WS_POPUP) & ~WS_OVERLAPPEDWINDOW);
  } else if ((GetWindowLongPtr(w, GWL_STYLE) & WS_OVERLAPPEDWINDOW) != WS_OVERLAPPEDWINDOW &&
    (GetWindowLongPtr(w, GWL_STYLE) & WS_POPUP)) {
    SetWindowLongPtr(w, GWL_STYLE, (GetWindowLongPtr(w, GWL_STYLE) | WS_OVERLAPPEDWINDOW) & ~WS_POPUP);
    if (fxd[w]) {
      SetWindowLongPtr(w, GWL_STYLE, GetWindowLongPtr(w, GWL_STYLE) & ~WS_SIZEBOX);
    } else {
      SetWindowLongPtr(w, GWL_STYLE, GetWindowLongPtr(w, GWL_STYLE) | WS_SIZEBOX);
    }
    if (mxm[w]) {
      SetWindowLongPtr(w, GWL_STYLE, GetWindowLongPtr(w, GWL_STYLE) & ~WS_MAXIMIZEBOX);
    } else {
      SetWindowLongPtr(w, GWL_STYLE, GetWindowLongPtr(w, GWL_STYLE) | WS_MAXIMIZEBOX);
    }
    MoveWindow(w, rw.left, rw.top, (rw.right - rw.left) + dwid[w], (rw.bottom - rw.top) + dhgt[w], true);
  }
  #else
  typedef struct {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long inputMode;
    unsigned long status;
  } Hints;
  Display *d = XOpenDisplay(nullptr);
  Window w = (Window)(std::intptr_t)window;
  Hints hints;
  Atom property = XInternAtom(d, "_MOTIF_WM_HINTS", false);
  hints.flags = 2;
  hints.decorations = (unsigned long)showborder;
  XChangeProperty(d, w, property, property, 32, PropModeReplace, (unsigned char *)&hints, 5);
  XCloseDisplay(d);
  #endif
  return 0;
}
