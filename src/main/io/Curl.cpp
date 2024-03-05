#include "Curl.h"

#include "Settings.h"
#include "logger/logger.h"
#include "LibraryLoader.h"

namespace dynlib {
    #include <curl/curl.h>    
}

static LibraryLoader curlLib("libcurl-x64");
typedef dynlib::CURLcode (* curl_global_init_PROC) (long flags);
typedef struct dynlib::curl_slist* (*curl_slist_append_PROC) (struct dynlib::curl_slist *, const char *);
typedef void (*curl_slist_free_all_PROC) (struct dynlib::curl_slist *);
typedef void (*curl_global_cleanup_PROC) (void);
typedef dynlib::CURL* (*curl_easy_init_PROC) (void);
typedef dynlib::CURLcode (*curl_easy_setopt_PROC) (dynlib::CURL *curl, dynlib::CURLoption option, ...);
typedef dynlib::CURLcode (*curl_easy_perform_PROC) (dynlib::CURL *curl);
typedef void (*curl_easy_cleanup_PROC) (dynlib::CURL *curl);
typedef const char * (*curl_easy_strerror_PROC) (dynlib::CURLcode ret);
typedef dynlib::CURLcode (*curl_easy_getinfo_PROC) (dynlib::CURL *curl, dynlib::CURLINFO info, ...);

static curl_global_init_PROC       curl_global_init    = NULL;
static curl_slist_append_PROC      curl_slist_append   = NULL;
static curl_slist_free_all_PROC    curl_slist_free_all = NULL;
static curl_global_cleanup_PROC    curl_global_cleanup = NULL;
static curl_easy_init_PROC         curl_easy_init      = NULL;
static curl_easy_setopt_PROC       curl_easy_setopt    = NULL;
static curl_easy_perform_PROC      curl_easy_perform   = NULL;
static curl_easy_cleanup_PROC      curl_easy_cleanup   = NULL;
static curl_easy_strerror_PROC     curl_easy_strerror  = NULL;
static curl_easy_getinfo_PROC      curl_easy_getinfo   = NULL;

static size_t writeDataCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t length = size * nmemb;
    ((std::string*)userp)->append((char*)contents, length);

    return length;
}

RequestType Curl::getRequestType(std::string requestType) {
    if (requestType == "HTTP_POST" || requestType == "POST") {
        return RequestType::HTTP_POST;
    } else if (requestType == "HTTP_GET" || requestType == "GET") {
        return RequestType::HTTP_GET;
    } else if (requestType == "HTTP_PUT" || requestType == "PUT") {
        return RequestType::HTTP_PUT;
    } else if (requestType == "HTTP_DELETE" || requestType == "DELETE") {
        return RequestType::HTTP_DELETE;
    } else if (requestType == "HTTP_HEAD" || requestType == "HEAD") {
        return RequestType::HTTP_HEAD;
    } else if (requestType == "HTTP_OPTIONS" || requestType == "OPTIONS") {
        return RequestType::HTTP_OPTIONS;
    } else {
        loggerWarning("Invalid request type '%s', defaulting to POST", requestType.c_str());
        return RequestType::HTTP_POST;
    }
}

std::string Curl::getRequestType(RequestType requestType) {
    switch(requestType) {
        case RequestType::HTTP_POST:
            return "POST";
        case RequestType::HTTP_GET:
            return "GET";
        case RequestType::HTTP_PUT:
            return "PUT";
        case RequestType::HTTP_DELETE:
            return "DELETE";
        case RequestType::HTTP_HEAD:
            return "HEAD";
        case RequestType::HTTP_OPTIONS:
            return "OPTIONS";
        default:
            loggerWarning("Invalid request type, defaulting to empty");
            return "";
    }
}

bool Curl::globalInit() {
    if (curl_global_init) {
        loggerFatal("Can init only once");
        return false;
    }

    if (!curlLib.load()) {
        if (Settings::demo.networking) {
            loggerError("Could not initialize curl lib");
            Settings::demo.networking = false;
        } else {
            loggerDebug("Did not initialize curl lib");
        }
        
        return false;
    }

    curl_global_init = (curl_global_init_PROC)(curlLib.getProcAddress("curl_global_init"));
    curl_slist_append = (curl_slist_append_PROC)(curlLib.getProcAddress("curl_slist_append"));
    curl_slist_free_all = (curl_slist_free_all_PROC)(curlLib.getProcAddress("curl_slist_free_all"));
    curl_global_cleanup = (curl_global_cleanup_PROC)(curlLib.getProcAddress("curl_global_cleanup"));
    curl_easy_init = (curl_easy_init_PROC)(curlLib.getProcAddress("curl_easy_init"));
    curl_easy_setopt = (curl_easy_setopt_PROC)(curlLib.getProcAddress("curl_easy_setopt"));
    curl_easy_perform = (curl_easy_perform_PROC)(curlLib.getProcAddress("curl_easy_perform"));
    curl_easy_cleanup = (curl_easy_cleanup_PROC)(curlLib.getProcAddress("curl_easy_cleanup"));
    curl_easy_strerror = (curl_easy_strerror_PROC)(curlLib.getProcAddress("curl_easy_strerror"));
    curl_easy_getinfo = (curl_easy_getinfo_PROC)(curlLib.getProcAddress("curl_easy_getinfo"));

    curl_global_init(CURL_GLOBAL_DEFAULT);

    Settings::demo.networking = true;
    return true;
}

bool Curl::globalExit() {
    if (curl_global_cleanup) {
        curl_global_cleanup();
    }

    return true;
}


Curl::Curl() {
    curl = NULL;
    readBuffer = "";
    httpStatus = 0L;
}

Curl::~Curl() {
    if (curl) {
        exit();
    }
}

bool Curl::init() {
    curl = static_cast<void*>(curl_easy_init());
    if (!curl) {
        loggerError("Could not initialize curl");
        return false;
    }


    return true;
}

bool Curl::exit() {
    if (!curl) {
        return true;
    }

    curl_easy_cleanup(static_cast<dynlib::CURL*>(curl));
    curl = NULL;

    return true;
}

void Curl::open(RequestType type, std::string url) {
    if (!curl) {
        if (!init()) {
            return;
        }
    }

    headers.clear();

    if (type == RequestType::HTTP_POST) {
        curl_easy_setopt(static_cast<dynlib::CURL*>(curl), dynlib::CURLOPT_POST, 1L);
    } else {
        curl_easy_setopt(static_cast<dynlib::CURL*>(curl), dynlib::CURLOPT_POST, 0L);
    }

    curl_easy_setopt(static_cast<dynlib::CURL*>(curl), dynlib::CURLOPT_URL, url.c_str());
    
    const long CONNECT_TIMEOUT_MS = 10000L;
    curl_easy_setopt(static_cast<dynlib::CURL*>(curl), dynlib::CURLOPT_CONNECTTIMEOUT_MS, CONNECT_TIMEOUT_MS);
    const long TRANSFER_TIMEOUT_MS = 30000L;
    curl_easy_setopt(static_cast<dynlib::CURL*>(curl), dynlib::CURLOPT_TIMEOUT_MS, TRANSFER_TIMEOUT_MS);

    // INSECURE: Do not verify via CA trustroots
    curl_easy_setopt(static_cast<dynlib::CURL*>(curl), dynlib::CURLOPT_SSL_VERIFYPEER, 0L);
    // INSECURE: Do not verify that TLS cert matches host
    curl_easy_setopt(static_cast<dynlib::CURL*>(curl), dynlib::CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(static_cast<dynlib::CURL*>(curl), dynlib::CURLOPT_WRITEFUNCTION, writeDataCallback);
    curl_easy_setopt(static_cast<dynlib::CURL*>(curl), dynlib::CURLOPT_WRITEDATA, &readBuffer);
}

void Curl::setRequestHeader(std::string key, std::string value) {
    headers[key] = value;
}

void Curl::send(std::string data) {
    readBuffer = "";
    curl_easy_setopt(curl, dynlib::CURLOPT_POSTFIELDS, data.c_str());

    struct dynlib::curl_slist *list = NULL;
    for (const auto &header : headers) {
        std::string headerValue = header.first + std::string(": ") + header.second;
        list = curl_slist_append(list, headerValue.c_str());
    }
    curl_easy_setopt(static_cast<dynlib::CURL*>(curl), dynlib::CURLOPT_HTTPHEADER, list);

    dynlib::CURLcode res = curl_easy_perform(static_cast<dynlib::CURL*>(curl));
    curl_slist_free_all(list);

    if(res != dynlib::CURLE_OK) {
        loggerWarning("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        readBuffer = "";
    } else {
        loggerTrace("Got response! resp:'%s'\n", readBuffer.c_str());
    }

    httpStatus = 0L;
    curl_easy_getinfo(static_cast<dynlib::CURL*>(curl), dynlib::CURLINFO_RESPONSE_CODE, &httpStatus);
}

const std::string& Curl::getData() {
    return readBuffer;
}

long Curl::getHttpStatus() {
    return httpStatus;
}