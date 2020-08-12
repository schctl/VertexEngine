#pragma once

#include "GL/Buffer.h"
#include "GL/Shader.h"

#include "VulkanContext.h"

namespace Vertex
{
    const VkFormat ShaderDataTypeToVulkan(ShaderDataType type);

    static VkVertexInputBindingDescription GetBindingDescription(BufferLayout& layout)
    {
        VkVertexInputBindingDescription binding_description {};

        binding_description.binding   = 0;
        binding_description.stride    = layout.GetStride();
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return binding_description;
    };

    template <size_t Bindings>
    static std::array<VkVertexInputAttributeDescription, Bindings> GetAttributeDescriptions(BufferLayout& layout)
    {
        std::array<VkVertexInputAttributeDescription, Bindings> attribute_descriptions {};

        auto& elements = const_cast<std::vector<BufferElement>&>(layout.GetElements());

        for (uint32_t i = 0; i < Bindings; i++)
        {
            attribute_descriptions[i].binding  = 0;
            attribute_descriptions[i].location = 0;
            attribute_descriptions[i].format   = ShaderDataTypeToVulkan(elements[i].type);
            attribute_descriptions[i].offset   = elements[i].offset;
        }

        return attribute_descriptions;
    };

    class VulkanShaderModule
    {
    public:
        VulkanShaderModule(const std::vector<unsigned char>& source,
                           VkPipelineShaderStageCreateInfo&  shader_stage_info);

        ~VulkanShaderModule();

    private:
        VkShaderModule m_InternalVkShaderModule;
    };

    class VulkanShader : public Shader
    {
    public:
        VulkanShader(const char* vertex_src, const char* fragment_src);
        ~VulkanShader();

        void Bind() const override;
        void Unbind() const override;

    private:
        VkPipeline m_GraphicsPipeline;
    };
}