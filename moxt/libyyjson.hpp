//
// Created by ku on 2023/9/26.
//

#ifndef MOXT_LIBYYJSON_HPP
#define MOXT_LIBYYJSON_HPP

#include "common.hpp"
#include <yyjson.h>

/**
 Read a JSON string.

 This function is thread-safe.

 @param dat The JSON data (UTF-8 without BOM), null-terminator is not required.
    If this parameter is NULL, the function will fail and return NULL.
 @param len The length of JSON data in bytes.
    If this parameter is 0, the function will fail and return NULL.
 @param flg The JSON read options.
    Multiple options can be combined with `|` operator. 0 means no options.
 @return A new JSON document, or NULL if an error occurs.
    When it's no longer needed, it should be freed with `yyjson_doc_free()`.
 */
SEQ_FUNC yyjson_doc *seq_yyjson_read(const char *dat, size_t len,
                                     yyjson_read_flag flg);

/*==============================================================================
 * JSON Document API (Implementation)
 *============================================================================*/

SEQ_FUNC yyjson_val *seq_yyjson_doc_get_root(yyjson_doc *doc);

SEQ_FUNC size_t seq_yyjson_doc_get_read_size(yyjson_doc *doc);

SEQ_FUNC size_t seq_yyjson_doc_get_val_count(yyjson_doc *doc);

SEQ_FUNC void seq_yyjson_doc_free(yyjson_doc *doc);

/*==============================================================================
 * JSON Object API
 *============================================================================*/

/** Returns the number of key-value pairs in this object.
    Returns 0 if `obj` is NULL or type is not object. */
SEQ_FUNC size_t seq_yyjson_obj_size(yyjson_val *obj);

/** Returns the value to which the specified key is mapped.
    Returns NULL if this object contains no mapping for the key.
    Returns NULL if `obj/key` is NULL, or type is not object.

    The `key` should be a null-terminated UTF-8 string.

    @warning This function takes a linear search time. */
SEQ_FUNC yyjson_val *seq_yyjson_obj_get(yyjson_val *obj, const char *key);

/** Returns the value to which the specified key is mapped.
    Returns NULL if this object contains no mapping for the key.
    Returns NULL if `obj/key` is NULL, or type is not object.

    The `key` should be a UTF-8 string, null-terminator is not required.
    The `key_len` should be the length of the key, in bytes.

    @warning This function takes a linear search time. */
SEQ_FUNC yyjson_val *seq_yyjson_obj_getn(yyjson_val *obj, const char *key,
                                         size_t key_len);

/** Returns the JSON value's type.
    Returns YYJSON_TYPE_NONE if `val` is NULL. */
SEQ_FUNC yyjson_type seq_yyjson_get_type(yyjson_val *val);

/** Returns the JSON value's subtype.
    Returns YYJSON_SUBTYPE_NONE if `val` is NULL. */
SEQ_FUNC yyjson_subtype seq_yyjson_get_subtype(yyjson_val *val);

/** Returns the JSON value's tag.
    Returns 0 if `val` is NULL. */
SEQ_FUNC uint8_t seq_yyjson_get_tag(yyjson_val *val);

/** Returns the JSON value's type description.
    The return value should be one of these strings: "raw", "null", "string",
    "array", "object", "true", "false", "uint", "sint", "real", "unknown". */
SEQ_FUNC const char *seq_yyjson_get_type_desc(yyjson_val *val);

/** Returns the content if the value is raw.
    Returns NULL if `val` is NULL or type is not raw. */
SEQ_FUNC const char *seq_yyjson_get_raw(yyjson_val *val);

/** Returns the content if the value is bool.
    Returns NULL if `val` is NULL or type is not bool. */
SEQ_FUNC bool seq_yyjson_get_bool(yyjson_val *val);

/** Returns the content and cast to uint64_t.
    Returns 0 if `val` is NULL or type is not integer(sint/uint). */
SEQ_FUNC uint64_t seq_yyjson_get_uint(yyjson_val *val);

/** Returns the content and cast to int64_t.
    Returns 0 if `val` is NULL or type is not integer(sint/uint). */
SEQ_FUNC int64_t seq_yyjson_get_sint(yyjson_val *val);

/** Returns the content and cast to int.
    Returns 0 if `val` is NULL or type is not integer(sint/uint). */
SEQ_FUNC int seq_yyjson_get_int(yyjson_val *val);

/** Returns the content if the value is real number, or 0.0 on error.
    Returns 0.0 if `val` is NULL or type is not real(double). */
SEQ_FUNC double seq_yyjson_get_real(yyjson_val *val);

/** Returns the content and typecast to `double` if the value is number.
    Returns 0.0 if `val` is NULL or type is not number(uint/sint/real). */
SEQ_FUNC double seq_yyjson_get_num(yyjson_val *val);
/** Returns the content if the value is string.
    Returns NULL if `val` is NULL or type is not string. */
SEQ_FUNC const char *seq_yyjson_get_str(yyjson_val *val);

/** Returns the content length (string length, array size, object size.
    Returns 0 if `val` is NULL or type is not string/array/object. */
SEQ_FUNC size_t seq_yyjson_get_len(yyjson_val *val);

/** Returns whether the JSON value is equals to a string.
    Returns false if input is NULL or type is not string. */
SEQ_FUNC bool seq_yyjson_equals_str(yyjson_val *val, const char *str);

/** Returns whether the JSON value is equals to a string.
    The `str` should be a UTF-8 string, null-terminator is not required.
    Returns false if input is NULL or type is not string. */
SEQ_FUNC bool seq_yyjson_equals_strn(yyjson_val *val, const char *str,
                                     size_t len);

/** Returns whether two JSON values are equal (deep compare).
    Returns false if input is NULL.
    @note the result may be inaccurate if object has duplicate keys.
    @warning This function is recursive and may cause a stack overflow
        if the object level is too deep. */
SEQ_FUNC bool seq_yyjson_equals(yyjson_val *lhs, yyjson_val *rhs);

/** Set the value to raw.
    Returns false if input is NULL or `val` is object or array.
    @warning This will modify the `immutable` value, use with caution. */
SEQ_FUNC bool seq_yyjson_set_raw(yyjson_val *val, const char *raw, size_t len);

/** Set the value to null.
    Returns false if input is NULL or `val` is object or array.
    @warning This will modify the `immutable` value, use with caution. */
SEQ_FUNC bool seq_yyjson_set_null(yyjson_val *val);

/** Set the value to bool.
    Returns false if input is NULL or `val` is object or array.
    @warning This will modify the `immutable` value, use with caution. */
SEQ_FUNC bool seq_yyjson_set_bool(yyjson_val *val, bool num);

/** Set the value to uint.
    Returns false if input is NULL or `val` is object or array.
    @warning This will modify the `immutable` value, use with caution. */
SEQ_FUNC bool seq_yyjson_set_uint(yyjson_val *val, uint64_t num);

/** Set the value to sint.
    Returns false if input is NULL or `val` is object or array.
    @warning This will modify the `immutable` value, use with caution. */
SEQ_FUNC bool seq_yyjson_set_sint(yyjson_val *val, int64_t num);

/** Set the value to int.
    Returns false if input is NULL or `val` is object or array.
    @warning This will modify the `immutable` value, use with caution. */
SEQ_FUNC bool seq_yyjson_set_int(yyjson_val *val, int num);

/** Set the value to real.
    Returns false if input is NULL or `val` is object or array.
    @warning This will modify the `immutable` value, use with caution. */
SEQ_FUNC bool seq_yyjson_set_real(yyjson_val *val, double num);

/** Set the value to string (null-terminated).
    Returns false if input is NULL or `val` is object or array.
    @warning This will modify the `immutable` value, use with caution. */
SEQ_FUNC bool seq_yyjson_set_str(yyjson_val *val, const char *str);

/** Set the value to string (with length).
    Returns false if input is NULL or `val` is object or array.
    @warning This will modify the `immutable` value, use with caution. */
SEQ_FUNC bool seq_yyjson_set_strn(yyjson_val *val, const char *str, size_t len);

/*==============================================================================
 * JSON Array API
 *============================================================================*/

/** Returns the number of elements in this array.
    Returns 0 if `arr` is NULL or type is not array. */
SEQ_FUNC size_t seq_yyjson_arr_size(yyjson_val *arr);

/** Returns the element at the specified position in this array.
    Returns NULL if array is NULL/empty or the index is out of bounds.
    @warning This function takes a linear search time if array is not flat.
        For example: `[1,{},3]` is flat, `[1,[2],3]` is not flat. */
SEQ_FUNC yyjson_val *seq_yyjson_arr_get(yyjson_val *arr, size_t idx);

/** Returns the first element of this array.
    Returns NULL if `arr` is NULL/empty or type is not array. */
SEQ_FUNC yyjson_val *seq_yyjson_arr_get_first(yyjson_val *arr);

/** Returns the last element of this array.
    Returns NULL if `arr` is NULL/empty or type is not array.
    @warning This function takes a linear search time if array is not flat.
        For example: `[1,{},3]` is flat, `[1,[2],3]` is not flat.*/
SEQ_FUNC yyjson_val *seq_yyjson_arr_get_last(yyjson_val *arr);

/*==============================================================================
 * JSON Object Iterator API
 *============================================================================*/

/**
 A JSON object iterator.

 @par Example
 @code
    yyjson_val *key, *val;
    yyjson_obj_iter iter = yyjson_obj_iter_with(obj);
    while ((key = yyjson_obj_iter_next(&iter))) {
        val = yyjson_obj_iter_get_val(key);
        your_func(key, val);
    }
 @endcode
 */

/**
 Initialize an iterator for this object.

 @param obj The object to be iterated over.
    If this parameter is NULL or not an object, `iter` will be set to empty.
 @param iter The iterator to be initialized.
    If this parameter is NULL, the function will fail and return false.
 @return true if the `iter` has been successfully initialized.

 @note The iterator does not need to be destroyed.
 */
SEQ_FUNC bool seq_yyjson_obj_iter_init(yyjson_val *obj, yyjson_obj_iter *iter);

/**
 Create an iterator with an object, same as `yyjson_obj_iter_init()`.

 @param obj The object to be iterated over.
    If this parameter is NULL or not an object, an empty iterator will returned.
 @return A new iterator for the object.

 @note The iterator does not need to be destroyed.
 */
SEQ_FUNC yyjson_obj_iter seq_yyjson_obj_iter_with(yyjson_val *obj);

/**
 Returns whether the iteration has more elements.
 If `iter` is NULL, this function will return false.
 */
SEQ_FUNC bool seq_yyjson_obj_iter_has_next(yyjson_obj_iter *iter);

/**
 Returns the next key in the iteration, or NULL on end.
 If `iter` is NULL, this function will return NULL.
 */
SEQ_FUNC yyjson_val *seq_yyjson_obj_iter_next(yyjson_obj_iter *iter);

/**
 Returns the value for key inside the iteration.
 If `iter` is NULL, this function will return NULL.
 */
SEQ_FUNC yyjson_val *seq_yyjson_obj_iter_get_val(yyjson_val *key);

/**
 Iterates to a specified key and returns the value.

 This function does the same thing as `yyjson_obj_get()`, but is much faster
 if the ordering of the keys is known at compile-time and you are using the same
 order to look up the values. If the key exists in this object, then the
 iterator will stop at the next key, otherwise the iterator will not change and
 NULL is returned.

 @param iter The object iterator, should not be NULL.
 @param key The key, should be a UTF-8 string with null-terminator.
 @return The value to which the specified key is mapped.
    NULL if this object contains no mapping for the key or input is invalid.

 @warning This function takes a linear search time if the key is not nearby.
 */
SEQ_FUNC yyjson_val *seq_yyjson_obj_iter_get(yyjson_obj_iter *iter,
                                             const char *key);

/**
 Iterates to a specified key and returns the value.

 This function does the same thing as `yyjson_obj_getn()`, but is much faster
 if the ordering of the keys is known at compile-time and you are using the same
 order to look up the values. If the key exists in this object, then the
 iterator will stop at the next key, otherwise the iterator will not change and
 NULL is returned.

 @param iter The object iterator, should not be NULL.
 @param key The key, should be a UTF-8 string, null-terminator is not required.
 @param key_len The the length of `key`, in bytes.
 @return The value to which the specified key is mapped.
    NULL if this object contains no mapping for the key or input is invalid.

 @warning This function takes a linear search time if the key is not nearby.
 */
SEQ_FUNC yyjson_val *seq_yyjson_obj_iter_getn(yyjson_obj_iter *iter,
                                              const char *key, size_t key_len);

SEQ_FUNC yyjson_val *seq_unsafe_yyjson_get_first(yyjson_val *ctn);

SEQ_FUNC yyjson_val *seq_unsafe_yyjson_get_next(yyjson_val *val);

SEQ_FUNC yyjson_obj_iter *seq_yyjson_obj_iter_ptr_new(yyjson_val *obj);

SEQ_FUNC void seq_yyjson_obj_iter_ptr_free(yyjson_obj_iter *iter_ptr);

SEQ_FUNC void seq_yyjson_obj_foreach_test(yyjson_val *val);

/*==============================================================================
 * Mutable JSON Object API
 *============================================================================*/

/** Returns the number of key-value pairs in this object.
    Returns 0 if `obj` is NULL or type is not object. */
SEQ_FUNC size_t seq_yyjson_mut_obj_size(yyjson_mut_val *obj);

/** Returns the value to which the specified key is mapped.
    Returns NULL if this object contains no mapping for the key.
    Returns NULL if `obj/key` is NULL, or type is not object.

    The `key` should be a null-terminated UTF-8 string.

    @warning This function takes a linear search time. */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_obj_get(yyjson_mut_val *obj,
                                                const char *key);

/** Returns the value to which the specified key is mapped.
    Returns NULL if this object contains no mapping for the key.
    Returns NULL if `obj/key` is NULL, or type is not object.

    The `key` should be a UTF-8 string, null-terminator is not required.
    The `key_len` should be the length of the key, in bytes.

    @warning This function takes a linear search time. */
SEQ_FUNC yyjson_mut_val *
seq_yyjson_mut_obj_getn(yyjson_mut_val *obj, const char *key, size_t key_len);

/*==============================================================================
 * Mutable JSON Document API
 *============================================================================*/

/** Returns the root value of this JSON document.
    Returns NULL if `doc` is NULL. */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_doc_get_root(yyjson_mut_doc *doc);

/** Sets the root value of this JSON document.
    Pass NULL to clear root value of the document. */
SEQ_FUNC void seq_yyjson_mut_doc_set_root(yyjson_mut_doc *doc,
                                          yyjson_mut_val *root);

/**
 Set the string pool size for a mutable document.
 This function does not allocate memory immediately, but uses the size when
 the next memory allocation is needed.

 If the caller knows the approximate bytes of strings that the document needs to
 store (e.g. copy string with `yyjson_mut_strcpy` function), setting a larger
 size can avoid multiple memory allocations and improve performance.

 @param doc The mutable document.
 @param len The desired string pool size in bytes (total string length).
 @return true if successful, false if size is 0 or overflow.
 */
SEQ_FUNC bool seq_yyjson_mut_doc_set_str_pool_size(yyjson_mut_doc *doc,
                                                   size_t len);

/**
 Set the value pool size for a mutable document.
 This function does not allocate memory immediately, but uses the size when
 the next memory allocation is needed.

 If the caller knows the approximate number of values that the document needs to
 store (e.g. create new value with `yyjson_mut_xxx` functions), setting a larger
 size can avoid multiple memory allocations and improve performance.

 @param doc The mutable document.
 @param count The desired value pool size (number of `yyjson_mut_val`).
 @return true if successful, false if size is 0 or overflow.
 */
SEQ_FUNC bool seq_yyjson_mut_doc_set_val_pool_size(yyjson_mut_doc *doc,
                                                   size_t count);

/** Release the JSON document and free the memory.
    After calling this function, the `doc` and all values from the `doc` are no
    longer available. This function will do nothing if the `doc` is NULL.  */
SEQ_FUNC void seq_yyjson_mut_doc_free(yyjson_mut_doc *doc);

/** Creates and returns a new mutable JSON document, returns NULL on error.
    If allocator is NULL, the default allocator will be used. */
SEQ_FUNC yyjson_mut_doc *seq_yyjson_mut_doc_new(const yyjson_alc *alc);

/** Copies and returns a new mutable document from input, returns NULL on error.
    This makes a `deep-copy` on the immutable document.
    If allocator is NULL, the default allocator will be used.
    @note `imut_doc` -> `mut_doc`. */
SEQ_FUNC yyjson_mut_doc *seq_yyjson_doc_mut_copy(yyjson_doc *doc,
                                                 const yyjson_alc *alc);

/** Copies and returns a new mutable document from input, returns NULL on error.
    This makes a `deep-copy` on the mutable document.
    If allocator is NULL, the default allocator will be used.
    @note `mut_doc` -> `mut_doc`. */
SEQ_FUNC yyjson_mut_doc *seq_yyjson_mut_doc_mut_copy(yyjson_mut_doc *doc,
                                                     const yyjson_alc *alc);

/** Copies and returns a new mutable value from input, returns NULL on error.
    This makes a `deep-copy` on the immutable value.
    The memory was managed by mutable document.
    @note `imut_val` -> `mut_val`. */
SEQ_FUNC yyjson_mut_val *seq_yyjson_val_mut_copy(yyjson_mut_doc *doc,
                                                 yyjson_val *val);

/** Copies and returns a new mutable value from input, returns NULL on error.
    This makes a `deep-copy` on the mutable value.
    The memory was managed by mutable document.
    @note `mut_val` -> `mut_val`.
    @warning This function is recursive and may cause a stack overflow
        if the object level is too deep. */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_val_mut_copy(yyjson_mut_doc *doc,
                                                     yyjson_mut_val *val);

/** Copies and returns a new immutable document from input,
    returns NULL on error. This makes a `deep-copy` on the mutable document.
    The returned document should be freed with `yyjson_doc_free()`.
    @note `mut_doc` -> `imut_doc`.
    @warning This function is recursive and may cause a stack overflow
        if the object level is too deep. */
SEQ_FUNC yyjson_doc *seq_yyjson_mut_doc_imut_copy(yyjson_mut_doc *doc,
                                                  const yyjson_alc *alc);

/** Copies and returns a new immutable document from input,
    returns NULL on error. This makes a `deep-copy` on the mutable value.
    The returned document should be freed with `yyjson_doc_free()`.
    @note `mut_val` -> `imut_doc`.
    @warning This function is recursive and may cause a stack overflow
        if the object level is too deep. */
SEQ_FUNC yyjson_doc *seq_yyjson_mut_val_imut_copy(yyjson_mut_val *val,
                                                  const yyjson_alc *alc);

SEQ_FUNC void seq_yyjson_mut_doc_set_root(yyjson_mut_doc *doc,
                                          yyjson_mut_val *root);

/*==============================================================================
 * Mutable JSON Object Modification Convenience API (Implementation)
 *============================================================================*/

// #define yyjson_mut_obj_add_func(func) \
//     if (yyjson_likely(doc && yyjson_mut_is_obj(obj) && _key)) { \
//         yyjson_mut_val *key = unsafe_yyjson_mut_val(doc, 2); \
//         if (yyjson_likely(key)) { \
//             size_t len = unsafe_yyjson_get_len(obj); \
//             yyjson_mut_val *val = key + 1; \
//             size_t key_len = strlen(_key); \
//             bool noesc = unsafe_yyjson_is_str_noesc(_key, key_len); \
//             key->tag = YYJSON_TYPE_STR; \
//             key->tag |= noesc ? YYJSON_SUBTYPE_NOESC : YYJSON_SUBTYPE_NONE; \
//             key->tag |= (uint64_t)strlen(_key) << YYJSON_TAG_BIT; \
//             key->uni.str = _key; \
//             func \
//             unsafe_yyjson_mut_obj_add(obj, key, val, len); \
//             return true; \
//         } \
//     } \
//     return false

SEQ_FUNC bool seq_yyjson_mut_obj_add_null(yyjson_mut_doc *doc,
                                          yyjson_mut_val *obj,
                                          const char *_key);

SEQ_FUNC bool seq_yyjson_mut_obj_add_true(yyjson_mut_doc *doc,
                                          yyjson_mut_val *obj,
                                          const char *_key);

SEQ_FUNC bool seq_yyjson_mut_obj_add_false(yyjson_mut_doc *doc,
                                           yyjson_mut_val *obj,
                                           const char *_key);

SEQ_FUNC bool seq_yyjson_mut_obj_add_bool(yyjson_mut_doc *doc,
                                          yyjson_mut_val *obj, const char *_key,
                                          bool _val);

SEQ_FUNC bool seq_yyjson_mut_obj_add_uint(yyjson_mut_doc *doc,
                                          yyjson_mut_val *obj, const char *_key,
                                          uint64_t _val);

SEQ_FUNC bool seq_yyjson_mut_obj_add_sint(yyjson_mut_doc *doc,
                                          yyjson_mut_val *obj, const char *_key,
                                          int64_t _val);

SEQ_FUNC bool seq_yyjson_mut_obj_add_int(yyjson_mut_doc *doc,
                                         yyjson_mut_val *obj, const char *_key,
                                         int64_t _val);

SEQ_FUNC bool seq_yyjson_mut_obj_add_real(yyjson_mut_doc *doc,
                                          yyjson_mut_val *obj, const char *_key,
                                          double _val);

SEQ_FUNC bool seq_yyjson_mut_obj_add_str(yyjson_mut_doc *doc,
                                         yyjson_mut_val *obj, const char *_key,
                                         const char *_val);

SEQ_FUNC bool seq_yyjson_mut_obj_add_strn(yyjson_mut_doc *doc,
                                          yyjson_mut_val *obj, const char *_key,
                                          const char *_val, size_t _len);

SEQ_FUNC bool seq_yyjson_mut_obj_add_strcpy(yyjson_mut_doc *doc,
                                            yyjson_mut_val *obj,
                                            const char *_key, const char *_val);

SEQ_FUNC bool seq_yyjson_mut_obj_add_strncpy(yyjson_mut_doc *doc,
                                             yyjson_mut_val *obj,
                                             const char *_key, const char *_val,
                                             size_t _len);

SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_obj_add_arr(yyjson_mut_doc *doc,
                                                    yyjson_mut_val *obj,
                                                    const char *_key);

SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_obj_add_obj(yyjson_mut_doc *doc,
                                                    yyjson_mut_val *obj,
                                                    const char *_key);

SEQ_FUNC bool seq_yyjson_mut_obj_add_val(yyjson_mut_doc *doc,
                                         yyjson_mut_val *obj, const char *_key,
                                         yyjson_mut_val *_val);

SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_obj_remove_str(yyjson_mut_val *obj,
                                                       const char *key);

SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_obj_remove_strn(yyjson_mut_val *obj,
                                                        const char *_key,
                                                        size_t _len);

SEQ_FUNC bool seq_yyjson_mut_obj_rename_key(yyjson_mut_doc *doc,
                                            yyjson_mut_val *obj,
                                            const char *key,
                                            const char *new_key);

SEQ_FUNC bool seq_yyjson_mut_obj_rename_keyn(yyjson_mut_doc *doc,
                                             yyjson_mut_val *obj,
                                             const char *key, size_t len,
                                             const char *new_key,
                                             size_t new_len);

/*==============================================================================
 * Mutable JSON Array Creation API
 *============================================================================*/

/**
 Creates and returns an empty mutable array.
 @param doc A mutable document, used for memory allocation only.
 @return The new array. NULL if input is NULL or memory allocation failed.
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr(yyjson_mut_doc *doc);

/**
 Creates and returns a new mutable array with the given boolean values.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of boolean values.
 @param count The value count. If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const bool vals[3] = { true, false, true };
    yyjson_mut_val *arr = yyjson_mut_arr_with_bool(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_bool(yyjson_mut_doc *doc,
                                                      const bool *vals,
                                                      size_t count);

/**
 Creates and returns a new mutable array with the given sint numbers.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of sint numbers.
 @param count The number count. If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const int64_t vals[3] = { -1, 0, 1 };
    yyjson_mut_val *arr = yyjson_mut_arr_with_sint64(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_sint(yyjson_mut_doc *doc,
                                                      const int64_t *vals,
                                                      size_t count);

/**
 Creates and returns a new mutable array with the given uint numbers.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of uint numbers.
 @param count The number count. If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const uint64_t vals[3] = { 0, 1, 0 };
    yyjson_mut_val *arr = yyjson_mut_arr_with_uint(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_uint(yyjson_mut_doc *doc,
                                                      const uint64_t *vals,
                                                      size_t count);

/**
 Creates and returns a new mutable array with the given real numbers.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of real numbers.
 @param count The number count. If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const double vals[3] = { 0.1, 0.2, 0.3 };
    yyjson_mut_val *arr = yyjson_mut_arr_with_real(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_real(yyjson_mut_doc *doc,
                                                      const double *vals,
                                                      size_t count);

/**
 Creates and returns a new mutable array with the given int8 numbers.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of int8 numbers.
 @param count The number count. If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const int8_t vals[3] = { -1, 0, 1 };
    yyjson_mut_val *arr = yyjson_mut_arr_with_sint8(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_sint8(yyjson_mut_doc *doc,
                                                       const int8_t *vals,
                                                       size_t count);

/**
 Creates and returns a new mutable array with the given int16 numbers.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of int16 numbers.
 @param count The number count. If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const int16_t vals[3] = { -1, 0, 1 };
    yyjson_mut_val *arr = yyjson_mut_arr_with_sint16(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_sint16(yyjson_mut_doc *doc,
                                                        const int16_t *vals,
                                                        size_t count);

/**
 Creates and returns a new mutable array with the given int32 numbers.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of int32 numbers.
 @param count The number count. If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const int32_t vals[3] = { -1, 0, 1 };
    yyjson_mut_val *arr = yyjson_mut_arr_with_sint32(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_sint32(yyjson_mut_doc *doc,
                                                        const int32_t *vals,
                                                        size_t count);

/**
 Creates and returns a new mutable array with the given int64 numbers.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of int64 numbers.
 @param count The number count. If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const int64_t vals[3] = { -1, 0, 1 };
    yyjson_mut_val *arr = yyjson_mut_arr_with_sint64(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_sint64(yyjson_mut_doc *doc,
                                                        const int64_t *vals,
                                                        size_t count);

/**
 Creates and returns a new mutable array with the given uint8 numbers.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of uint8 numbers.
 @param count The number count. If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const uint8_t vals[3] = { 0, 1, 0 };
    yyjson_mut_val *arr = yyjson_mut_arr_with_uint8(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_uint8(yyjson_mut_doc *doc,
                                                       const uint8_t *vals,
                                                       size_t count);

/**
 Creates and returns a new mutable array with the given uint16 numbers.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of uint16 numbers.
 @param count The number count. If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const uint16_t vals[3] = { 0, 1, 0 };
    yyjson_mut_val *arr = yyjson_mut_arr_with_uint16(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_uint16(yyjson_mut_doc *doc,
                                                        const uint16_t *vals,
                                                        size_t count);

/**
 Creates and returns a new mutable array with the given uint32 numbers.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of uint32 numbers.
 @param count The number count. If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const uint32_t vals[3] = { 0, 1, 0 };
    yyjson_mut_val *arr = yyjson_mut_arr_with_uint32(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_uint32(yyjson_mut_doc *doc,
                                                        const uint32_t *vals,
                                                        size_t count);

/**
 Creates and returns a new mutable array with the given uint64 numbers.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of uint64 numbers.
 @param count The number count. If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
     const uint64_t vals[3] = { 0, 1, 0 };
     yyjson_mut_val *arr = yyjson_mut_arr_with_uint64(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_uint64(yyjson_mut_doc *doc,
                                                        const uint64_t *vals,
                                                        size_t count);

/**
 Creates and returns a new mutable array with the given float numbers.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of float numbers.
 @param count The number count. If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const float vals[3] = { -1.0f, 0.0f, 1.0f };
    yyjson_mut_val *arr = yyjson_mut_arr_with_float(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_float(yyjson_mut_doc *doc,
                                                       const float *vals,
                                                       size_t count);

/**
 Creates and returns a new mutable array with the given double numbers.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of double numbers.
 @param count The number count. If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const double vals[3] = { -1.0, 0.0, 1.0 };
    yyjson_mut_val *arr = yyjson_mut_arr_with_double(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_double(yyjson_mut_doc *doc,
                                                        const double *vals,
                                                        size_t count);

/**
 Creates and returns a new mutable array with the given strings, these strings
 will not be copied.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of UTF-8 null-terminator strings.
    If this array contains NULL, the function will fail and return NULL.
 @param count The number of values in `vals`.
    If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @warning The input strings are not copied, you should keep these strings
    unmodified for the lifetime of this JSON document. If these strings will be
    modified, you should use `yyjson_mut_arr_with_strcpy()` instead.

 @par Example
 @code
    const char *vals[3] = { "a", "b", "c" };
    yyjson_mut_val *arr = yyjson_mut_arr_with_str(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_str(yyjson_mut_doc *doc,
                                                     const char **vals,
                                                     size_t count);

/**
 Creates and returns a new mutable array with the given strings and string
 lengths, these strings will not be copied.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of UTF-8 strings, null-terminator is not required.
    If this array contains NULL, the function will fail and return NULL.
 @param lens A C array of string lengths, in bytes.
 @param count The number of strings in `vals`.
    If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @warning The input strings are not copied, you should keep these strings
    unmodified for the lifetime of this JSON document. If these strings will be
    modified, you should use `yyjson_mut_arr_with_strncpy()` instead.

 @par Example
 @code
    const char *vals[3] = { "a", "bb", "c" };
    const size_t lens[3] = { 1, 2, 1 };
    yyjson_mut_val *arr = yyjson_mut_arr_with_strn(doc, vals, lens, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_strn(yyjson_mut_doc *doc,
                                                      const char **vals,
                                                      const size_t *lens,
                                                      size_t count);

/**
 Creates and returns a new mutable array with the given strings, these strings
 will be copied.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of UTF-8 null-terminator strings.
    If this array contains NULL, the function will fail and return NULL.
 @param count The number of values in `vals`.
    If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const char *vals[3] = { "a", "b", "c" };
    yyjson_mut_val *arr = yyjson_mut_arr_with_strcpy(doc, vals, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_strcpy(yyjson_mut_doc *doc,
                                                        const char **vals,
                                                        size_t count);

/**
 Creates and returns a new mutable array with the given strings and string
 lengths, these strings will be copied.

 @param doc A mutable document, used for memory allocation only.
    If this parameter is NULL, the function will fail and return NULL.
 @param vals A C array of UTF-8 strings, null-terminator is not required.
    If this array contains NULL, the function will fail and return NULL.
 @param lens A C array of string lengths, in bytes.
 @param count The number of strings in `vals`.
    If this value is 0, an empty array will return.
 @return The new array. NULL if input is invalid or memory allocation failed.

 @par Example
 @code
    const char *vals[3] = { "a", "bb", "c" };
    const size_t lens[3] = { 1, 2, 1 };
    yyjson_mut_val *arr = yyjson_mut_arr_with_strn(doc, vals, lens, 3);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_arr_with_strncpy(yyjson_mut_doc *doc,
                                                         const char **vals,
                                                         const size_t *lens,
                                                         size_t count);

/*==============================================================================
 * JSON Document Writer API
 *============================================================================*/

/**
 Write a document to JSON string with options.

 This function is thread-safe when:
 The `alc` is thread-safe or NULL.

 @param doc The JSON document.
    If this doc is NULL or has no root, the function will fail and return false.
 @param flg The JSON write options.
    Multiple options can be combined with `|` operator. 0 means no options.
 @param alc The memory allocator used by JSON writer.
    Pass NULL to use the libc's default allocator.
 @param len A pointer to receive output length in bytes (not including the
    null-terminator). Pass NULL if you don't need length information.
 @param err A pointer to receive error information.
    Pass NULL if you don't need error information.
 @return A new JSON string, or NULL if an error occurs.
    This string is encoded as UTF-8 with a null-terminator.
    When it's no longer needed, it should be freed with free() or alc->free().
 */
SEQ_FUNC char *seq_yyjson_write_opts(const yyjson_doc *doc,
                                     yyjson_write_flag flg,
                                     const yyjson_alc *alc, size_t *len,
                                     yyjson_write_err *err);

/**
 Write a document to JSON file with options.

 This function is thread-safe when:
 1. The file is not accessed by other threads.
 2. The `alc` is thread-safe or NULL.

 @param path The JSON file's path.
    If this path is NULL or invalid, the function will fail and return false.
    If this file is not empty, the content will be discarded.
 @param doc The JSON document.
    If this doc is NULL or has no root, the function will fail and return false.
 @param flg The JSON write options.
    Multiple options can be combined with `|` operator. 0 means no options.
 @param alc The memory allocator used by JSON writer.
    Pass NULL to use the libc's default allocator.
 @param err A pointer to receive error information.
    Pass NULL if you don't need error information.
 @return true if successful, false if an error occurs.

 @warning On 32-bit operating system, files larger than 2GB may fail to write.
 */
SEQ_FUNC bool seq_yyjson_write_file(const char *path, const yyjson_doc *doc,
                                    yyjson_write_flag flg,
                                    const yyjson_alc *alc,
                                    yyjson_write_err *err);

/**
 Write a document to file pointer with options.

 @param fp The file pointer.
    The data will be written to the current position of the file.
    If this fp is NULL or invalid, the function will fail and return false.
 @param doc The JSON document.
    If this doc is NULL or has no root, the function will fail and return false.
 @param flg The JSON write options.
    Multiple options can be combined with `|` operator. 0 means no options.
 @param alc The memory allocator used by JSON writer.
    Pass NULL to use the libc's default allocator.
 @param err A pointer to receive error information.
    Pass NULL if you don't need error information.
 @return true if successful, false if an error occurs.

 @warning On 32-bit operating system, files larger than 2GB may fail to write.
 */
SEQ_FUNC bool seq_yyjson_write_fp(FILE *fp, const yyjson_doc *doc,
                                  yyjson_write_flag flg, const yyjson_alc *alc,
                                  yyjson_write_err *err);

/**
 Write a document to JSON string.

 This function is thread-safe.

 @param doc The JSON document.
    If this doc is NULL or has no root, the function will fail and return false.
 @param flg The JSON write options.
    Multiple options can be combined with `|` operator. 0 means no options.
 @param len A pointer to receive output length in bytes (not including the
    null-terminator). Pass NULL if you don't need length information.
 @return A new JSON string, or NULL if an error occurs.
    This string is encoded as UTF-8 with a null-terminator.
    When it's no longer needed, it should be freed with free().
 */
SEQ_FUNC char *seq_yyjson_write(const yyjson_doc *doc, yyjson_write_flag flg,
                                size_t *len);

/**
 Write a document to JSON string with options.

 This function is thread-safe when:
 1. The `doc` is not modified by other threads.
 2. The `alc` is thread-safe or NULL.

 @param doc The mutable JSON document.
    If this doc is NULL or has no root, the function will fail and return false.
 @param flg The JSON write options.
    Multiple options can be combined with `|` operator. 0 means no options.
 @param alc The memory allocator used by JSON writer.
    Pass NULL to use the libc's default allocator.
 @param len A pointer to receive output length in bytes (not including the
    null-terminator). Pass NULL if you don't need length information.
 @param err A pointer to receive error information.
    Pass NULL if you don't need error information.
 @return A new JSON string, or NULL if an error occurs.
    This string is encoded as UTF-8 with a null-terminator.
    When it's no longer needed, it should be freed with free() or alc->free().
 */
SEQ_FUNC char *seq_yyjson_mut_write_opts(const yyjson_mut_doc *doc,
                                         yyjson_write_flag flg,
                                         const yyjson_alc *alc, size_t *len,
                                         yyjson_write_err *err);

/**
 Write a document to JSON file with options.

 This function is thread-safe when:
 1. The file is not accessed by other threads.
 2. The `doc` is not modified by other threads.
 3. The `alc` is thread-safe or NULL.

 @param path The JSON file's path.
    If this path is NULL or invalid, the function will fail and return false.
    If this file is not empty, the content will be discarded.
 @param doc The mutable JSON document.
    If this doc is NULL or has no root, the function will fail and return false.
 @param flg The JSON write options.
    Multiple options can be combined with `|` operator. 0 means no options.
 @param alc The memory allocator used by JSON writer.
    Pass NULL to use the libc's default allocator.
 @param err A pointer to receive error information.
    Pass NULL if you don't need error information.
 @return true if successful, false if an error occurs.

 @warning On 32-bit operating system, files larger than 2GB may fail to write.
 */
SEQ_FUNC bool seq_yyjson_mut_write_file(const char *path,
                                        const yyjson_mut_doc *doc,
                                        yyjson_write_flag flg,
                                        const yyjson_alc *alc,
                                        yyjson_write_err *err);

/**
 Write a document to file pointer with options.

 @param fp The file pointer.
    The data will be written to the current position of the file.
    If this fp is NULL or invalid, the function will fail and return false.
 @param doc The mutable JSON document.
    If this doc is NULL or has no root, the function will fail and return false.
 @param flg The JSON write options.
    Multiple options can be combined with `|` operator. 0 means no options.
 @param alc The memory allocator used by JSON writer.
    Pass NULL to use the libc's default allocator.
 @param err A pointer to receive error information.
    Pass NULL if you don't need error information.
 @return true if successful, false if an error occurs.

 @warning On 32-bit operating system, files larger than 2GB may fail to write.
 */
SEQ_FUNC bool seq_yyjson_mut_write_fp(FILE *fp, const yyjson_mut_doc *doc,
                                      yyjson_write_flag flg,
                                      const yyjson_alc *alc,
                                      yyjson_write_err *err);

/**
 Write a document to JSON string.

 This function is thread-safe when:
 The `doc` is not modified by other threads.

 @param doc The JSON document.
    If this doc is NULL or has no root, the function will fail and return false.
 @param flg The JSON write options.
    Multiple options can be combined with `|` operator. 0 means no options.
 @param len A pointer to receive output length in bytes (not including the
    null-terminator). Pass NULL if you don't need length information.
 @return A new JSON string, or NULL if an error occurs.
    This string is encoded as UTF-8 with a null-terminator.
    When it's no longer needed, it should be freed with free().
 */
SEQ_FUNC char *seq_yyjson_mut_write(const yyjson_mut_doc *doc,
                                    yyjson_write_flag flg, size_t *len);

/*==============================================================================
 * Mutable JSON Object Creation API
 *============================================================================*/

/** Creates and returns a mutable object, returns NULL on error. */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_obj(yyjson_mut_doc *doc);

/**
 Creates and returns a mutable object with keys and values, returns NULL on
 error. The keys and values are not copied. The strings should be a
 null-terminated UTF-8 string.

 @warning The input string is not copied, you should keep this string
    unmodified for the lifetime of this JSON document.

 @par Example
 @code
    const char *keys[2] = { "id", "name" };
    const char *vals[2] = { "01", "Harry" };
    yyjson_mut_val *obj = yyjson_mut_obj_with_str(doc, keys, vals, 2);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_obj_with_str(yyjson_mut_doc *doc,
                                                     const char **keys,
                                                     const char **vals,
                                                     size_t count);

/**
 Creates and returns a mutable object with key-value pairs and pair count,
 returns NULL on error. The keys and values are not copied. The strings should
 be a null-terminated UTF-8 string.

 @warning The input string is not copied, you should keep this string
    unmodified for the lifetime of this JSON document.

 @par Example
 @code
    const char *kv_pairs[4] = { "id", "01", "name", "Harry" };
    yyjson_mut_val *obj = yyjson_mut_obj_with_kv(doc, kv_pairs, 2);
 @endcode
 */
SEQ_FUNC yyjson_mut_val *seq_yyjson_mut_obj_with_kv(yyjson_mut_doc *doc,
                                                    const char **kv_pairs,
                                                    size_t pair_count);

#endif // MOXT_LIBYYJSON_HPP
