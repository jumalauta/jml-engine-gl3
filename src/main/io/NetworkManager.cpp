#include "NetworkManager.h"
#include "Curl.h"
#include "Settings.h"

NetworkManager& NetworkManager::getInstance() {
    static NetworkManager networkManager;
    return networkManager;
}

NetworkManager::NetworkManager() {
}

bool NetworkManager::start() {
    if (!Curl::globalInit()) {
        return false;
    }

    return true;
}

bool NetworkManager::exit() {
    if (!Curl::globalExit()) {
        return false;
    }

    return true;
}
