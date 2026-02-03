#ifndef ECT_SDK_CONFIG_HPP
#define ECT_SDK_CONFIG_HPP

#include <cstdint>

namespace ect::sdk
{
    // -------------------------------------------------------------------------
    // SDK identity
    // -------------------------------------------------------------------------

    inline constexpr const char* SDK_NAME = "ECT-SDK";

    // -------------------------------------------------------------------------
    // Versioning (semantic)
    // -------------------------------------------------------------------------

    inline constexpr std::uint32_t VERSION_MAJOR = 1;
    inline constexpr std::uint32_t VERSION_MINOR = 0;
    inline constexpr std::uint32_t VERSION_PATCH = 0;

    // Optional build identifier (set by build system if needed)
    inline constexpr std::uint32_t BUILD_ID = 0;

    // -------------------------------------------------------------------------
    // Version helpers
    // -------------------------------------------------------------------------

    constexpr std::uint32_t version_compact()
    {
        return (VERSION_MAJOR << 16)
             | (VERSION_MINOR << 8)
             | (VERSION_PATCH);
    }

} // namespace ect::sdk

#endif // ECT_SDK_CONFIG_HPP
