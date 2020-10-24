#pragma once

#include "GL/Buffer.h"
#include "GL/Shader.h"

#include "VulkanContext.h"

namespace Vertex
{
    const VkFormat ShaderDataTypeToVulkan(ShaderDataType type);

    static VkVertexInputBindingDescription GetBindingDescription(const BufferLayout& layout)
    {
        VkVertexInputBindingDescription binding_description {};

        binding_description.binding   = 0;
        binding_description.stride    = layout.GetStride();
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return binding_description;
    }

    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions(const BufferLayout& layout)
    {
        std::vector<VkVertexInputAttributeDescription> attribute_descriptions;

        auto& elements = const_cast<std::vector<BufferElement>&>(layout.GetElements());

        for (uint32_t i = 0; i < layout.GetElements().size(); i++)
        {
            VkVertexInputAttributeDescription description {};
            description.binding  = 0;
            description.location = i;
            description.format   = ShaderDataTypeToVulkan(elements[i].type);
            description.offset   = elements[i].offset;
            attribute_descriptions.emplace_back(description);
        }

        return attribute_descriptions;
    }

    class VulkanShaderModule
    {
    public:
        VulkanShaderModule(std::vector<char>& source, VkPipelineShaderStageCreateInfo& shader_stage_info);
        ~VulkanShaderModule();

    private:
        VkShaderModule m_InternalVkShaderModule;
    };

    class VulkanShader : public Shader
    {
    public:
        VulkanShader(std::vector<char>& vertex_src, std::vector<char>& fragment_src, const BufferLayout vertex_layout);
        ~VulkanShader();

        void Bind() const override;
        void Unbind() const override;

    private:
        VkPipeline m_GraphicsPipeline;
    };
}