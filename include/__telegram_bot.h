#pragma once
#include <cassert>
#include <optional>
#include <queue>
#include <string>
#include <type_traits>
#include <variant>

#include "headers/update_manager.h"
#include "headers/apimanager.h"

namespace telegram {
using opt_error = std::optional<Error>;
/**
 * @brief The Telegram Bot class that contains methods that implements Telegram Bot Api
 * This class also contain some additional methods to add sequences or callbacks
 *
 *
 * The implementation of this class is rather trivial, if you look at any API method
 * it would look like this
 *
 * std::pair<T,opt_error> method(args) {
 *  QueryBuilder builder;
 *  builder << make_named_pair(args);
 *  return ApiCall(__func__,method);
 * }
 *
 * Explanation:
 * 1) Every API method returns pair of type and error. Users are supposed to always
 * check if error contains value (because in that case there will be default constucted T)
 * 2) Args are always named as required in API
 * 3) make_named_pair is a macro, that produces a pair of arg and it`s name,
 * so users MUST NOT change arg names
 * 4) ApiCall is function from ApiManager class, that uses first parameter and a
 * name of method from Telegram Bot Api, so users MUST NOT change function names.
 */

class Bot {
  std::unique_ptr<ApiManager> api;
  UpdateManager updater;
  bool stopPolling = false;
  bool webhookSet = false;

public:
  Bot(const std::string &token,std::size_t thread_number = std::thread::hardware_concurrency()) noexcept;
  /**
   * @brief Set callback for Updates
   * \warning there can be only ONE callback for Updates
   */
  void onUpdate(UpdateCallback &&cb);

  /**
   * @brief set callback for ChosenInlineResult
   * @param cmd - data that will trigger the callback
   * @param cb - callback function that must follow void(ChosenInlineResult&&) signature
   */
  void onChosenInlineResult(std::string_view cmd, ChosenInlineResultCallback &&cb);

  /**
   * @brief set callback for CallbackQuery
   * @param cmd - data that will trigger the callback
   * @param cb - callback function that must follow void(CallbackQuery&&) signature
   */
  void onQueryCallback(std::string_view cmd, QueryCallback &&cb);

  /**
   * @brief set callback for Message (preferrably commands like /help)
   * @param cmd - text -in message that will trigger the callback
   * @param cb - callback that must follow void(Message&&) signature
   */
  void onMessage(std::string_view cmd, MessageCallback &&cb);

  /**
   * @brief set callback for InlineQuery
   * @param cmd - query in InlineQuery that will trigger the callback
   * @param cb - callback that must follow void(Message&&) signature
   */
  void onInlineQuery(std::string_view cmd, InlineQueryCallback&& cb);

  /**
   * @brief set callback for ShippingQuery
   * @param cmd - invoice_payload that will trigger the callback
   * @param cb - callback that must follow void(Message&&) signature
   */
  void onShippingQuery(std::string_view cmd,ShippingQueryCallback&& cb);

  /**
   * @brief set callback for PrecheckoutQuery query
   * @param cmd - invoice_payload -in query that will trigger the callback
   * @param cb - callback that must follow void(ShippingQuery&&) signature
   */
  void onPreCheckoutQuery(std::string_view cmd,PreCheckoutQueryCallback&& cb);

  /**
   * @brief Templated function that can be used to set callback with uniform syntax
   */
  template<class CallbackType>
  void onEvent(std::string_view cmd, CallbackType&& cb) {
       updater.addCallback(cmd, std::forward<CallbackType>(cb));
  }
  /**
   * @brief Templated function that can be used to set callback using std::regex
   * \warning regexes have the lowest priorty in callbacks routing
   * @param cmd - regex that will trigger the callback
   * @param cb - callback, must be one either one of MessageCallback, QueryCallback, InlineQueryCallback,
     ChosenInlineResultCallback,ShippingQueryCallback,PreCheckoutQueryCallback;
   */
  template<class CallbackType>
  void onEvent(std::regex cmd, CallbackType&& cb) {
       updater.addCallback(cmd, std::move(cb));
  }
  /**
   * @brief Function set sequence
   * \description Use this function to set sequence for id (this can be any number what
   * allows you to indentify sequence for routing)
   * @param id - any number that will be used to route callbacks of sequence
   * @param seq - sequence that must be a std::shared_ptr
   */
  template <class Event,class Check>
  void startSequence(int64_t id, std::shared_ptr<Sequence<Event,Check>> seq) {
      updater.addSequence(id, seq);
  }
  /**
   * @brief Removes sequence for 'user_id'
   * @param id - id that was specified in startSequence
   */
  void stopSequence(int64_t id);
  /**
   * Use this method to get current webhook status. Requires no parameters.
   * On success, returns a WebhookInfo object.
   * If the bot is using getUpdates, will return an object with the url field empty.
   */
  /**
   * @brief Short syntax to reply to message
   * @param msg - message to reply
   * @param message - text to send user
   */


  std::pair<Message, opt_error> reply(const Message &msg,
                                      const std::string &message) const;

  /// stops the bot
  void stop();

  /// TODO: add description
  void start(std::optional<int64_t> timeout = {}, std::optional<int64_t> offset = {}, std::optional<int8_t> limit = {},
             std::optional<std::vector<std::string_view>> allowed_updates = {});
  /**
   * @brief Starts webhook server on 0.0.0.0 address and given port and sets webhook for teleragram bot
   * @param url - url that will be used in setWebhook server
   * @param port - port that server will listen to
   * @param cert_path - path to public certificate (must have .pem format)
   * @param key_path - path to private key path (must have .pem format)
   */
  bool setWebhookServer(const std::string &url, uint16_t port,
                        const std::string &cert_path = "./cert.pem",
                        const std::string &key_path = "./key.pem");

  std::string getUpdatesRawJson(
          std::optional<uint32_t> offset = {}, std::optional<uint8_t> limit = {},
          std::optional<uint32_t> timeout = {},
          std::optional<std::vector<std::string_view>> allowed_updates = {});

  // --------------------- AUTOGENERATED CODE -------------------------------------


