#include "libsimdjson.hpp"
#include "simdjson.h"
#include <cstddef>
#include <cstdlib>
#include <sstream>
#include <string_view>

SEQ_FUNC ondemand::parser *
seq_simdjson_ondemand_parser_new(size_t max_capacity) {
    return mem_new<ondemand::parser, size_t>(max_capacity);
}

SEQ_FUNC padded_string *seq_simdjson_padded_string_new(const char *s,
                                                       size_t len) {
    return mem_new<padded_string, const char *, size_t>(s, len);
}

SEQ_FUNC ondemand::document *
seq_simdjson_ondemand_parser_parse(ondemand::parser *parser,
                                   padded_string *data) {
    auto doc = mem_dup<ondemand::document>(parser->iterate(*data));
    return doc;
}

#define ONDEMAND_DOCUMENT_GET_IMPL(T, U)                                       \
    SEQ_FUNC void *seq_simdjson_ondemand_get_##U##_d(                          \
        ondemand::T *doc, const char *key, size_t len) {                       \
        return mem_dup<ondemand::U>((*doc)[std::string_view(key, len)]);       \
    }

#define ONDEMAND_DOCUMENT_GET_RAW_IMPL(N, T, U, DEFAULT)                       \
    SEQ_FUNC U seq_simdjson_ondemand_get_##N##_d(                              \
        ondemand::T *doc, const char *key, size_t len) {                       \
        auto s = std::string_view(key, len);                                   \
        auto v = (*doc)[s];                                                    \
        U res;                                                                 \
        auto ok = v.get(res);                                                  \
        return ok == SUCCESS ? res : (DEFAULT);                                \
    }

#define ONDEMAND_VALUE_GET_IMPL(N, T, U)                                       \
    SEQ_FUNC void *seq_simdjson_ondemand_get_##U##_##N(                        \
        ondemand::T *p, const char *key, size_t len) {                         \
        return mem_dup<ondemand::U>((*p)[std::string_view(key, len)]);         \
    }

#define ONDEMAND_VALUE_GET_RAW_IMPL(N, T, U, DEFAULT)                          \
    SEQ_FUNC U seq_simdjson_ondemand_get_##N##_v(                              \
        ondemand::T *p, const char *key, size_t len) {                         \
        auto v = (*p)[std::string_view(key, len)];                             \
        U res;                                                                 \
        auto ok = v.get(res);                                                  \
        return ok == SUCCESS ? res : (DEFAULT);                                \
    }

#define ONDEMAND_OBJECT_GET_RAW_IMPL(N, T, U, DEFAULT)                         \
    SEQ_FUNC U seq_simdjson_ondemand_get_##N##_o(                              \
        ondemand::T *p, const char *key, size_t len) {                         \
        auto v = (*p)[std::string_view(key, len)];                             \
        U res;                                                                 \
        auto ok = v.get(res);                                                  \
        return ok == SUCCESS ? res : (DEFAULT);                                \
    }

#define ONDEMAND_GET_RAW_IMPL(N, S, T, U, DEFAULT)                             \
    SEQ_FUNC U seq_simdjson_ondemand_##N##_##S(ondemand::T *p) {               \
        U res;                                                                 \
        auto ok = (*p).get(res);                                               \
        return ok == SUCCESS ? res : (DEFAULT);                                \
    }

#define ONDEMAND_GET_STR_RAW_IMPL(N, T)                                        \
    SEQ_FUNC const char *seq_simdjson_ondemand_string_##N(ondemand::T *p,      \
                                                          size_t *n) {         \
        std ::string_view s;                                                   \
        auto ok = (*p).get(s);                                                 \
        if (ok == SUCCESS) {                                                   \
            *n = s.size();                                                     \
            return s.data();                                                   \
        } else {                                                               \
            *n = 0;                                                            \
            return "";                                                         \
        }                                                                      \
    }

#define ONDEMAND_DOCUMENT_GET_STR_IMPL(N, T)                                   \
    SEQ_FUNC const char *seq_simdjson_ondemand_get_string_##N(                 \
        ondemand::T *p, const char *key, size_t len, size_t *n) {              \
        auto v = (*p)[std::string_view(key, len)];                             \
        std::string_view s;                                                    \
        auto ok = v.get(s);                                                    \
        if (ok == SUCCESS) {                                                   \
            *n = s.size();                                                     \
            return s.data();                                                   \
        } else {                                                               \
            *n = 0;                                                            \
            return "";                                                         \
        }                                                                      \
    }

// 实现释放函数
#define ONDEMAND_FREE_RAW_IMPL(T)                                              \
    SEQ_FUNC void seq_simdjson_##T##_free(T *p) { mem_delete(p); }

#define ONDEMAND_FREE_IMPL(T)                                                  \
    SEQ_FUNC void seq_simdjson_ondemand_##T##_free(ondemand::T *p) {           \
        mem_delete(p);                                                         \
    }

// const char *
// seq_simdjson_ondemand_get_string_d(ondemand ::document *p,
//                                    const char *key, size_t len, size_t *n)

ONDEMAND_DOCUMENT_GET_STR_IMPL(d, document)
ONDEMAND_DOCUMENT_GET_STR_IMPL(v, value)
ONDEMAND_DOCUMENT_GET_STR_IMPL(o, object)

// int64_t seq_simdjson_ondemand_get_int_d(
//  ondemand::document *doc, const char *key, size_t len)
ONDEMAND_DOCUMENT_GET_RAW_IMPL(int, document, int64_t, 0)
ONDEMAND_DOCUMENT_GET_RAW_IMPL(uint, document, uint64_t, 0)
ONDEMAND_DOCUMENT_GET_RAW_IMPL(float, document, double, 0.0)
ONDEMAND_DOCUMENT_GET_RAW_IMPL(bool, document, bool, false)

// int64_t seq_simdjson_ondemand_get_int_v(
//  ondemand::value *value, const char *key, size_t len)
ONDEMAND_VALUE_GET_RAW_IMPL(int, value, int64_t, 0)
ONDEMAND_VALUE_GET_RAW_IMPL(uint, value, uint64_t, 0)
ONDEMAND_VALUE_GET_RAW_IMPL(float, value, double, 0.0)
ONDEMAND_VALUE_GET_RAW_IMPL(bool, value, bool, false)

// #define ONDEMAND_GET_RAW_IMPL(N, S, T, U, FUNC)
ONDEMAND_GET_RAW_IMPL(int, v, value, int64_t, 0)
ONDEMAND_GET_RAW_IMPL(uint, v, value, uint64_t, 0)
ONDEMAND_GET_RAW_IMPL(float, v, value, double, 0.0)
ONDEMAND_GET_RAW_IMPL(bool, v, value, bool, false)

ONDEMAND_GET_STR_RAW_IMPL(v, value)

SEQ_FUNC ondemand::object *seq_simdjson_ondemand_object_v(ondemand::value *p) {
    return mem_dup<ondemand::object>((*p).get_object().value());
}

SEQ_FUNC ondemand::array *seq_simdjson_ondemand_array_v(ondemand::value *p) {
    return mem_dup<ondemand::array>((*p).get_array().value());
}

// int64_t seq_simdjson_ondemand_get_int_o(
//                                                    ondemand ::value *value,
//                                                    const char *key,
//                                                    size_t len)
ONDEMAND_OBJECT_GET_RAW_IMPL(int, object, int64_t, 0)
ONDEMAND_OBJECT_GET_RAW_IMPL(uint, object, uint64_t, 0)
ONDEMAND_OBJECT_GET_RAW_IMPL(float, object, double, 0.0)
ONDEMAND_OBJECT_GET_RAW_IMPL(bool, object, bool, false)

// void *seq_simdjson_ondemand_get_value_d(
//                                                    ondemand ::document *doc,
//                                                    const char *key,
//                                                    size_t len)
ONDEMAND_DOCUMENT_GET_IMPL(document, value)
ONDEMAND_DOCUMENT_GET_IMPL(document, object)
ONDEMAND_DOCUMENT_GET_IMPL(document, array)

// void *seq_simdjson_ondemand_get_value_v(
//                                                    ondemand ::value *value,
//                                                    const char *key,
//                                                    size_t len)
ONDEMAND_VALUE_GET_IMPL(v, value, value)
ONDEMAND_VALUE_GET_IMPL(v, value, object)
ONDEMAND_VALUE_GET_IMPL(v, value, array)

ONDEMAND_VALUE_GET_IMPL(o, object, value)
ONDEMAND_VALUE_GET_IMPL(o, object, object)
ONDEMAND_VALUE_GET_IMPL(o, object, array)

// void seq_simdjson_ondemand_parser_free(ondemand::parser *p)
ONDEMAND_FREE_IMPL(parser)
ONDEMAND_FREE_RAW_IMPL(padded_string)
ONDEMAND_FREE_IMPL(document)
ONDEMAND_FREE_IMPL(value)
ONDEMAND_FREE_IMPL(object)
ONDEMAND_FREE_IMPL(array)

SEQ_FUNC size_t seq_simdjson_ondemand_array_count_elements(ondemand::array *p) {
    return p->count_elements();
}

SEQ_FUNC ondemand::value *seq_simdjson_ondemand_array_at(ondemand::array *p,
                                                         size_t index) {
    return mem_dup<ondemand::value>(p->at(index).value());
}

#define ONDEMAND_ARRAY_AT(N, U, FUNC)                                          \
    SEQ_FUNC U seq_simdjson_ondemand_array_at_##N(ondemand::array *p,          \
                                                  size_t index) {              \
        return p->at(index).FUNC().value();                                    \
    }

ONDEMAND_ARRAY_AT(int, int64_t, get_int64)
ONDEMAND_ARRAY_AT(uint, int64_t, get_uint64)
ONDEMAND_ARRAY_AT(float, double, get_double)
ONDEMAND_ARRAY_AT(bool, bool, get_bool)

SEQ_FUNC const char *seq_simdjson_ondemand_array_at_str(ondemand::array *p,
                                                        size_t index,
                                                        size_t *n) {
    std::string_view s = p->at(index).get_string().value();
    *n = s.size();
    return s.data();
}

SEQ_FUNC ondemand::object *
seq_simdjson_ondemand_array_at_obj(ondemand::array *p, size_t index) {
    return mem_dup<ondemand::object>(p->at(index).get_object().value());
}

SEQ_FUNC ondemand::array *seq_simdjson_ondemand_array_at_arr(ondemand::array *p,
                                                             size_t index) {
    return mem_dup<ondemand::array>(p->at(index).get_array().value());
}

#define ONDEMAND_ARRAY_ITER_GET(N, U, FUNC)                                    \
    SEQ_FUNC U seq_simdjson_ondemand_array_iter_get_##N(                       \
        ondemand::array_iterator *self) {                                      \
        return (**self).FUNC().value();                                        \
    }

ONDEMAND_ARRAY_ITER_GET(int, int64_t, get_int64)
ONDEMAND_ARRAY_ITER_GET(uint, int64_t, get_uint64)
ONDEMAND_ARRAY_ITER_GET(float, double, get_double)
ONDEMAND_ARRAY_ITER_GET(bool, int64_t, get_bool)

SEQ_FUNC int64_t seq_simdjson_ondemand_value_type(ondemand::value *p) {
    /**
     * The type of a JSON value.
     *
    enum class json_type {
        // Start at 1 to catch uninitialized / default values more easily
        array=1, ///< A JSON array   ( [ 1, 2, 3 ... ] )
        object,  ///< A JSON object  ( { "a": 1, "b" 2, ... } )
        number,  ///< A JSON number  ( 1 or -2.3 or 4.5e6 ...)
        string,  ///< A JSON string  ( "a" or "hello world\n" ...)
        boolean, ///< A JSON boolean (true or false)
        null     ///< A JSON null    (null)
    };
        */
    switch (p->type()) {
    case ondemand::json_type::array:
        return 1;
    case ondemand::json_type::object:
        return 2;
    case ondemand::json_type::number:
        return 3;
    case ondemand::json_type::string:
        return 4;
    case ondemand::json_type::boolean:
        return 5;
    case ondemand::json_type::null:
        return 6;
    default:
        return 0;
    };
}

SEQ_FUNC ondemand::value *
seq_simdjson_ondemand_array_iter_get(ondemand::array_iterator *self) {
    return mem_dup<ondemand::value>((**self).value());
}

SEQ_FUNC const char *
seq_simdjson_ondemand_array_iter_get_str(ondemand::array_iterator *self,
                                         size_t *n) {
    // std::string_view s = (**self).get_string().value();
    std::string_view s;
    auto ok = (**self).get(s);
    if (ok == SUCCESS) {
        *n = s.size();
        return s.data();
    } else {
        *n = 0;
        return "";
    }
}

SEQ_FUNC ondemand::object *
seq_simdjson_ondemand_array_iter_get_obj(ondemand::array_iterator *self) {
    return mem_dup<ondemand::object>((**self).get_object().value());
}

SEQ_FUNC ondemand::array *
seq_simdjson_ondemand_array_iter_get_arr(ondemand::array_iterator *self) {
    return mem_dup<ondemand::array>((**self).get_array().value());
}

SEQ_FUNC bool
seq_simdjson_ondemand_array_iter_get_str_pair(ondemand::array_iterator *self,
                                              char *item1, char *item2) {
    auto arr = (**self).get_array().value();
    size_t index = 0;
    for (auto i : arr) {
        std::string_view s = i.get_string().value();
        if (index == 0) {
            memcpy(item1, s.data(), s.length());
        } else if (index == 1) {
            memcpy(item2, s.data(), s.length());
        }
        ++index;
    }
    return index == 1;
}

SEQ_FUNC ondemand::array_iterator *
seq_simdjson_ondemand_array_begin(ondemand::array *p) {
    auto a = p->begin();
    return mem_dup<ondemand::array_iterator>(p->begin().value());
}

SEQ_FUNC ondemand::array_iterator *
seq_simdjson_ondemand_array_end(ondemand::array *p) {
    return mem_dup<ondemand::array_iterator>(p->end());
}

SEQ_FUNC bool
seq_simdjson_ondemand_array_iter_not_equal(ondemand::array_iterator *lhs,
                                           ondemand::array_iterator *rhs) {
    return *lhs != *rhs;
}

SEQ_FUNC void
seq_simdjson_ondemand_array_iter_step(ondemand::array_iterator *self) {
    ++(*self);
}

SEQ_FUNC void
seq_simdjson_ondemand_array_iter_free(ondemand::array_iterator *p) {
    mem_delete<ondemand::array_iterator>(p);
}

// DOM

SEQ_FUNC dom::parser *seq_simdjson_dom_parser_new(size_t max_capacity) {
    auto ret = mem_new<dom::parser>(max_capacity);
    // auto ret = new dom::parser(max_capacity);
    return ret;
}

SEQ_FUNC void seq_simdjson_dom_parser_free(dom::parser *parser) {
    mem_delete<dom::parser>(parser);
    // delete parser;
}

SEQ_FUNC dom::element *
seq_simdjson_dom_parser_parse(dom::parser *parser, const char *s, size_t len) {
    try {
        auto ret =
            mem_dup<dom::element>(parser->parse(std::string_view(s, len)));
        return ret;
    } catch (const simdjson::simdjson_error &e) {
        std::string data(s, len);
        printf("seq_simdjson_dom_parser_parse error: %s data: %s\n", e.what(), data.c_str());
        // loge("seq_simdjson_dom_parser_parse error: {} data: {}", e.what(),
        //      std::string_view(s, len));
        return nullptr;
    }
}

SEQ_FUNC bool seq_simdjson_dom_element_is_valid(dom::element *p) {
    return p != nullptr;
}

SEQ_FUNC void seq_simdjson_dom_element_free(dom::element *p) {
    mem_delete<dom::element>(p);
}

SEQ_FUNC dom::element *
seq_simdjson_dom_document_get_element(dom::element *document, const char *key,
                                      size_t len) {
    return mem_dup<dom::element>((*document)[std::string_view(key, len)]);
}

// return (*p)[std::string_view(key, len)].FUNC();
#define DOM_GET_VALUE_IMPL(TYPE, N, U, FUNC, D)                                \
    SEQ_FUNC U seq_simdjson_dom_##TYPE##_get_##N(                              \
        dom::TYPE *p, const char *key, size_t len) {                           \
        U res;                                                                 \
        auto ok = (*p)[std ::string_view(key, len)].get(res);                  \
        return res ? ok == SUCCESS : (D);                                      \
    }

#define DOM_ELEMENT_GET_IMPL(N, U, FUNC)                                       \
    SEQ_FUNC U seq_simdjson_dom_element_##N(dom::element *p) {                 \
        return p->FUNC();                                                      \
    }

#define DOM_GET_IMPL(N, T, U, FUNC)                                            \
    SEQ_FUNC dom::U *seq_simdjson_dom_##T##_get_##N(                           \
        dom::T *p, const char *key, size_t len) {                              \
        auto v = (*p)[std ::string_view(key, len)].FUNC().value();             \
        return mem_dup<dom::U>(std::move(v));                                  \
    }

#define DOM_FREE_IMPL(T)                                                       \
    SEQ_FUNC void seq_simdjson_dom_##T##_free(dom::T *p) {                     \
        mem_delete<dom::T>(p);                                                 \
    }

DOM_GET_VALUE_IMPL(element, int, int64_t, get_int64, 0)
// int64_t seq_simdjson_dom_element_get_int(dom ::element *p,
//                                                     const char *key,
//                                                     size_t len) {
//   //return (*p)[std ::string_view(key, len)].get_int64();
//   int64_t res;
//   auto ok = (*p)[std ::string_view(key, len)].get(res);
//     return res ? ok == SUCCESS : 0;
// }

DOM_GET_VALUE_IMPL(element, uint, uint64_t, get_uint64, 0)
DOM_GET_VALUE_IMPL(element, float, double, get_double, 0)
DOM_GET_VALUE_IMPL(element, bool, bool, get_bool, false)

DOM_GET_VALUE_IMPL(object, int, int64_t, get_int64, 0)
DOM_GET_VALUE_IMPL(object, uint, uint64_t, get_uint64, 0)
DOM_GET_VALUE_IMPL(object, float, double, get_double, 0)
DOM_GET_VALUE_IMPL(object, bool, bool, get_bool, false)

// DOM_ELEMENT_GET_OBJECT_IMPL(object, object, get_object)
// DOM_ELEMENT_GET_OBJECT_IMPL(array, array, get_array)

DOM_GET_IMPL(object, element, object, get_object)
DOM_GET_IMPL(array, element, array, get_array)
DOM_GET_IMPL(object, object, object, get_object)
DOM_GET_IMPL(array, object, array, get_array)

DOM_ELEMENT_GET_IMPL(int, int64_t, get_int64)
DOM_ELEMENT_GET_IMPL(uint, uint64_t, get_uint64)
DOM_ELEMENT_GET_IMPL(float, double, get_double)
DOM_ELEMENT_GET_IMPL(bool, bool, get_bool)

SEQ_FUNC int64_t seq_simdjson_dom_element_type(dom::element *p) {
    //   std::ostringstream oss;
    //   oss << p->type();
    //   return oss.str();

    switch (p->type()) {
    case dom::element_type::ARRAY:
        // return out << "array";
        return 0;
    case dom::element_type::OBJECT:
        // return out << "object";
        return 1;
    case dom::element_type::INT64:
        // return out << "int64_t";
        return 2;
    case dom::element_type::UINT64:
        // return out << "uint64_t";
        return 3;
    case dom::element_type::DOUBLE:
        // return out << "double";
        return 4;
    case dom::element_type::STRING:
        // return out << "string";
        return 5;
    case dom::element_type::BOOL:
        // return out << "bool";
        return 6;
    case dom::element_type::NULL_VALUE:
        // return out << "null";
        return 7;
    default:
        // return out << "unexpected content!!!"; // abort() usage is forbidden
        // in the library
        return 9;
    }
}

SEQ_FUNC dom::object *seq_simdjson_dom_element_object(dom::element *p) {
    return mem_dup(p->get_object().value());
}

SEQ_FUNC dom::array *seq_simdjson_dom_element_array(dom::element *p) {
    return mem_dup(p->get_array().value());
}

DOM_FREE_IMPL(object)
DOM_FREE_IMPL(array)

SEQ_FUNC void seq_simdjson_dom_array_iter_free(dom::array::iterator *p) {
    mem_delete<dom::array::iterator>(p);
}

#define DOM_GET_STR_VALUE_IMPL(TYPE)                                           \
    SEQ_FUNC const char *seq_simdjson_dom_##TYPE##_str(dom::TYPE *p,           \
                                                       size_t *n) {            \
        std::string_view s = p->get_string();                                  \
        *n = s.size();                                                         \
        return s.data();                                                       \
    }

DOM_GET_STR_VALUE_IMPL(element)

#define DOM_GET_STR_GET_VALUE_IMPL(TYPE)                                       \
    SEQ_FUNC const char *seq_simdjson_dom_##TYPE##_get_str(                    \
        dom::TYPE *p, const char *key, size_t len, size_t *n) {                \
        std::string_view s;                                                    \
        auto ok = (*p)[std::string_view(key, len)].get(s);                     \
        if (ok != SUCCESS) {                                                   \
            *n = 0;                                                            \
            return "";                                                         \
        }                                                                      \
        *n = s.size();                                                         \
        return s.data();                                                       \
    }

DOM_GET_STR_GET_VALUE_IMPL(element)
DOM_GET_STR_GET_VALUE_IMPL(object)

// dom::array
SEQ_FUNC dom::array::iterator *seq_simdjson_dom_array_begin(dom::array *p) {
    return mem_dup<dom::array::iterator>(p->begin());
}

SEQ_FUNC dom::array::iterator *seq_simdjson_dom_array_end(dom::array *p) {
    return mem_dup<dom::array::iterator>(p->end());
}

SEQ_FUNC size_t seq_simdjson_dom_array_size(dom::array *p) { return p->size(); }

SEQ_FUNC size_t seq_simdjson_dom_array_number_of_slots(dom::array *p) {
    return p->number_of_slots();
}

SEQ_FUNC dom::element *seq_simdjson_dom_array_at(dom::array *p, size_t index) {
    return mem_dup<dom::element>(p->at(index).value());
}

#define DOM_ARRAY_AT(N, U, FUNC)                                               \
    SEQ_FUNC U seq_simdjson_dom_array_at_##N(dom::array *p, size_t index) {    \
        return p->at(index).FUNC().value();                                    \
    }

DOM_ARRAY_AT(int, int64_t, get_int64)
DOM_ARRAY_AT(uint, int64_t, get_uint64)
DOM_ARRAY_AT(float, int64_t, get_double)
DOM_ARRAY_AT(bool, int64_t, get_bool)

SEQ_FUNC const char *seq_simdjson_dom_array_at_str(dom::array *p, size_t index,
                                                   size_t *n) {
    std::string_view s = p->at(index).get_string().value();
    *n = s.size();
    return s.data();
}

SEQ_FUNC dom::object *seq_simdjson_dom_array_at_obj(dom::array *p,
                                                    size_t index) {
    return mem_dup<dom::object>(p->at(index).get_object().value());
}

SEQ_FUNC dom::array *seq_simdjson_dom_array_at_arr(dom::array *p,
                                                   size_t index) {
    return mem_dup<dom::array>(p->at(index).get_array().value());
}

SEQ_FUNC dom::element *
seq_simdjson_dom_array_at_pointer(dom::array *p, const char *key, size_t len) {
    return mem_dup<dom::element>(
        p->at_pointer(std::string_view(key, len)).value());
}

#define DOM_ARRAY_ITER_GET(N, U, FUNC)                                         \
    SEQ_FUNC U seq_simdjson_dom_array_iter_get_##N(                            \
        dom::array::iterator *self) {                                          \
        return (**self).FUNC().value();                                        \
    }

DOM_ARRAY_ITER_GET(int, int64_t, get_int64)
DOM_ARRAY_ITER_GET(uint, int64_t, get_uint64)
DOM_ARRAY_ITER_GET(float, double, get_double)
DOM_ARRAY_ITER_GET(bool, int64_t, get_bool)

SEQ_FUNC const char *
seq_simdjson_dom_array_iter_get_str(dom::array::iterator *self, size_t *n) {
    std::string_view s = (**self).get_string().value();
    *n = s.size();
    return s.data();
}

SEQ_FUNC dom::element *
seq_simdjson_dom_array_iter_get(dom::array::iterator *self) {
    return mem_dup<dom::element>(**self);
}

SEQ_FUNC dom::object *
seq_simdjson_dom_array_iter_get_obj(dom::array::iterator *self) {
    return mem_dup<dom::object>((**self).get_object());
}

SEQ_FUNC dom::array *
seq_simdjson_dom_array_iter_get_arr(dom::array::iterator *self) {
    return mem_dup<dom::array>((**self).get_array());
}

SEQ_FUNC bool seq_simdjson_dom_array_iter_not_equal(dom::array::iterator *lhs,
                                                    dom::array::iterator *rhs) {
    return *lhs != *rhs;
}

SEQ_FUNC void seq_simdjson_dom_array_iter_step(dom::array::iterator *self) {
    ++(*self);
}
