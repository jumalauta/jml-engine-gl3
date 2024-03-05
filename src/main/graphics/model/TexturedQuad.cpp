#include "TexturedQuad.h"

#include "Settings.h"
#include "logger/logger.h"


#include "graphics/Graphics.h"
#include "graphics/Image.h"
#include "graphics/Texture.h"
#include "graphics/Fbo.h"

#include "math/TransformationMatrix.h"

#define VERTEX_ATTRIB 0
#define UV_ATTRIB 1
#define NORMAL_ATTRIB 2

TexturedQuad* TexturedQuad::newInstance(double width, double height) {
    TexturedQuad* texturedQuad = new TexturedQuad(width, height);

    loggerTrace("TexturedQuad instantiated! texture:0x%p, width:%.0f, height:%.0f", texturedQuad->getTexture(), texturedQuad->getWidth(), texturedQuad->getHeight());

    return texturedQuad;
}

TexturedQuad* TexturedQuad::newInstance(Fbo* fbo, Texture *fboTexture) {
    if (fbo == NULL || fbo->getColorTexture() == NULL) {
        loggerWarning("Invalid FBO state. fbo:0x%p, fboColorTexture:0x%p", fbo, fbo->getColorTexture());
        return NULL;
    }

    //NB. FBO should be size of full window dimensions...
    double width = static_cast<double>(Settings::demo.graphics.canvasWidth) * (fbo->getWidth()/Settings::window.screenAreaWidth);
    double height = static_cast<double>(Settings::demo.graphics.canvasHeight) * (fbo->getHeight()/Settings::window.screenAreaHeight);

    TexturedQuad* texturedQuad = new TexturedQuad(width, height);
    texturedQuad->setParent(static_cast<void*>(fbo));

    //determine texture, if not given
    if (!fboTexture) {
        fboTexture = fbo->getColorTexture();
        if (!fboTexture) {
            fboTexture = fbo->getDepthTexture();
        }
    }

    if (!fboTexture) {
        loggerError("Could not find texture for the FBO! fbo:'%s' (0x%p)", fbo->getName().c_str(), fbo);
    }

    texturedQuad->setTexture(fboTexture);
    texturedQuad->setCanvasDimensions(width, height);
    texturedQuad->setAlignment(Alignment::CENTERED);

    loggerTrace("TexturedQuad instantiated! texture:0x%p, width:%.0f, height:%.0f, fbo:%s", texturedQuad->getTexture(), texturedQuad->getWidth(), texturedQuad->getHeight(), fbo->getName().c_str());

    return texturedQuad;
}

TexturedQuad* TexturedQuad::newInstance(Image* image) {
    if (image == NULL || image->getTexture() == NULL) {
        loggerWarning("Invalid Image state. image:0x%p, imageTexture:0x%p", image, image->getTexture());
        return NULL;
    }

    TexturedQuad *texturedQuad = new TexturedQuad(image->getWidth(), image->getHeight());
    texturedQuad->setParent(static_cast<void*>(image));
    texturedQuad->setTexture(image->getTexture());

    loggerTrace("TexturedQuad instantiated! texture:0x%p, width:%.0f, height:%.0f, image:%s", texturedQuad->getTexture(), texturedQuad->getWidth(), texturedQuad->getHeight(), image->getFilePath().c_str());

    return texturedQuad;
}

TexturedQuad::TexturedQuad(double width, double height) : Mesh() {
    parent = NULL;

    setTexture(NULL);
    setDimensions(width, height);

    perspective2d = true;
    setPerspective2d(true);

    setPosition(0.0, 0.0, 0.0);
    setAngle(0.0, 0.0, 0.0);
    setScale(1.0, 1.0, 1.0);

    setColor(1,1,1,1);

    setCanvasDimensions(static_cast<double>(Settings::demo.graphics.canvasWidth), static_cast<double>(Settings::demo.graphics.canvasHeight));
    setAlignment(Alignment::CENTERED);
}

TexturedQuad::~TexturedQuad() {
    deinit();
}

void TexturedQuad::setParent(void* parent) {
    this->parent = parent;
}

void* TexturedQuad::getParent() {
    return parent;
}

void TexturedQuad::setTexture(Texture *texture, unsigned int unit) {
    if (!getMaterial()) {
        setMaterial(new Material());
    }

    getMaterial()->setTexture(texture, unit);
}

void TexturedQuad::setCanvasDimensions(double width, double height) {
    this->canvasWidth = width;
    this->canvasHeight = height;
}

void TexturedQuad::setDimensions(double width, double height) {
    this->width = width;
    this->height = height;
}

void TexturedQuad::setPerspective2d(bool perspective2d) {
    bool perspectiveChanged = false;
    if (perspective2d != this->perspective2d) {
        perspectiveChanged = true;
    }

    this->perspective2d = perspective2d;

    if (perspectiveChanged) {
        // Vertex data needs to be redone
        loggerTrace("Perspective changed, reinitializing vertex data. texture:0x%p", getTexture());
        if (!init()) {
            loggerError("Perspective changing failed! texture:0x%p", getTexture());
        }
    }
}

void TexturedQuad::setColor(double r, double g, double b, double a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

void TexturedQuad::setAlignment(Alignment alignment) {
    if (! perspective2d) {
        return;
    }

    switch(alignment) {
        case Alignment::CENTERED:
            setPosition(canvasWidth/2.0, canvasHeight/2.0, 0.0);
            break;
        default:
            loggerFatal("Alignment not implemented");
            break;
    }
}

void TexturedQuad::setPosition(double x, double y, double z) {
    Mesh::setTranslate(x, y, z);

    this->x = x;
    this->y = y;
    this->z = z;
}

void TexturedQuad::setAngle(double x, double y, double z) {
    Mesh::setRotate(x, y, z);

    this->angleX = x;
    this->angleY = y;
    this->angleZ = z;
}

void TexturedQuad::setScale(double x, double y, double z) {
    Mesh::setScale(x, y, z);

    this->scaleX = x;
    this->scaleY = y;
    this->scaleZ = z;
}

bool TexturedQuad::init() {
    PROFILER_BLOCK("TexturedQuad::init");

    //Perspective setup needs to be done in a dynamic way...
    float w = width;
    float h = height;
    if (!perspective2d) {
        w = w / h;
        h = 1.0;
    }

    setFaceDrawType(FaceType::TRIANGLE_STRIP);
    clear();

    // Triangle strip is drawn counter clock wise:
    // 1) v0 - v1 - v2
    // 2) v3 - v2 - v1
    // v0 - v2
    //  | / |
    // v1 - v3
    addVertex(-w/2,  h/2);
    addVertex(-w/2, -h/2);
    addVertex( w/2,  h/2);
    addVertex( w/2, -h/2);

    addTexCoord(0.0f, 1.0f);
    addTexCoord(0.0f, 0.0f);
    addTexCoord(1.0f, 1.0f);
    addTexCoord(1.0f, 0.0f);

    addNormal(0.0f, 0.0f, 1.0f);
    addNormal(0.0f, 0.0f, 1.0f);
    addNormal(0.0f, 0.0f, 1.0f);
    addNormal(0.0f, 0.0f, 1.0f);

    if (!generate()) {
        return false;
    }

    return true;
}

bool TexturedQuad::deinit() {
    if (getMaterial()) {
        delete getMaterial();
        setMaterial(NULL);
    }

    free();

    return true;
}

void TexturedQuad::draw() {
    PROFILER_BLOCK("TexturedQuad::draw");

    TransformationMatrix& transformationMatrix = TransformationMatrix::getInstance();
    transformationMatrix.push();
    if (perspective2d) {
        transformationMatrix.perspective2d(canvasWidth, canvasHeight);
    } else {
        transformationMatrix.perspective3d();
    }

    /*
    // Handled in Mesh::draw()
    transformationMatrix.translate(x, y, z);
    transformationMatrix.scale(scaleX, scaleY, scaleZ);
    transformationMatrix.rotateX(angleX);
    transformationMatrix.rotateY(angleY);
    transformationMatrix.rotateZ(angleZ);
    */

    Graphics::getInstance().setColor(Color(r, g, b, a));

    Mesh::draw();

    transformationMatrix.pop();
}

Texture* TexturedQuad::getTexture(unsigned int unit) {
    Material* material = getMaterial();
    if (!material) {
        return NULL;
    }

    return material->getTexture(unit);
}

double TexturedQuad::getWidth() {
    return width;
}

double TexturedQuad::getHeight() {
    return height;
}
