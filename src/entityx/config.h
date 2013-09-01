#pragma once

#include <stdint.h>

// Use std::shared_ptr

#include <memory>

namespace entityx {
using std::make_shared;
using std::shared_ptr;
using std::static_pointer_cast;
using std::weak_ptr;
using std::enable_shared_from_this;
}

namespace entityx {
static const uint64_t MAX_COMPONENTS = 64;
}
