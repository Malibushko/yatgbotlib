#include "querybuilder.h"
using namespace telegram;

QueryBuilder::QueryBuilder(
    rapidjson::Document::AllocatorType &allocator)
    : doc{&allocator} {}

std::string QueryBuilder::getQuery() const noexcept {
  return JsonParser::i().rapidDocumentToString(doc);
}
const rapidjson::Document &QueryBuilder::getDocument() const noexcept {
  return doc;
}
