#include "networkmanager.h"

using namespace telegram;
using namespace telegram::utility;
NetworkManager::NetworkManager(const char *host) : cli{host} {
  cli.set_follow_location(true);
}

std::shared_ptr<httplib::Response>
NetworkManager::post(const std::string &url,
                     const httplib::Headers &headers,
                     const std::string &body ,
                     const std::string &content_type) {
  auto reply = cli.Post(url.data(), headers, body, content_type.data());
  if (reply && reply->status) {
      // check if status is redirection
    if (reply->status == std::clamp(reply->status,
                                    static_cast<int>(http::redirection),
                                    static_cast<int>(http::client_error))) {
        // follow the redirection
      reply = cli.Post(url.data(), headers, body, content_type.data());
    }
    return reply;
  } else
    return {};
}

uint32_t NetworkManager::ipv4(const std::string& s) {
    uint8_t last_pos = 0;
    uint8_t buf = 0;
    uint8_t oct1 = std::stoi(s.substr(0,buf = last_pos = s.find_first_of('.')));
    uint8_t oct2 = std::stoi(s.substr(buf+1,last_pos = s.find_first_of('.',buf+1)));
    buf = last_pos;
    uint8_t oct3 = std::stoi(s.substr(buf+1,last_pos = s.find_first_of('.',buf+1)));
    buf = last_pos;
    uint8_t oct4 = std::stoi(s.substr(buf+1));

    auto octetToIP = [&]() -> uint32_t {
        uint32_t result{0};
        result |= oct1;
        result <<= 8;
        result |= oct2;
        result <<= 8;
        result |= oct3;
        result <<= 8;
        result |= oct4;
        return result;
    };
    return octetToIP();
}


std::shared_ptr<httplib::Response>
NetworkManager::post(const std::string &url,
                     const httplib::MultipartFormDataItems &items) {
  auto reply = cli.Post(url.data(), items);
  if (reply && reply->status) {

    if (reply->status == std::clamp(reply->status,
                                    static_cast<int>(http::redirection),
                                    static_cast<int>(http::client_error))) {
        // follow the redirection
        reply = cli.Post(url.data(), items);
    }
    return reply;
  } else
    return {};
}
