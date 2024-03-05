#ifndef ENGINE_IO_SOCKET_H_
#define ENGINE_IO_SOCKET_H_

#ifdef _WIN32

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // Windows XP
#endif
#include <winsock2.h>
#include <Ws2tcpip.h>

#else // !_WIN32

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#endif

#include <string>

enum class SocketType {
    TCP,
    UDP
};

class Socket {
public:
    Socket();
    ~Socket();
    bool establishConnection();
    bool closeConnection();
    bool sendData(const void *buf, size_t len, int flags = 0);
    bool receiveData(void *buf, size_t len, int flags = 0);

    void setHost(std::string host);
    const std::string& getHost() const;
    void setPort(unsigned short port);
    unsigned short getPort() const;
    void setType(SocketType type);
    const SocketType& getType() const;

private:
    static bool init();
    static bool deinit();
    static int constructedSockets;

    SOCKET sock;
    SocketType type;
    std::string host;
    unsigned short port;
};

#endif /*ENGINE_IO_SOCKET_H_*/
