#include "Text.h"
#include "Font.h"

#include "Settings.h"

#include "graphics/ShaderProgram.h"
#include "math/TransformationMatrix.h"
#include "io/MemoryManager.h"
#include "logger/logger.h"
#include "graphics/Graphics.h"

Text::Text(std::string text) {
    this->text = text;
    font = NULL;
    setSize(256.0);
    setPosition(0.0, 0.0, 0.0);
    setScale(1.0, 1.0);
    setColor(1.0, 1.0, 1.0, 1.0);
    setPerspective2d(true);
    setAngle(0.0, 0.0, 0.0);
}

void Text::setFont(Font* font) {
    this->font = font;
}

void Text::setSize(double fontSize) {
    this->fontSize = fontSize;
}

void Text::setScale(double x, double y, double z) {
    this->scaleX = x;
    this->scaleY = y;
    this->scaleZ = z;
}

void Text::setColor(double r, double g, double b, double a) {
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
}

void Text::setPosition(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

void Text::setPerspective2d(bool perspective2d) {
    this->perspective2d = perspective2d;
}

void Text::setAngle(double x, double y, double z) {
    degreesX = x;
    degreesY = y;
    degreesZ = z;
}

void Text::setText(std::string text) {
    this->text = text;
}

void Text::draw() {
    if (font == NULL) {
        loggerWarning("Could not initialize font!");
        return;
    }

    Graphics::getInstance().setColor(color);

    TransformationMatrix& transformationMatrix = TransformationMatrix::getInstance();
    if (perspective2d) {
        transformationMatrix.perspective2d();
    } else {
        transformationMatrix.perspective3d();
    }

    transformationMatrix.translate(x, y, z);
    transformationMatrix.rotateX(degreesX);
    transformationMatrix.rotateY(degreesY);
    transformationMatrix.rotateZ(degreesZ);
    transformationMatrix.scale(scaleX*0.1, scaleY*0.1, 1.0); //TODO: scaleZ missing and fix scales anyway...

    ShaderProgram::useCurrentBind();

    font->drawText(0, 0, 0, fontSize, text);
}
