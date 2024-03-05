#ifndef ENGINE_IO_CURL_H_
#define ENGINE_IO_CURL_H_

#include <string>
#include <map>

enum class RequestType {
    HTTP_POST = 0,
    HTTP_GET = 1,
    HTTP_PUT = 2,
    HTTP_DELETE = 3,
    HTTP_HEAD = 4,
    HTTP_OPTIONS = 5
};

class Curl {
public:
    static RequestType getRequestType(std::string requestType);
    static std::string getRequestType(RequestType requestType);
    static bool globalInit();
    static bool globalExit();
    Curl();
    ~Curl();
    bool init();
    bool exit();
    void open(RequestType type, std::string url);
    void setRequestHeader(std::string key, std::string value);
    void send(std::string data = "");
    const std::string& getData();
    long getHttpStatus();
private:
    std::map<std::string, std::string> headers;
    std::string readBuffer;
    long httpStatus;
    void *curl;
};

#endif /*ENGINE_IO_CURL_H_*/
