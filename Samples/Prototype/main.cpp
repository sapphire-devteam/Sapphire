// Copyright 2020 Sapphire development team. All Rights Reserved.

#include <string>
#include <iostream>

#include <Sapphire/Core/Time/Chrono.hpp>
#include <Sapphire/Maths/Space/Quaternion.hpp>

#include <Sapphire/Window/GLFWWindow.hpp>

#include <Sapphire/Rendering/Vulkan/VkRenderInstance.hpp>
#include <Sapphire/Rendering/Vulkan/Queue/VkCommandBuffer.hpp>
#include <Sapphire/Rendering/Framework/Model/UniformBufferObject.hpp>

// Material
#include <Sapphire/Rendering/Framework/Primitives/Pipeline/PipelineCreateInfos.hpp>
#include <Sapphire/Rendering/Vulkan/Primitives/Pipeline/VkShader.hpp>
#include <Sapphire/Rendering/Vulkan/Model/VkTexture.hpp>
#include <Sapphire/Rendering/Vulkan/Primitives/VkRenderMaterial.hpp>

// Mesh
#include <Sapphire/Rendering/Vulkan/Model/VkMesh.hpp>

using namespace Sa;

#define LOG(_str) std::cout << _str << std::endl;

int main()
{
	LOG("=== Start ===");


	// === Create ===
	VkRenderInstance instance;
	instance.Create();

	GLFWWindow window;
	window.Create(800u, 800u);

	VkRenderSurface& surface = const_cast<VkRenderSurface&>(static_cast<const VkRenderSurface&>(instance.CreateRenderSurface(window)));


	// Create Material.
	VkShader vertShader;
	vertShader.Create(instance, L"../../Bin/Shaders/default_vert.spv");

	VkShader fragShader;
	fragShader.Create(instance, L"../../Bin/Shaders/default_frag.spv");

	VkTexture catTexture;
	catTexture.Create(instance, "../../Engine/Resources/Textures/SampleCat.jpg");

	PipelineCreateInfos mainPipelineInfos
	{
		surface,
		surface.GetViewport(),

		&vertShader,
		&fragShader,

		{ &catTexture },

		PolygonMode::Fill,
		CullingMode::None,
		FrontFaceMode::Clockwise
	};

	VkRenderMaterial material;
	material.CreatePipeline(instance, mainPipelineInfos);


	// Create Mesh.
	VkMesh mesh;
	const std::vector<Vertex> vertices =
	{
		{ { -0.5f, -0.5f, 0.0f }, Vec3f::Forward, { 0.0f, 0.0f } },
		{ { 0.5f, -0.5f, 0.0f }, Vec3f::Forward, { 1.0f, 0.0f } },
		{ { 0.5f, 0.5f, 0.0f }, Vec3f::Forward, { 1.0f, 1.0f } },
		{ { -0.5f, 0.5f, 0.0f }, Vec3f::Forward, { 0.0f, 1.0f } },

		{ { 0.5f + -0.5f, 0.5f + -0.5f, -0.5f }, Vec3f::Forward, { 0.0f, 0.0f } },
		{ { 0.5f + 0.5f, 0.5f + -0.5f, -0.5f }, Vec3f::Forward, { 1.0f, 0.0f } },
		{ { 0.5f + 0.5f, 0.5f + 0.5f, -0.5f }, Vec3f::Forward, { 1.0f, 1.0f } },
		{ { 0.5f + -0.5f, 0.5f + 0.5f, -0.5f }, Vec3f::Forward, { 0.0f, 1.0f } }
	};
	
	const std::vector<uint32> indices =
	{
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};

	mesh.Create(instance, vertices, indices);

	const float r = 1.0f;
	const float l = -1.0f;
	const float t = 1.0f;
	const float b = -1.0f;
	const float n = 1.0f;
	const float f = 100.0f;

	Mat4f orthoMat
	(
		2 / (r - l), 0, 0, -(r + l) / (r - l),
		0, 2 / (t - b), 0, -(t + b) / (t - b),
		0, 0, -2 / (f - n), -(f + n) / (f - n),
		0, 0, 0, 1
	);

	Chrono chrono;
	float time = 0.0f;

	// Main Loop
	while (!window.ShouldClose())
	{
		float deltaTime = chrono.Restart() * 0.00005f;
		time += deltaTime;

		window.Update();
		instance.Update();

		// Update Uniform Buffer.
		UniformBufferObject ubo;
		ubo.modelMat = Mat4f::MakeRotation(Quatf(time, Vec3f::Forward));
		ubo.projMat = orthoMat;


		VkRenderFrame frame = surface.GetSwapChain().Update(instance.GetDevice());

		void* data;
		vkMapMemory(instance.GetDevice(), frame.uniformBuffer, 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(instance.GetDevice(), frame.uniformBuffer);


		const VkCommandBufferBeginInfo commandBufferBeginInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
		};

		SA_VK_ASSERT(vkBeginCommandBuffer(frame.graphicsCommandBuffer, &commandBufferBeginInfo),
			LibCommandFailed, Rendering, L"Failed to begin command buffer!");


		VkClearValue clearValue[2];
		clearValue[0].color = VkClearColorValue{ 0.0f, 0.0f, 0.07f, 1.0f };
		clearValue[1].depthStencil = VkClearDepthStencilValue{ 1.0f, 0 };


		const VkRenderPassBeginInfo renderPassBeginInfo
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,							// sType.
			nullptr,															// pNext.
			surface.GetRenderPass(),											// renderPass.
			frame.frameBuffer,													// framebuffer
			VkRect2D{ VkOffset2D{}, surface.GetImageExtent() },					// renderArea.
			sizeof(clearValue) / sizeof(VkClearValue),							// clearValueCount.
			clearValue															// pClearValues.
		};

		vkCmdBeginRenderPass(frame.graphicsCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		material.Bind(frame);

		mesh.Draw(frame);

		vkCmdEndRenderPass(frame.graphicsCommandBuffer);

		SA_VK_ASSERT(vkEndCommandBuffer(frame.graphicsCommandBuffer),
			LibCommandFailed, Rendering, L"Failed to end command buffer!");
	}



	// === Destroy ===
	vkDeviceWaitIdle(instance.GetDevice());

	// Destroy mesh.
	mesh.Destroy(instance);

	// Destroy Material.
	material.DestroyPipeline(instance);

	catTexture.Destroy(instance);
	fragShader.Destroy(instance);
	vertShader.Destroy(instance);


	instance.DestroyRenderSurface(window);
	
	window.Destroy();

	instance.Destroy();


	LOG("\n=== End ===");

	return 0;
}
