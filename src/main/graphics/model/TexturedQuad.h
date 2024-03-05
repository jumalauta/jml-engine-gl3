#ifndef ENGINE_GRAPHICS_TEXTUREDQUAD_H_
#define ENGINE_GRAPHICS_TEXTUREDQUAD_H_

#include "graphics/datatypes.h"
#include "Mesh.h"

enum class Alignment {
    CENTERED = 1,
    HORIZONTAL,
    VERTICAL
};

class Texture;
class Image;
class Fbo;

class TexturedQuad : public Mesh {
public:
    static TexturedQuad* newInstance(double width, double height);
    static TexturedQuad* newInstance(Fbo* fbo, Texture *fboTexture=NULL);
    static TexturedQuad* newInstance(Image* image);

    ~TexturedQuad();

    void setParent(void* parent);
    void* getParent();

    void setTexture(Texture *texture, unsigned int unit = 0);
    void setCanvasDimensions(double width, double height);
    void setDimensions(double width, double height);

    void setPerspective2d(bool perspective2d);
    void setColor(double r, double g, double b, double a);
    void setAlignment(Alignment alignment);
    //FIXME: naming conventions...
    void setPosition(double x, double y, double z);
    void setAngle(double x, double y, double z);
    void setScale(double x, double y, double z);

    bool init();
    bool deinit();
    void draw();

    Texture* getTexture(unsigned int unit = 0);
    double getWidth();
    double getHeight();

private:
    TexturedQuad(double width, double height);

    void *parent;

    bool perspective2d;

    double width, height;

    double x, y, z;
    double angleX, angleY, angleZ;
    double scaleX, scaleY, scaleZ;
    double r,g,b,a;

    double canvasWidth, canvasHeight;
};

#endif /*ENGINE_GRAPHICS_TEXTUREDQUAD_H_*/
