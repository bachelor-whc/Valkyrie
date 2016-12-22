#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(set=0, binding=1) uniform sampler2D samplerColor;
layout(set=1, binding=0) uniform sampler2D whiteTexture;

layout(location=0) in vec2 inUV;
layout(location=0) out vec4 outFragColor;

void main() 
{
	vec4 texw = texture(whiteTexture, inUV, 0.0f);
	vec4 texs = texture(samplerColor, inUV, 0.0f);
	outFragColor = texw - texs;
}