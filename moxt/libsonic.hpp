#include "common.hpp"
#include <sonic/allocator.h>
#include <sonic/sonic.h>

using NodeType = sonic_json::Node;

SEQ_FUNC void seq_test_sonic_cpp();
SEQ_FUNC void seq_test_sonic_cpp_wrap();

SEQ_FUNC sonic_json::Document *seq_sonic_json_document_new();

SEQ_FUNC void seq_sonic_json_document_free(sonic_json::Document *doc);

SEQ_FUNC sonic_json::Document::Allocator *
seq_sonic_json_document_get_allocator(sonic_json::Document *doc);

SEQ_FUNC void seq_sonic_json_document_set_object(sonic_json::Document *doc);

SEQ_FUNC void seq_sonic_json_document_add_string(
    sonic_json::Document *doc, sonic_json::Document::Allocator *alloc,
    const char *key, size_t keyLen, const char *value, size_t valueLen);

SEQ_FUNC void
seq_sonic_json_document_add_int(sonic_json::Document *doc,
                                sonic_json::Document::Allocator *alloc,
                                const char *key, size_t keyLen, int64_t value);

SEQ_FUNC void seq_sonic_json_document_add_double(
    sonic_json::Document *doc, sonic_json::Document::Allocator *alloc,
    const char *key, size_t keyLen, double value);

SEQ_FUNC void
seq_sonic_json_document_add_bool(sonic_json::Document *doc,
                                 sonic_json::Document::Allocator *alloc,
                                 const char *key, size_t keyLen, bool value);

SEQ_FUNC void seq_sonic_json_document_add_string_array(
    sonic_json::Document *doc, sonic_json::Document::Allocator *alloc,
    const char *key, size_t keyLen, const char *value, size_t valueLen);

SEQ_FUNC void seq_sonic_json_document_add_int_array(
    sonic_json::Document *doc, sonic_json::Document::Allocator *alloc,
    const char *key, size_t keyLen, const char *value, size_t valueLen);

SEQ_FUNC void seq_sonic_json_document_add_double_array(
    sonic_json::Document *doc, sonic_json::Document::Allocator *alloc,
    const char *key, size_t keyLen, const char *value, size_t valueLen);

SEQ_FUNC void seq_sonic_json_document_add_bool_array(
    sonic_json::Document *doc, sonic_json::Document::Allocator *alloc,
    const char *key, size_t keyLen, const char *value, size_t valueLen);

SEQ_FUNC size_t seq_sonic_json_document_to_string(sonic_json::Document *doc,
                                                  char *result);
