#version 460
#extension GL_EXT_debug_printf : enable
#extension GL_ARB_separate_shader_objects : enable
// #extension GL_EXT_debug_printf : enable
#extension GL_EXT_scalar_block_layout : enable

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(set = 0, binding = 0, std430) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(constant_id = 0) const int SIZE_X = 1; //specialization constants
layout(constant_id = 1) const int SIZE_Y = 1;
layout(constant_id = 2) const int SIZE_Z = 1;
#define N SIZE_X*SIZE_Y*SIZE_Z

struct LBMStruct
{
    float rho;
};

layout(std430, set = 0, binding = 1) buffer buf2
{
    LBMStruct elem[];
};


void main() {
        gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition.xyz, 1.0f);

        fragTexCoord = inTexCoord;

//        fragColor = inColor.xyz;
        fragColor = vec3(elem[gl_VertexIndex].rho,0,0);
//        debugPrintfEXT("id=%d, rho=%f\n",gl_VertexIndex,elem[gl_VertexIndex].rho);
//        debugPrintfEXT("size=%d\n",SIZE_X);
}
