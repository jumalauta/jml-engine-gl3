#ifndef ENGINE_IO_NETWORKMANAGER_H_
#define ENGINE_IO_NETWORKMANAGER_H_

class NetworkManager {
public:
    static NetworkManager& getInstance();
    NetworkManager();
    bool start();
    bool exit();
};

#endif /*ENGINE_IO_NETWORKMANAGER_H_*/
