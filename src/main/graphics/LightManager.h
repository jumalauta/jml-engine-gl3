#ifndef ENGINE_GRAPHICS_LIGHTMANAGER_H_
#define ENGINE_GRAPHICS_LIGHTMANAGER_H_

#include "Light.h"
#include "Settings.h"
#include <vector>

class LightManager {
public:
    static LightManager& getInstance();
    ~LightManager();

    void setLighting(bool lighting);
    bool getLighting();

    void setActiveLightCount(unsigned int activeLightCount);
    unsigned int getActiveLightCount() const;
    void setLight(unsigned int lightIndex, Light light);
    Light& getLight(unsigned int lightIndex);
private:
    explicit LightManager();
    bool lighting;
    unsigned int activeLightCount;
    std::vector<Light> lights;
};

#endif /*ENGINE_GRAPHICS_LIGHTMANAGER_H_*/
