#pragma once

class VertexFormats {
public:
    enum VertexAttribute : uint8_t {
        ATTR_POSITION,
        ATTR_COLOR,
        ATTR_TEXCOORD,
        ATTR_COUNT
    };

    static inline std::array<uint8_t, ATTR_COUNT> ATTRIBUTE_SIZES = {3, 4, 2};

    static inline std::array<VertexAttribute, 1> POSITION_ATTRIB = {ATTR_POSITION};
    static inline std::array<VertexAttribute, 2> POSITION_COLOR_ATTRIB = {ATTR_POSITION, ATTR_COLOR};
    static inline std::array<VertexAttribute, 2> POSITION_TEX_ATTRIB = {ATTR_POSITION, ATTR_TEXCOORD};
    static inline std::array<VertexAttribute, 3> POSITION_TEX_COLOR_ATTRIB = {ATTR_POSITION, ATTR_TEXCOORD, ATTR_COLOR};

    static inline std::array<std::string, 1> POSITION = {"Position"};
    static inline std::array<std::string, 2> POSITION_COLOR = {"Position", "Color"};
    static inline std::array<std::string, 2> POSITION_TEX = {"Position", "UV"};
    static inline std::array<std::string, 3> POSITION_TEX_COLOR = {"Position", "UV", "Color"};
};
