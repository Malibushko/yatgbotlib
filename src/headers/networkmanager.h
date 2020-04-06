#pragma once
#include <memory>
#include <httplib/httplib.h>
#include "utility/utility.h"

namespace telegram {
/**
 * @brief Class for making HTTP requests
 */
class NetworkManager {
  httplib::Client cli;
  /// http return code groups
  enum class http {
    informational = 100,
    success = 200,
    redirection = 300,
    client_error = 400,
    server_error = 500
  };
public:
  NetworkManager(const char *host);
  NetworkManager(const NetworkManager &) = delete;
  NetworkManager(NetworkManager &&) = delete;
  NetworkManager &operator=(const NetworkManager &) = delete;
  NetworkManager &operator=(NetworkManager &&) = delete;
  /**
   * @brief Function transforms string representation of IPv4 address to 32-bit int
   * @param s - string in a format "255.255.255.255"
   * \return 32-bit integet representing IP address
   */
  static uint32_t ipv4(const std::string& s);
  /**
    * @brief Usual POST request
    * @param url - url for POST request
    * @param headers - headers of POST request
    * @param body - body of POST request (empty json object by default)
    * @param content_type - 'Content-Type' header of POST request (application/json by default)
    */
  std::shared_ptr<httplib::Response>
  post(const std::string &url, const httplib::Headers &headers = {},
       const std::string &body = "{}",
       const std::string &content_type =
           utility::toString(ContentTypes::application_json).data());
  /**
    * @brief multipart/form POST requiest
    * @param url - url for POST request
    * @param items - multipart data to be sent
    */
  std::shared_ptr<httplib::Response>
  post(const std::string &url, const httplib::MultipartFormDataItems &items);
};

} // namespace telegram
