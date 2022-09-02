#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "Config.hpp"


using VkAllocationCallbacksPair = std::pair<const VkAllocationCallbacks*, const VkAllocationCallbacks*>;

class Pipeline
{
public:
    Pipeline() = default;
    Pipeline(const Pipeline&) =  delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline(Pipeline&&) = delete;
    Pipeline& operator=(Pipeline&&) = delete;
    
    void setupGraphicsPipeline(const VkDevice device, const VkRenderPass renderPass, const VkAllocationCallbacksPair pAllocators = {nullptr, nullptr});
    void destroyGraphicsPipeline(const VkDevice device, const VkAllocationCallbacksPair pAllocators = {nullptr, nullptr});
    
private:
    VkPipelineLayout graphicsPipelineLayout = VK_NULL_HANDLE;
    VkPipeline graphicsPipeline = VK_NULL_HANDLE;
    
    VkShaderModule createShaderModule(const VkDevice device, const std::vector<char>& code);
    void populateVertexCreateInfo(VkPipelineVertexInputStateCreateInfo& vertexInputInfo);
    void populateAssemblyCreateInfo(VkPipelineInputAssemblyStateCreateInfo& inputAssembly);
    void populateViewportCreateInfo(VkPipelineViewportStateCreateInfo& viewportState);
    void  populateDynamicCreateInfo(std::vector<VkDynamicState>& dynamicStates, VkPipelineDynamicStateCreateInfo& dynamicState);
    void populateRasterizationCreateInfo(VkPipelineRasterizationStateCreateInfo& rasterizer);
    void populateMultisampleCreateInfo(VkPipelineMultisampleStateCreateInfo& multisampling);
    void populateColorBlendCreateInfo(VkPipelineColorBlendAttachmentState& colorBlendAttachment, VkPipelineColorBlendStateCreateInfo& colorBlending);
    void populatePipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo& pipelineLayoutInfo);
};

#endif
