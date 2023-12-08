#ifndef MOXT_LIBEASYLOG_HPP
#define MOXT_LIBEASYLOG_HPP

#include "common.hpp"

SEQ_FUNC void seq_init_log(uint8_t level, int64_t mode);

SEQ_FUNC void seq_logvd(const char *s, size_t sLen);

SEQ_FUNC void seq_logvi(const char *s, size_t sLen);

SEQ_FUNC void seq_logvw(const char *s, size_t sLen);

SEQ_FUNC void seq_logve(const char *s, size_t sLen);

SEQ_FUNC void seq_set_log_output_level();

SEQ_FUNC void seq_log_test();

#endif