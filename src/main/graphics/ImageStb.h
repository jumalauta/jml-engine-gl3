#ifndef ENGINE_GRAPHICS_IMAGESTB_H_
#define ENGINE_GRAPHICS_IMAGESTB_H_

#include "Image.h"

class ImageStb : public Image {
public:
    explicit ImageStb(std::string filePath);
    ~ImageStb();
    bool load(bool rollback=false);
    bool isSupported();
};

#endif /*ENGINE_GRAPHICS_IMAGESTB_H_*/
