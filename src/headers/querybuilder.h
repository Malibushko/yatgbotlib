#pragma once
#include <string>
#include <rapidjson/document.h>
#include "json_parser.h"
#include "utility/utility.h"
namespace telegram {
/**
 * @brief Class that builds JSON Document
 * The class has overloaded operator<< for writing to document
 *
 * Working with this class is as follows
 * 1) Create QueryBuilder instance
 * 2) Write data using 'make_named_pair' macro
 * 3) Grab data using getQuery or getDocument methods
 */
class QueryBuilder {
  rapidjson::Document doc{};

public:
  explicit QueryBuilder() = default;
  explicit QueryBuilder(rapidjson::Document::AllocatorType &allocator);

  /**
   * Overloaded shift operator for writing data
   * Accepts pair of parameter name and it`s value
   * Preferrably used with 'make_named_pair' macro
   */
  template <class T>
  friend QueryBuilder &operator<<(QueryBuilder &builder,
                                  const std::pair<std::string_view, T> &pair);
  /// Produce a query
  std::string getQuery() const noexcept;
  /**
   * Get document with written values
   * \warning if no value was sent the document will not contain any value
   * and using rapidjson::Document::GetObject will trigger assert
   */
  const rapidjson::Document &getDocument() const noexcept;
};

template <class T>
QueryBuilder &operator<<(QueryBuilder &builder,
                         const std::pair<std::string_view, T> &pair) {
  if (!builder.doc.IsObject())
    builder.doc.SetObject();

  rapidjson::Value val;
  if constexpr (std::is_enum_v<traits::optional_or_value<T>>) {
      std::string_view mappedValue;
      if constexpr (traits::is_optional_v<std::decay_t<T>>)
        mappedValue = utility::toString(pair.second.value_or(traits::optional_or_value<T>{}));
      else
        mappedValue = utility::toString(pair.second);

      val.SetString(mappedValue.data(),mappedValue.size());
  }
  else
    JsonParser::i().valueToJson(pair.second, val, builder.doc.GetAllocator());

  if (!val.IsNull()) {
    rapidjson::Value name(rapidjson::kStringType);
    name.SetString(pair.first.data(), pair.first.size(),
                   builder.doc.GetAllocator());
    builder.doc.AddMember(name, val, builder.doc.GetAllocator());
  }
  return builder;
}
} // namespace telegram
