#include "LightManager.h"
#include "logger/logger.h"
#include "Settings.h"

LightManager& LightManager::getInstance() {
    static LightManager lightManager;
    return lightManager;
}

LightManager::LightManager() {
    lighting = true;
    //FIXME THIS IS A DIRTY HACK - since we're now binding stuff
    //automatically binding doesn't work is shader is loaded before light is known in js...
    activeLightCount = 0;

    
    /*//FIXME: Support for script to bind and animate lighting
    Light l1;
    l1.setType(LightType::DIRECTIONAL);
    l1.setPosition(0,0,0);
    l1.setDirection(1,-1,0);
    l1.setDiffuse(0.4, 0.4, 0.4, 1);
    l1.setAmbient(0.3, 0.3, 0.3, 1);
    l1.setSpecular(1, 1, 1, 1);
    setLight(0, l1);
    //lights.push_back(l1);*/

    /*Light l2;
    l2.setType(LightType::DIRECTIONAL);
    l2.setPosition(0,0,0);
    l2.setDirection(1,0,0);
    l2.setDiffuse(0.0, 0.0, 0.6, 1);
    l2.setAmbient(0.0, 0.0, 0.0, 1);
    l2.setSpecular(0, 0, 1, 1);
    lights.push_back(l2);*/

    //light[0] = Light(LIGHT_DIRECTIONAL, vec3(0,0,0), vec3(-1,0,0), vec4(0.6, 0.0, 0.0, 1), vec4(0.1, 0.1, 0.1, 1), vec4(1, 0, 0, 1));
    //light[1] = Light(LIGHT_DIRECTIONAL, vec3(0,0,0), vec3(1,0,0), vec4(0.0, 0.0, 0.6, 1), vec4(0.0, 0.0, 0.0, 1), vec4(0, 0, 1, 1));
}

LightManager::~LightManager() {
}

void LightManager::setLighting(bool lighting) {
    this->lighting = lighting;
}

bool LightManager::getLighting() {
    return lighting;
}

void LightManager::setActiveLightCount(unsigned int activeLightCount) {
    /*if (!lighting) {
        loggerError("Lighting not enabled. Can't set active light count.");
        return;
    }*/

    if (Settings::demo.graphics.maxActiveLightCount < activeLightCount) {
        loggerError("Attempted to set more lights than allowed. maxActiveLightCount:%u, activeLightCount:%u", Settings::demo.graphics.maxActiveLightCount, activeLightCount);
        return;
    }

    this->activeLightCount = activeLightCount;
}

unsigned int LightManager::getActiveLightCount() const {
    if (!lighting) {
        return 0;
    }

    return activeLightCount;
}

void LightManager::setLight(unsigned int lightIndex, Light light) {
    if (lightIndex > lights.size()) {
        loggerError("Attempted to set light over currently initialized lights. lightIndex:%u, lights:%u", lightIndex, lights.size());
        return;
    }

    if (lightIndex >= lights.size()) {
        lights.push_back(light);
    } else {
        lights[lightIndex] = light;
    }
}

Light& LightManager::getLight(unsigned int lightIndex) {
    if (lightIndex == lights.size()) {
        setLight(lightIndex, Light());
    } else if (lightIndex > lights.size()) {
        loggerError("Attempted to get light that has not been initialized. lightIndex:%u, lights:%u", lightIndex, lights.size());
        return lights[0];
    } 

    return lights[lightIndex];
}
