#pragma once
#include <string_view>
#include <vector>
#include <memory>
#include <algorithm>
#include <optional>
template <class T>
class Trie {
    struct TrieNode {
        int index = 0;
        T value;
        std::vector<std::shared_ptr<TrieNode>> children{};
        bool is_end_of_word {false};
        TrieNode() = default;
        TrieNode(int index) : index{index} {}
        std::shared_ptr<TrieNode> findChildren(int index) {
            auto result = std::find_if(children.begin(),children.end(),
                                       [index](const std::shared_ptr<TrieNode> & val){
                return val->index == index;
            });
            if (result == children.end())
                return nullptr;
            else
                return *result;
        }
    };
    std::shared_ptr<TrieNode> root{std::make_shared<TrieNode>()};

    bool empty(const std::shared_ptr<TrieNode>& node) const {
        return std::find_if(node->children.begin(),node->children.end(),[](auto && val){return val;}) ==
                node->children.end();
    }
    bool erase_impl(std::shared_ptr<TrieNode> iter,std::string_view item) {
        if (!iter)
            return false;
        if (item.length()) {
            if (!iter->is_end_of_word && erase_impl(iter->findChildren(item[0]),item.substr(1))) {
                if (empty(iter)) {
                    iter.reset();
                    return true;
                }
                else
                    return false;
            }
        }
        if (!item.length() && iter->is_end_of_word) {
            if (empty(iter)) {
                iter.reset();
                return true;
            } else {
                iter->is_end_of_word = false;
                return false;
            }
        }
        return false;
    }
    size_t m_size{0};
public:
    void insert(std::string_view item,const T& value) {
        std::shared_ptr<TrieNode> iter = root;
        for (auto && it : item) {
            if (!iter->findChildren(it)) {
                iter->children.emplace_back(std::make_shared<TrieNode>(it));
            }
            iter = iter->findChildren(it);
        }
        iter->is_end_of_word = true;
        iter->value = value;
        ++m_size;
    }
    std::optional<T> find(std::string_view item) const {
        if (empty(root))
            return std::optional<T>();
        std::shared_ptr<TrieNode> iter = root;
        for (char it : item) {
            iter = iter->findChildren(it);
            if (!iter)
                return std::optional<T>();
        }
        return (iter->is_end_of_word ? iter->value : std::optional<T>());
    }
    void erase(std::string_view item) {
        if (!erase_impl(root,item)) {
            --m_size;
        }
    }
    int32_t size() const noexcept{
        return m_size;
    }
};

