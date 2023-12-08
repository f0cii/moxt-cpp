#ifndef MOXT_LIBSAMPLE_HPP
#define MOXT_LIBSAMPLE_HPP

#include "common.hpp"

typedef int (*SeqAddCallbackFunc)(int, int) noexcept; // noexcept

SEQ_FUNC int seq_add(int a, int b);

SEQ_FUNC int seq_add_callback(int a, int b, SeqAddCallbackFunc callback);

#endif