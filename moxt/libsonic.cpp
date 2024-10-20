#include "libsonic.hpp"
#include "common.hpp"
#include "moxt/utils/cast.hpp"
#include <absl/strings/numbers.h>
#include <absl/strings/str_split.h>
#include <cstddef>
#include <cstdint>

inline std::string sonic_json_to_string(const sonic_json::Document &doc) {
    sonic_json::WriteBuffer wb;
    doc.Serialize(wb);
    std::string s = wb.ToString();
    return s;
}

inline std::string sonic_json_to_string(const sonic_json::Document *doc) {
    sonic_json::WriteBuffer wb;
    doc->Serialize(wb);
    std::string s = wb.ToString();
    return s;
}

SEQ_FUNC void seq_test_sonic_cpp() {
    sonic_json::Document doc_mut;
    auto &alloc = doc_mut.GetAllocator();
    std::string category("linear");
    std::string symbol("BTCUSDT");
    doc_mut.SetObject();
    doc_mut.AddMember("category", NodeType(category), alloc);
    doc_mut.AddMember("symbol", NodeType(symbol), alloc);

    std::string body_str = sonic_json_to_string(doc_mut);
    logi("body_str: {}", body_str);
}

SEQ_FUNC void seq_test_sonic_cpp_wrap() {
    sonic_json::Document *doc = new sonic_json::Document();
    auto alloc = &doc->GetAllocator();
    std::string *category = new std::string("linear");
    std::string *symbol = new std::string("BTCUSDT");
    doc->SetObject();
    doc->AddMember("category", NodeType(*category), *alloc);
    doc->AddMember("symbol", NodeType(*symbol), *alloc);
    delete category;
    delete symbol;

    std::string body_str = sonic_json_to_string(doc);
    logi("body_str: {}", body_str);
}

SEQ_FUNC sonic_json::Document *seq_sonic_json_document_new() {
    return new sonic_json::Document();
}

SEQ_FUNC void seq_sonic_json_document_free(sonic_json::Document *doc) {
    delete doc;
}

SEQ_FUNC sonic_json::Document::Allocator *
seq_sonic_json_document_get_allocator(sonic_json::Document *doc) {
    return &doc->GetAllocator();
}

SEQ_FUNC void seq_sonic_json_document_set_object(sonic_json::Document *doc) {
    doc->SetObject();
}

SEQ_FUNC void seq_sonic_json_document_add_string(
    sonic_json::Document *doc, sonic_json::Document::Allocator *alloc,
    const char *key, size_t keyLen, const char *value, size_t valueLen) {
    auto key_ = std::string_view(key, keyLen);
    auto value_ = std::string_view(value, valueLen);
    doc->AddMember(key_, NodeType(value_), *alloc);
}

SEQ_FUNC void
seq_sonic_json_document_add_int(sonic_json::Document *doc,
                                sonic_json::Document::Allocator *alloc,
                                const char *key, size_t keyLen, int64_t value) {
    auto key_ = std::string_view(key, keyLen);
    doc->AddMember(key_, NodeType(value), *alloc);
}

SEQ_FUNC void seq_sonic_json_document_add_double(
    sonic_json::Document *doc, sonic_json::Document::Allocator *alloc,
    const char *key, size_t keyLen, double value) {
    auto key_ = std::string(key, keyLen);
    doc->AddMember(key_, NodeType(value), *alloc);
}

SEQ_FUNC void
seq_sonic_json_document_add_bool(sonic_json::Document *doc,
                                 sonic_json::Document::Allocator *alloc,
                                 const char *key, size_t keyLen, bool value) {
    auto key_ = std::string_view(key, keyLen);
    doc->AddMember(key_, NodeType(value), *alloc);
}

SEQ_FUNC void seq_sonic_json_document_add_string_array(
    sonic_json::Document *doc, sonic_json::Document::Allocator *alloc,
    const char *key, size_t keyLen, const char *value, size_t valueLen) {
    auto key_ = std::string_view(key, keyLen);
    auto value_ = std::string_view(value, valueLen);
    NodeType nodeArgs(sonic_json::kArray);
    auto strArray = absl::StrSplit(value_, absl::ByAnyChar(","));
    for (auto &str : strArray) {
        nodeArgs.PushBack(NodeType(str), *alloc);
    }
    doc->AddMember(key_, nodeArgs, *alloc);
}

SEQ_FUNC void seq_sonic_json_document_add_int_array(
    sonic_json::Document *doc, sonic_json::Document::Allocator *alloc,
    const char *key, size_t keyLen, const char *value, size_t valueLen) {
    auto key_ = std::string_view(key, keyLen);
    auto value_ = std::string_view(value, valueLen);
    NodeType nodeArgs(sonic_json::kArray);
    auto strArray = absl::StrSplit(value_, absl::ByAnyChar(","));
    for (auto &str : strArray) {
        int64_t v = strtoi(str);
        nodeArgs.PushBack(NodeType(v), *alloc);
    }
    doc->AddMember(key_, nodeArgs, *alloc);
}

SEQ_FUNC void seq_sonic_json_document_add_double_array(
    sonic_json::Document *doc, sonic_json::Document::Allocator *alloc,
    const char *key, size_t keyLen, const char *value, size_t valueLen) {
    auto key_ = std::string_view(key, keyLen);
    auto value_ = std::string_view(value, valueLen);
    NodeType nodeArgs(sonic_json::kArray);
    auto strArray = absl::StrSplit(value_, absl::ByAnyChar(","));
    for (auto &str : strArray) {
        double v = strtod(str);
        nodeArgs.PushBack(NodeType(v), *alloc);
    }
    doc->AddMember(key_, nodeArgs, *alloc);
}

SEQ_FUNC void seq_sonic_json_document_add_bool_array(
    sonic_json::Document *doc, sonic_json::Document::Allocator *alloc,
    const char *key, size_t keyLen, const char *value, size_t valueLen) {
    auto key_ = std::string_view(key, keyLen);
    auto value_ = std::string_view(value, valueLen);
    NodeType nodeArgs(sonic_json::kArray);
    auto strArray = absl::StrSplit(value_, absl::ByAnyChar(","));
    for (auto &str : strArray) {
        bool v;
        auto ok = absl::SimpleAtob(str, &v);
        if (ok) {
            nodeArgs.PushBack(NodeType(v), *alloc);
        }
    }
    doc->AddMember(key_, nodeArgs, *alloc);
}

SEQ_FUNC void seq_sonic_json_document_add_node(
    sonic_json::Document *doc, sonic_json::Document::Allocator *alloc,
    const char *key, size_t keyLen, NodeType *node) {
    auto key_ = std::string_view(key, keyLen);
    doc->AddMember(key_, *node, *alloc);
}

SEQ_FUNC NodeType *
seq_sonic_json_document_find_member(sonic_json::Document *doc,
                                    sonic_json::Document::Allocator *alloc,
                                    const char *key, size_t keyLen) {
    auto key_ = std::string_view(key, keyLen);
    auto m = doc->FindMember(key_);
    if (m == doc->MemberEnd()) {
        return nullptr;
    } else {
        return &m->value;
    }
}

SEQ_FUNC NodeType *seq_sonic_json_node_new() {
    auto *node = new NodeType();
    return node;
}

SEQ_FUNC void seq_sonic_json_node_free(NodeType *node) { delete node; }

SEQ_FUNC void seq_sonic_json_node_set_object(NodeType *node) {
    node->SetObject();
}

SEQ_FUNC void seq_sonic_json_node_add_string(
    NodeType *node, sonic_json::Document::Allocator *alloc, const char *key,
    size_t keyLen, const char *value, size_t valueLen) {
    auto key_ = std::string_view(key, keyLen);
    auto value_ = std::string_view(value, valueLen);
    node->AddMember(key_, NodeType(value_), *alloc);
}

SEQ_FUNC void
seq_sonic_json_node_add_int(NodeType *node,
                            sonic_json::Document::Allocator *alloc,
                            const char *key, size_t keyLen, int64_t value) {
    auto key_ = std::string_view(key, keyLen);
    node->AddMember(key_, NodeType(value), *alloc);
}

SEQ_FUNC void
seq_sonic_json_node_add_double(NodeType *node,
                               sonic_json::Document::Allocator *alloc,
                               const char *key, size_t keyLen, double value) {
    auto key_ = std::string(key, keyLen);
    node->AddMember(key_, NodeType(value), *alloc);
}

SEQ_FUNC void
seq_sonic_json_node_add_bool(NodeType *node,
                             sonic_json::Document::Allocator *alloc,
                             const char *key, size_t keyLen, bool value) {
    auto key_ = std::string_view(key, keyLen);
    node->AddMember(key_, NodeType(value), *alloc);
}

SEQ_FUNC void
seq_sonic_json_node_add_node(NodeType *node,
                             sonic_json::Document::Allocator *alloc,
                             const char *key, size_t keyLen, NodeType *valueNode) {
    auto key_ = std::string_view(key, keyLen);
    node->AddMember(key_, *valueNode, *alloc);
}

SEQ_FUNC size_t seq_sonic_json_document_to_string(sonic_json::Document *doc,
                                                  char *result) {
    auto s = sonic_json_to_string(doc);
    // logi("s={}", s);
    strcpy(result, s.c_str());
    return s.length();
}