#include "EGEPlatform.h"

// All EGE/Windows dependencies are confined to this file.
#include <graphics.h>   // EGE: initgraph, closegraph, drawing, input, etc.
#include <ege.h>        // for ege::MUSIC
#include <cstdlib>

// ====== Window ======

bool EGEPlatform::initWindow(int width, int height, const char* title) {
    initgraph(width, height, INIT_RENDERMANUAL);
    setcaption(title);
    return is_run();
}

void EGEPlatform::closeWindow() {
    closegraph();
}

bool EGEPlatform::isRunning() {
    return is_run();
}

// ====== Drawing ======

void EGEPlatform::clearDevice() {
    // setbkcolor must have been called first
    cleardevice();
}

void EGEPlatform::flushWindow() {
    flushwindow();
}

void EGEPlatform::fillRect(int x, int y, int w, int h) {
    bar(x, y, x + w, y + h);
}

void EGEPlatform::alphaFillRect(int x, int y, int w, int h) {
    ege_fillrect((float)x, (float)y, (float)w, (float)h);
}

void EGEPlatform::drawRect(int x1, int y1, int x2, int y2) {
    rectangle(x1, y1, x2, y2);
}

void EGEPlatform::fillCircle(int cx, int cy, int r) {
    solidcircle(cx, cy, r);
}

void EGEPlatform::drawCircle(int cx, int cy, int r) {
    circle(cx, cy, r);
}

void EGEPlatform::drawLine(int x1, int y1, int x2, int y2) {
    line(x1, y1, x2, y2);
}

void EGEPlatform::fillEllipse(int cx, int cy, int w, int h) {
    fillellipse(cx, cy, w, h);
}

void EGEPlatform::drawText(int x, int y, const char* text) {
    outtextxy(x, y, text);
}

int EGEPlatform::textWidth(const char* text) {
    return textwidth(text);
}

void EGEPlatform::setFont(int size, const char* faceName) {
    setfont(size, 0, faceName);
}

void EGEPlatform::setFillColor(Color c) {
    setfillcolor(c);
}

void EGEPlatform::setLineColor(Color c) {
    setcolor(c);
}

void EGEPlatform::setBgColor(Color c) {
    setbkcolor(c);
}

void EGEPlatform::setLineStyle(int style, int thickness) {
    setlinestyle(style, 0, thickness);
}

void EGEPlatform::setTextBgMode(int mode) {
    setbkmode(mode);
}

void EGEPlatform::enableAntiAlias(bool on) {
    ege_enable_aa(on);
}

// ====== Input ======

bool EGEPlatform::isKeyDown(int keyCode) {
    return keystate(static_cast<key_msg_e>(keyCode));
}

bool EGEPlatform::hasMouseMsg() {
    return mousemsg();
}

MouseEvent EGEPlatform::getMouseMsg() {
    mouse_msg msg = getmouse();
    MouseEvent evt;
    evt.x = msg.x;
    evt.y = msg.y;
    evt.left_down = (msg.msg == mouse_msg_down && msg.is_left());
    return evt;
}

// ====== Timing ======

void EGEPlatform::delayFPS(int fps) {
    delay_fps(fps);
}

// ====== Audio ======
// Uses ege::MUSIC via opaque pointer to avoid exposing EGE in headers.

EGEPlatform::~EGEPlatform() {
    if (musicHandle_) {
        delete static_cast<ege::MUSIC*>(musicHandle_);
        musicHandle_ = nullptr;
    }
}

bool EGEPlatform::openMusic(const char* filePath) {
    if (!musicHandle_)
        musicHandle_ = new ege::MUSIC();
    auto* m = static_cast<ege::MUSIC*>(musicHandle_);
    return m->OpenFile(filePath) == 0;
}

void EGEPlatform::playMusic(bool loop) {
    if (!musicHandle_) return;
    auto* m = static_cast<ege::MUSIC*>(musicHandle_);
    if (loop)
        m->RepeatPlay();
    else
        m->Play();
}

void EGEPlatform::pauseMusic() {
    if (!musicHandle_) return;
    static_cast<ege::MUSIC*>(musicHandle_)->Pause();
}

void EGEPlatform::stopMusic() {
    if (!musicHandle_) return;
    static_cast<ege::MUSIC*>(musicHandle_)->Stop();
}

void EGEPlatform::closeMusic() {
    if (!musicHandle_) return;
    auto* m = static_cast<ege::MUSIC*>(musicHandle_);
    m->Stop();
    m->Close();
}
