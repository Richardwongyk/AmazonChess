#ifndef PLATFORM_IPLATFORM_H
#define PLATFORM_IPLATFORM_H

#include "KeyCode.h"

// 32-bit ARGB color (matching EGE's internal format)
using Color = unsigned int;

struct MouseEvent {
    int x, y;
    bool left_down;  // true = left button pressed down
};

// Line style constants
enum LineStyle {
    LINE_SOLID  = 0,
    LINE_DASHED = 1,
};

// Text background mode
enum TextBgMode {
    TEXT_BG_OPAQUE       = 0,
    TEXT_BG_TRANSPARENT  = 1,
};

/**
 * Abstract platform interface.
 *
 * Wraps all OS-specific operations needed by the game:
 * window management, 2D drawing, keyboard/mouse input, audio, and timing.
 *
 * Currently implemented by EGEPlatform (Windows GDI+/EGE).
 * To port to another graphics library (SDL2, SFML, raylib, etc.),
 * implement this interface and link the corresponding library.
 */
class IPlatform {
public:
    virtual ~IPlatform() = default;

    // ====== Window ======
    virtual bool initWindow(int width, int height, const char* title) = 0;
    virtual void closeWindow() = 0;
    virtual bool isRunning() = 0;

    // ====== Drawing ======
    virtual void clearDevice() = 0;
    virtual void flushWindow() = 0;

    // Filled rectangle (x, y, w, h). Opaque fill — maps to EGE's bar().
    virtual void fillRect(int x, int y, int w, int h) = 0;
    // Alpha-blended filled rectangle (x, y, w, h). Maps to EGE's ege_fillrect().
    virtual void alphaFillRect(int x, int y, int w, int h) = 0;
    // Rectangle outline (used for board cell borders)
    virtual void drawRect(int x1, int y1, int x2, int y2) = 0;
    // Filled circle
    virtual void fillCircle(int cx, int cy, int r) = 0;
    // Circle outline
    virtual void drawCircle(int cx, int cy, int r) = 0;
    // Line
    virtual void drawLine(int x1, int y1, int x2, int y2) = 0;
    // Filled ellipse
    virtual void fillEllipse(int cx, int cy, int w, int h) = 0;

    // Text
    virtual void drawText(int x, int y, const char* text) = 0;
    virtual int  textWidth(const char* text) = 0;
    virtual void setFont(int size, const char* faceName) = 0;

    // Colors & style
    virtual void setFillColor(Color c) = 0;
    virtual void setLineColor(Color c) = 0;
    virtual void setBgColor(Color c) = 0;
    virtual void setLineStyle(int style, int thickness = 1) = 0;
    virtual void setTextBgMode(int mode) = 0;
    virtual void enableAntiAlias(bool on) = 0;

    // ====== Input ======
    virtual bool isKeyDown(int keyCode) = 0;
    virtual bool hasMouseMsg() = 0;
    virtual MouseEvent getMouseMsg() = 0;

    // ====== Timing ======
    virtual void delayFPS(int fps) = 0;

    // ====== Audio ======
    virtual bool openMusic(const char* filePath) = 0;
    virtual void playMusic(bool loop) = 0;
    virtual void pauseMusic() = 0;
    virtual void stopMusic() = 0;
    virtual void closeMusic() = 0;
};

#endif
