#ifndef FACEHANDLER_H
#define FACEHANDLER_H

#include <Arduino.h>
#include <lvgl.h>

#define FACE_NORMAL   "normal"
#define FACE_HAPPY    "happy"
#define FACE_SAD      "sad"
#define FACE_ANGRY    "angry"
#define FACE_EXCITED  "excited"
#define FACE_SLEEPY   "sleepy"
#define FACE_COOL     "cool"
#define FACE_SCANNING "scanning"
#define FACE_HUNGRY   "hungry"
#define FACE_LOVE     "love"

class FaceHandler {
public:
    static void init();
    static lv_obj_t* createFaceWidget(lv_obj_t *parent, int width = 310, int height = 135);
    static void setFace(const char* expression);
    static const char* getFace();
    static const char* getFaceAscii();
    static void setSkin(int stage);
    static void setEnabled(bool en);
    static void setAnimationSpeed(int speed);
    static void setColor(lv_color_t color);
    static void setMood(const char* mood);
    static void blink();
    static void wink();
    static void triggerBounce();
    static void triggerLevelUp(int level);
    static void triggerEvolution(int stage);
    static void tick();
    static void cycleExpression();
};

#endif
