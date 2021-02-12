cmake_minimum_required(VERSION 3.18.0)

project(vLBM VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(PkgConfig REQUIRED)
find_package(Vulkan REQUIRED)
# find_library(SHADERC_LIB shaderc_combined $ENV{VULKAN_SDK}/Lib)
# find_package(GLEW REQUIRED)
# find_package(GLUT REQUIRED)
# find_package(OpenGL REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

#MESSAGE(STATUS "Found shaderc in: ${SHADERC_LIB}")

# include_directories(${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

#include_directories( )

set(SOURCES
    src/main.cpp
    src/VulkanDynamic.cpp
    src/vApp.cpp
    src/vImGUI.cpp
    src/MyShape.cpp
    src/MyBuffer.cpp
    src/MyTextureImage.cpp
    src/MyCompute.cpp)

set(HEADERS
    include/context.hpp
    include/VulkanDynamic.hpp
    include/vApp.hpp
    include/vImGUI.hpp
    include/MyShape.hpp
    include/MyBuffer.hpp
    include/MyTextureImage.hpp
    include/MyCompute.hpp)

#file(GLOB RES_FILES
#    shaders/vertShader.vert
#    shaders/fragShader.frag
#    shaders/compShader1.comp
#    shaders/compShader2.comp)

#add_executable(vNBody
#    readme.md
#    buildDebug/vk_layer_settings.txt
#    ${SOURCES}
#    ${HEADERS}
#    ${RES_FILES}
#    imgui/imgui.cpp imgui/imgui_widgets.cpp imgui/imgui_tables.cpp imgui/imgui_draw.cpp imgui/imgui_impl_glfw.cpp imgui/imgui_impl_vulkan.cpp imgui/imgui_demo.cpp
#    )


#execute_process (
#    COMMAND bash -c "date +'%F %T'"
#    OUTPUT_VARIABLE outVar
#)

#MESSAGE(STATUS "${outVar}")



#shaders/compShader1.comp
#shaders/compShader2.comp
set(KERNELS
${CMAKE_SOURCE_DIR}/shaders/vertShader.vert
${CMAKE_SOURCE_DIR}/shaders/fragShader.frag
${CMAKE_SOURCE_DIR}/shaders/compShader1.comp
${CMAKE_SOURCE_DIR}/shaders/compShader2.comp
)

#set(COMPILED_KERNELS
#${CMAKE_SOURCE_DIR}/vertShader.vert.spv
#${CMAKE_SOURCE_DIR}/fragShader.frag.spv
#)

#foreach(KERNEL ${KERNELS})
#    message("labas!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
#add_custom_command(OUTPUT ${KERNEL}.spv
#COMMAND ${KERNEL} -g -o ${KERNEL}.spv
#DEPENDS ${KERNEL}
#COMMENT "Rebuilding ${KERNEL}.spv" )
#message(STATUS "Generating build commands for ${KERNEL}.spv")
#endforeach()

#########################3

find_program(
        GLSLC#GLSL_VALIDATOR
        glslc#glslangValidator
        DOC "shaderc Vulkan Shader Compiler (glslc)"
        REQUIRED
)

#file(GLOB files "*.glsl")
foreach(file ${KERNELS})
#foreach(file ${files})
        get_filename_component(OUTPUT_FILENAME ${file} NAME)
        add_custom_command(
                OUTPUT ${OUTPUT_FILENAME}.spv
                COMMAND ${GLSLC} ${file} -g -o ${OUTPUT_FILENAME}.spv
                COMMENT "Compiling shaders ..."
                DEPENDS ${file}
        )
        list(APPEND OUTPUT_SHADERS ${OUTPUT_FILENAME}.spv)
endforeach()

add_custom_target(compile_shaders ALL
        DEPENDS ${OUTPUT_SHADERS})

##############

add_executable(vLBM
    readme.md
    buildDebug/vk_layer_settings.txt
    ${SOURCES}
    ${HEADERS}
    ${KERNELS}
#    ${COMPILED_KERNELS}
    imgui/imgui.cpp imgui/imgui_widgets.cpp imgui/imgui_tables.cpp imgui/imgui_draw.cpp imgui/imgui_impl_glfw.cpp imgui/imgui_impl_vulkan.cpp imgui/imgui_demo.cpp
    )

#add_executable(vNBody
#    readme.md
#    buildDebug/vk_layer_settings.txt
#    ${SOURCES}
#    ${HEADERS}
#    ${RES_FILES}
#    imgui/imgui.cpp imgui/imgui_widgets.cpp imgui/imgui_tables.cpp imgui/imgui_draw.cpp imgui/imgui_impl_glfw.cpp imgui/imgui_impl_vulkan.cpp imgui/imgui_demo.cpp
#    )

target_include_directories(vLBM PUBLIC include
    ${HEADERS}
    ${GLFW_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/imgui/
    )


target_link_libraries(vLBM
            dl pthread
            ${GLFW_LIBRARIES}
            ${Vulkan_LIBRARIES}
            ) #dl - for vulkan hpp:  dl.getProcAddress<PFN_vkGetInstanceProcAddr>
