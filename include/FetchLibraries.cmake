include(FetchContent)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 6f14f4792a0cde5d0cf2c910506724d61cb95834
)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG 6d9805df3f588402b17073dcd13eb569ceda864d
)

FetchContent_Declare(
        magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG d3f466bf0c5651ac41780be07f30eda4fbfe697f
)

FetchContent_Declare(
        giflib
        GIT_REPOSITORY https://github.com/xbmc/giflib.git
        GIT_TAG 7e74d92d318ed865e6775b3b05b0cf5c6a39bc20
)

FetchContent_MakeAvailable(
        glm
        spdlog
        magic_enum
        giflib
)