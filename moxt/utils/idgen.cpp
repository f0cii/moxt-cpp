//
// Created by ken on 2023/7/7.
//

#include "moxt/utils/idgen.hpp"
#include <nanoid/nanoid.h>

std::string IDGen::id() { return nanoid::generate(); }
