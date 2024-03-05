#include "Socket.h"
#include "logger/logger.h"

int Socket::constructedSockets = 0;

bool Socket::init()
{
    if (constructedSockets > 0) {
        constructedSockets++;
        return true; // NOP
    }
#ifdef _WIN32
    WSADATA wsaData;
    int ret = WSAStartup(MAKEWORD(1,1), &wsaData);
    if (ret == 0) {
        constructedSockets++;
        return true;
    } else {
        loggerWarning("Socket initialization startup failed: %d", ret);
        return false;
    }
#else
    constructedSockets++;
    return true;
#endif
}

bool Socket::deinit()
{
    if (constructedSockets == 0) {
        return true; // NOP
    }

    constructedSockets--;
    if (constructedSockets > 0) {
        return true; // NOP
    }
    constructedSockets = 0;
#ifdef _WIN32
    int ret = WSACleanup();
    if (ret == 0) {
        return true;
    } else {
        loggerWarning("Socket deinitialization cleanup failed: %d", ret);
        return false;
    }
#else
    return true;
#endif
}

Socket::Socket() {
    setHost("127.0.0.1");
    setPort(0);
    setType(SocketType::TCP);
    sock = INVALID_SOCKET;
}

Socket::~Socket() {
    deinit();
}

void Socket::setHost(std::string host) {
    this->host = host;
}
const std::string& Socket::getHost() const {
    return host;
}

void Socket::setPort(unsigned short port) {
    this->port = port;
}
unsigned short Socket::getPort() const {
    return port;
}

void Socket::setType(SocketType type) {
    this->type = type;
}
const SocketType& Socket::getType() const {
    return type;
}

bool Socket::establishConnection()
{
    if (!init()) {
        return false;
    }

    std::string strPort = std::to_string(port);
    struct addrinfo *addr;
    int ret = getaddrinfo(host.c_str(), strPort.c_str(), 0, &addr);
    if (ret != 0) {
        loggerWarning("Could not establish socket connection! host:%s, port:%s, type:%s, error:%d",
            host.c_str(), strPort.c_str(), type == SocketType::TCP ? "TCP" : "UDP", ret);
        return false;
    }

    int sockType = SOCK_STREAM;
    switch(type) {
        case SocketType::TCP:
            sockType = SOCK_STREAM;
            break;
        case SocketType::UDP:
            sockType = SOCK_DGRAM;
            break;
        default:
            loggerError("Socket type not recognized! host:%s, port:%s", host.c_str(), strPort.c_str());
            return false;
    }

    int connectRet = -1;
    struct addrinfo* curr = NULL;
    for (curr = addr; curr; curr = curr->ai_next) {
        sock = socket(curr->ai_family, sockType, 0);
        if (sock == INVALID_SOCKET) {
            continue;
        }

        connectRet = connect(sock, curr->ai_addr, (int)curr->ai_addrlen);
        if (connectRet == 0) {
            break;
        }

        if (!closeConnection()) {
            loggerDebug("Could not close socket connection. host:%s, port:%s, type:%s, socket:%d",
              host.c_str(), strPort.c_str(), type == SocketType::TCP ? "TCP" : "UDP", sock);
        }
    }

    freeaddrinfo(addr);

    if (sock == INVALID_SOCKET || curr == NULL) {
        loggerWarning("Could not establish socket connection! host:%s, port:%s, type:%s, connect:%d",
          host.c_str(), strPort.c_str(), type == SocketType::TCP ? "TCP" : "UDP", connectRet);
        return false;
    }

    loggerInfo("Established socket connection. host:%s, port:%s, type:%s, socket:%d, connect:%d",
      host.c_str(), strPort.c_str(), type == SocketType::TCP ? "TCP" : "UDP", sock, connectRet);

    return true;
}

bool Socket::closeConnection()
{
    int status = 0;

#ifdef _WIN32
    status = shutdown(sock, SD_BOTH);
    if (status == 0) {
        status = closesocket(sock);
    }
#else
    status = shutdown(sock, SHUT_RDWR);
    if (status == 0) {
        status = close(sock);
    }
#endif

    sock = INVALID_SOCKET;

    if (status != 0) {
        return false;
    }

    return true;
}

bool Socket::sendData(const void *buf, size_t len, int flags) {
    size_t ret = (size_t)send(sock, (const char*)buf, (int)len, flags);

    if (ret != len) {
        return false;
    }

    const char *pbuf = (const char*)buf;

    /*
    // TODO: Some option for printing the outgoing packets
    printf("socket %d packet(%d/%d b): ", (int)sock, (int)ret, (int)len);
    for(size_t i = 0; i < len; i++) {
        printf("%03u ", (unsigned int)pbuf[i]);
    }
    printf("\n");
    */


    return true;
}

bool Socket::receiveData(void *buf, size_t len, int flags) {
    size_t ret = (size_t)recv(sock, (char*)buf, (int)len, flags);

    if (ret != len) {
        return false;
    }

    return true;
}
