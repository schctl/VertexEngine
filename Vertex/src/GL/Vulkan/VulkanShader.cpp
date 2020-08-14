#include "VulkanShader.h"

namespace Vertex
{
    const VkFormat ShaderDataTypeToVulkan(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Bool:
            return VK_FORMAT_R8_SINT;
        case ShaderDataType::Mat3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case ShaderDataType::Mat4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case ShaderDataType::Int:
            return VK_FORMAT_R32_SINT;
        case ShaderDataType::Int2:
            return VK_FORMAT_R32G32_SINT;
        case ShaderDataType::Int3:
            return VK_FORMAT_R32G32B32_SINT;
        case ShaderDataType::Int4:
            return VK_FORMAT_R32G32B32A32_SINT;
        case ShaderDataType::Float:
            return VK_FORMAT_R32_SFLOAT;
        case ShaderDataType::Float2:
            return VK_FORMAT_R32G32_SINT;
        case ShaderDataType::Float3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case ShaderDataType::Float4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        }

        CoreLogger::Error("Unknown shader data type, cancelling...");
        return 0;
    }

    VulkanShaderModule::VulkanShaderModule(std::vector<char>&               source,
                                           VkPipelineShaderStageCreateInfo& shader_stage_info)
    {
        VkShaderModuleCreateInfo create_info {};
        create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = source.size();
        create_info.pCode    = reinterpret_cast<const uint32_t*>(source.data());

        if (vkCreateShaderModule(VulkanContext::Get()->GetLogicalDevice(), &create_info, nullptr,
                                 &m_InternalVkShaderModule)
            != VK_SUCCESS)
        {
            VX_CORE_ASSERT(false, "vkCreateShaderModule failed");
        }

        shader_stage_info.module = m_InternalVkShaderModule;
    }

    VulkanShaderModule::~VulkanShaderModule()
    {
        vkDestroyShaderModule(VulkanContext::Get()->GetLogicalDevice(), m_InternalVkShaderModule, nullptr);
    }

    VulkanShader::VulkanShader(std::vector<char>& vertex_src, std::vector<char>& fragment_src)
    {
        VkViewport viewport {};
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = (float)VulkanContext::Get()->GetSwapChainExtent().width;
        viewport.height   = (float)VulkanContext::Get()->GetSwapChainExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor {};
        scissor.offset = { 0, 0 };
        scissor.extent = VulkanContext::Get()->GetSwapChainExtent();

        VkPipelineViewportStateCreateInfo viewport_state {};
        viewport_state.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.pViewports    = &viewport;
        viewport_state.scissorCount  = 1;
        viewport_state.pScissors     = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer {};
        rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable        = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth               = 1.0f;
        rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable         = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp          = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor    = 0.0f; // Optional

        VkPipelineMultisampleStateCreateInfo multisampling {};
        multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable   = VK_FALSE;
        multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading      = 1.0f;     // Optional
        multisampling.pSampleMask           = nullptr;  // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable      = VK_FALSE; // Optional

        VkPipelineColorBlendAttachmentState color_blend_attachment {};
        color_blend_attachment.colorWriteMask
            = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable         = VK_FALSE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        color_blend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;      // Optional
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        color_blend_attachment.alphaBlendOp        = VK_BLEND_OP_ADD;      // Optional

        VkPipelineColorBlendStateCreateInfo color_blending {};
        color_blending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable     = VK_FALSE;
        color_blending.logicOp           = VK_LOGIC_OP_COPY; // Optional
        color_blending.attachmentCount   = 1;
        color_blending.pAttachments      = &color_blend_attachment;
        color_blending.blendConstants[0] = 0.0f; // Optional
        color_blending.blendConstants[1] = 0.0f; // Optional
        color_blending.blendConstants[2] = 0.0f; // Optional
        color_blending.blendConstants[3] = 0.0f; // Optional

        VkDynamicState dynamic_states[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };

        VkPipelineDynamicStateCreateInfo dynamic_state {};
        dynamic_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = 2;
        dynamic_state.pDynamicStates    = dynamic_states;

        VkPipelineShaderStageCreateInfo vert_shader_stage_info {};
        vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vert_shader_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo frag_shader_stage_info {};
        frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_shader_stage_info.pName = "main";

        VulkanShaderModule vertex_module(vertex_src, vert_shader_stage_info);
        VulkanShaderModule fragment_module(fragment_src, frag_shader_stage_info);

        VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

        VkPipelineVertexInputStateCreateInfo vertex_input_info {};
        vertex_input_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount   = InputBindingsLen;
        vertex_input_info.pVertexBindingDescriptions      = bindings.data();
        vertex_input_info.vertexAttributeDescriptionCount = InputAttribLen;
        vertex_input_info.pVertexAttributeDescriptions    = input_attributes.data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly {};
        input_assembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkGraphicsPipelineCreateInfo pipeline_info {};
        pipeline_info.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount = 2;
        pipeline_info.pStages    = shader_stages;

        pipeline_info.pVertexInputState   = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState      = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState   = &multisampling;
        pipeline_info.pDepthStencilState  = nullptr; // Optional
        pipeline_info.pColorBlendState    = &color_blending;
        pipeline_info.pDynamicState       = nullptr; // Optional

        pipeline_info.layout     = VulkanContext::Get()->GetPipelineLayout();
        pipeline_info.renderPass = VulkanContext::Get()->GetRenderPass();
        pipeline_info.subpass    = 0;

        pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipeline_info.basePipelineIndex  = -1;             // Optional

        if (vkCreateGraphicsPipelines(VulkanContext::Get()->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipeline_info,
                                      nullptr, &m_GraphicsPipeline)
            != VK_SUCCESS)
        {
            VX_CORE_ASSERT(false, "failed to create a graphics pipeline");
        }
    }
}
