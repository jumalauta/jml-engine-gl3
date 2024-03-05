#ifndef ENGINE_GRAPHICS_TEXT_H_
#define ENGINE_GRAPHICS_TEXT_H_

#include <string>

#include "datatypes.h"

class Font;

class Text {
public:
    Text(std::string text);
    void setFont(Font* font);
    void setSize(double fontSize);
    void setScale(double x, double y, double z = 1.0);
    void setPosition(double x, double y, double z);
    void setPerspective2d(bool perspective2d);
    void setAngle(double x, double y, double z);
    void setColor(double r, double g, double b, double a);
    void setText(std::string text);
    void draw();
private:
    Font* font;
    Color color;
    double fontSize;
    double x, y, z;
    double scaleX, scaleY, scaleZ;
    double r, g, b, a;
    double degreesX, degreesY, degreesZ;
    bool perspective2d;
    std::string text;
};

#endif /*ENGINE_GRAPHICS_TEXT_H_*/
