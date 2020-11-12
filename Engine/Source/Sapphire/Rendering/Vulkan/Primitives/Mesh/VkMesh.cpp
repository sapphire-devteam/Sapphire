// Copyright 2020 Sapphire development team. All Rights Reserved.

#include <Rendering/Vulkan/Primitives/Mesh/VkMesh.hpp>

#include <Core/Algorithms/SizeOf.hpp>

#include <Rendering/Vulkan/System/VkRenderInstance.hpp>

#if SA_RENDERING_API == SA_VULKAN

namespace Sa::Vk
{
	void Mesh::Create(const IRenderInstance& _instance, const RawMesh& _rawMesh)
	{
		const Device& device = _instance.As<RenderInstance>().device;


		// Create Vertex buffer.
		mVertexBuffer.Create(device, SizeOf(_rawMesh.vertices),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			_rawMesh.vertices.data());


		// Create Index buffer.
		mIndicesSize = SizeOf(_rawMesh.indices);
		mIndexBuffer.Create(device, sizeof(uint32) * mIndicesSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			_rawMesh.indices.data());
	}

	void Mesh::Destroy(const IRenderInstance& _instance)
	{
		const Device& device = _instance.As<RenderInstance>().device;

		mVertexBuffer.Destroy(device);
		mIndexBuffer.Destroy(device);

		mIndicesSize = uint32(-1);
	}

	//void Mesh::Draw(const IRenderFrame& _frame) const
	//{
	//	const VkRenderFrame& vkFrame = _frame.As<VkRenderFrame>();

	//	VkDeviceSize offsets[] = { 0 };
	//	::VkBuffer vkHandleVertexBuffer = mVertexBuffer;
	//	vkCmdBindVertexBuffers(vkFrame.framebuffer->GetCommandBuffer(), 0, 1, &vkHandleVertexBuffer, offsets);

	//	vkCmdBindIndexBuffer(vkFrame.framebuffer->GetCommandBuffer(), mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

	//	vkCmdDrawIndexed(vkFrame.framebuffer->GetCommandBuffer(), mIndicesSize, 1, 0, 0, 0);
	//}
}

#endif
