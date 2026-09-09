#ifndef FACEHANDLER_H
#define FACEHANDLER_H

#include <Arduino.h>
#include <lvgl.h>

#define FACE_HAPPY "happy"
#define FACE_SAD "sad"
#define FACE_ANGRY "angry"
#define FACE_EXCITED "excited"
#define FACE_SLEEPY "sleepy"
#define FACE_NORMAL "normal"

class FaceHandler {
public:
    static void init();
    static void setFace(const char* expression);
    static const char* getFace();
    static void setSkin(int stage);
    static void setEnabled(bool en);
    static void setAnimationSpeed(int speed);
    static void setColor(lv_color_t color);
    static void setMood(const char* mood);
};

#endif
