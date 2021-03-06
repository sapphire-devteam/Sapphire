// Copyright 2020 Sapphire development team. All Rights Reserved.

#version 450
#extension GL_ARB_separate_shader_objects : enable


// Uniform.
// CameraUBO: shared across shader instances.
layout(binding = 0) uniform Camera
{
	// Camera projection matrix.
    mat4 proj;
    
	// Camera inverse transformation matrix.
    mat4 viewInv;

    // Camera position.
    vec3 viewPosition;
} cameraUBO;


// Object UBO: specific to this shader instance.
layout(binding = 1) uniform ObjectUniformBuffer
{
	// Model transformation matrix.
    mat4 modelMat;

	// Material UV tilling.
    float uvTilling;

	// Material UV offset.
    float uvOffset;

} objectUBO;


// In.
layout(location = 0) in vec3 inPosition;
layout(location = 3) in vec2 inTexture;


// Out.
layout(location = 0) out DataBlock
{
	vec2 texture;
} vsOut;


// Code
void main()
{
    // Position.
    vec4 modelPosition = objectUBO.modelMat * vec4(inPosition, 1.0);

    gl_Position = cameraUBO.proj * cameraUBO.viewInv * modelPosition;


    // Texture
    vsOut.texture = inTexture * objectUBO.uvTilling + objectUBO.uvOffset;
}