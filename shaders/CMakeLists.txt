SET(SHADERS
    "fragment.glsl"
    "vertex.glsl"
)

add_custom_target(
    res
    SOURCES
    ${SHADERS}
)

source_group("shaders" FILES ${SHADERS})
