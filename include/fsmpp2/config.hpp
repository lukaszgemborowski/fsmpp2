#ifndef FSMPP2_CONFIG_HPP
#define FSMPP2_CONFIG_HPP

#define FSMPP2_VERSION_MAJOR 0
#define FSMPP2_VERSION_MINOR 1
#define FSMPP2_VERSION_PATCH 0

namespace fsmpp2
{

struct version_info {
    unsigned major, minor, patch;
};

constexpr auto version = version_info {
    FSMPP2_VERSION_MAJOR,
    FSMPP2_VERSION_MINOR,
    FSMPP2_VERSION_PATCH
};

} // namespace fsmpp2

#endif // FSMPP2_CONFIG_HPP