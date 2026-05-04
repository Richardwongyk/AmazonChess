#ifndef PLATFORM_EGEPLATFORM_H
#define PLATFORM_EGEPLATFORM_H

#include "IPlatform.h"

/**
 * EGE (Easy Graphics Engine) implementation of IPlatform.
 *
 * Wraps all EGE/Windows GDI+ calls behind the platform interface.
 * This is the default Windows backend.
 */
class EGEPlatform : public IPlatform {
public:
    EGEPlatform() = default;
    ~EGEPlatform() override;

    // Window
    bool initWindow(int width, int height, const char* title) override;
    void closeWindow() override;
    bool isRunning() override;

    // Drawing
    void clearDevice() override;
    void flushWindow() override;
    void fillRect(int x, int y, int w, int h) override;
    void alphaFillRect(int x, int y, int w, int h) override;
    void drawRect(int x1, int y1, int x2, int y2) override;
    void fillCircle(int cx, int cy, int r) override;
    void drawCircle(int cx, int cy, int r) override;
    void drawLine(int x1, int y1, int x2, int y2) override;
    void fillEllipse(int cx, int cy, int w, int h) override;
    void drawText(int x, int y, const char* text) override;
    int  textWidth(const char* text) override;
    void setFont(int size, const char* faceName) override;
    void setFillColor(Color c) override;
    void setLineColor(Color c) override;
    void setBgColor(Color c) override;
    void setLineStyle(int style, int thickness = 1) override;
    void setTextBgMode(int mode) override;
    void enableAntiAlias(bool on) override;

    // Input
    bool isKeyDown(int keyCode) override;
    bool hasMouseMsg() override;
    MouseEvent getMouseMsg() override;

    // Timing
    void delayFPS(int fps) override;

    // Audio
    bool openMusic(const char* filePath) override;
    void playMusic(bool loop) override;
    void pauseMusic() override;
    void stopMusic() override;
    void closeMusic() override;

private:
    void* musicHandle_ = nullptr;  // opaque: ege::MUSIC*
};

#endif
