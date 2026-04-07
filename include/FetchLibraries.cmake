include(FetchContent)

FetchContent_Declare(
        embed
        GIT_REPOSITORY https://github.com/batterycenter/embed.git
        GIT_TAG 465081903d97ff1ed05e1fd5d0b3c8032a4a26a6
)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG e7970a8b26732f1b0df9690f7180546f8c30e48e
)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG f1d748e5e3edfa4b1778edea003bac94781bc7b7
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
        embed
        glm
        spdlog
        magic_enum
        giflib
)