#include "headers/querybuilder.h"
#include "utility/utility.h"
telegram::QueryBuilder::QueryBuilder(
    rapidjson::Document::AllocatorType &allocator)
    : doc{&allocator} {}

std::string telegram::QueryBuilder::getQuery() const noexcept {
  return utility::to_string(doc);
}

const rapidjson::Document &telegram::QueryBuilder::getDocument() const
    noexcept {
  return doc;
}
