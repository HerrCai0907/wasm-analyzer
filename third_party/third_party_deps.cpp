#include "third_party_deps.hpp"
#include <array>
#include <span>

namespace wa {

static std::array third_party_impl{
    ThirdPartyDeps{.m_name = "argparse",
                   .m_link = "https://github.com/p-ranav/argparse/tree/8a7fa1899822947f7553921f7bcec12d8d0f89a7"},
};

std::span<ThirdPartyDeps> third_party{third_party_impl.data(), third_party_impl.size()};

} // namespace wa
