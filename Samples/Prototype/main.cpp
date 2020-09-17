// Copyright 2020 Sapphire development team. All Rights Reserved.

#include <string>
#include <iostream>

#include <Sapphire/Window/GLFWWindow.hpp>
#include <Sapphire/Rendering/Model/Mesh.hpp>
#include <Sapphire/Rendering/Model/Vertex.hpp>
#include <Sapphire/Rendering/Vulkan/VkRenderInstance.hpp>
#include <Sapphire/Rendering/Vulkan/Primitives/Pipeline/VkShader.hpp>
#include <Sapphire/Rendering/Vulkan/Primitives/Pipeline/VkRenderPipeline.hpp>
using namespace Sa;

#define LOG(_str) std::cout << _str << std::endl;

int main()
{
	LOG("=== Start ===");


	// Create.
	VkRenderInstance instance;
	instance.Create();

	GLFWWindow window;
	window.Create(800u, 600u);

	VkRenderSurface& surface = const_cast<VkRenderSurface&>(static_cast<const VkRenderSurface&>(instance.CreateRenderSurface(window)));

	const std::vector<Vertex> vertices =
	{
		{ { 0.0f, -0.5f, 0.0f }, Vec3f::Forward, { 1.0f, 1.0f, 1.0f } },
		{ { 0.5f, 0.5f, 0.0f }, Vec3f::Forward, { 0.0f, 1.0f, 0.0f } },
		{ { -0.5f, 0.5f, 0.0f }, Vec3f::Forward, { 0.0f, 0.0f, 1.0f } }
	};

	Mesh mesh;
	mesh.Create(instance.GetDevice(), vertices);

	VkShader vertShader;
	vertShader.Create(instance, ShaderType::Vertex, L"../../Bin/Shaders/default_vert.spv");

	VkShader fragShader;
	fragShader.Create(instance, ShaderType::Fragment, L"../../Bin/Shaders/default_frag.spv");

	VkRenderPipeline pipeline;
	pipeline.Create(instance, surface, { &vertShader, &fragShader }, surface.GetViewport());


	// Main Loop
	while (!window.ShouldClose())
	{
		instance.Update();

		VkRenderFrame frame = surface.GetSwapChain().Update(instance.GetDevice());

		const VkCommandBufferBeginInfo commandBufferBeginInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
		};

		SA_VK_ASSERT(vkBeginCommandBuffer(frame.graphicsCommandBuffer, &commandBufferBeginInfo),
			LibCommandFailed, Rendering, L"Failed to begin command buffer!");



		const VkClearValue clearValue{ VkClearColorValue{ 0.0f, 0.0f, 0.07f, 1.0f } };

		const VkRenderPassBeginInfo renderPassBeginInfo
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,							// sType.
			nullptr,															// pNext.
			surface.GetRenderPass(),											// renderPass.
			frame.frameBuffer,													// framebuffer
			VkRect2D{ VkOffset2D{}, surface.GetImageExtent() },					// renderArea.
			1,																	// clearValueCount.
			&clearValue															// pClearValues.
		};

		vkCmdBeginRenderPass(frame.graphicsCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


		vkCmdBindPipeline(frame.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		VkBuffer vertexBuffers[] = { mesh };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(frame.graphicsCommandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdDraw(frame.graphicsCommandBuffer, static_cast<uint32>(vertices.size()), 1, 0, 0);


		vkCmdEndRenderPass(frame.graphicsCommandBuffer);

		SA_VK_ASSERT(vkEndCommandBuffer(frame.graphicsCommandBuffer),
			LibCommandFailed, Rendering, L"Failed to end command buffer!");
	}



	// Destroy.
	pipeline.Destroy(instance);

	vertShader.Destroy(instance);
	fragShader.Destroy(instance);

	instance.DestroyRenderSurface(window);
	
	window.Destroy();

	instance.Destroy();


	LOG("\n=== End ===");

	return 0;
}
