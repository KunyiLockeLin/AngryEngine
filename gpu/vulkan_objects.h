#ifndef AE_GPU_VULKAN_OBJECTS_H
#define AE_GPU_VULKAN_OBJECTS_H

#if defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <memory>
#include <vulkan/vulkan.hpp>

#include "common/common.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)
BEGIN_NAMESPACE(vk)

OBJECT_KEY(VkObjectKey, Manager)

#define MANAGED_SINGLETON_VK_OBJECT(class_name) MANAGED_SINGLETON_OBJECT(class_name, VkObjectKey)
#define MANAGED_VK_OBJECT(class_name) MANAGED_OBJECT(class_name, VkObjectKey)

template <typename H, typename I>
class IVkObject : public common::IObject {
   private:
    const VkObjectType vk_type_;  // Maybe not necessary?

   protected:
    H vk_handle_;  // VkHandle
    I vk_info_;    // create_info, allocate_info or so on

    IVkObject(const VkObjectType vk_type) : common::IObject(), vk_type_(vk_type), vk_handle_(VK_NULL_HANDLE) {}

   public:
    IVkObject(const IVkObject &) = delete;
    IVkObject &operator=(const IVkObject &) = delete;

    H get_vk_handle() { return vk_handle_; }
    VkObjectType get_vk_type() { return vk_type_; }
};

class Instance : public IVkObject<VkInstance, VkInstanceCreateInfo> {
    MANAGED_SINGLETON_VK_OBJECT(Instance)

   public:
    VkResult initialize_vk(VkInstanceCreateInfo &vk_info) { return VK_SUCCESS; }
};

class PhysicalDevice : public IVkObject<VkPhysicalDevice, VkPhysicalDeviceProperties2> {
    MANAGED_SINGLETON_VK_OBJECT(PhysicalDevice)

   private:
    VkPhysicalDeviceFeatures2 physical_device_features2_;
    std::shared_ptr<Instance> instance_;

   public:
    VkResult initialize_vk() { return VK_SUCCESS; }
};

class Device : public IVkObject<VkDevice, VkDeviceCreateInfo> {
    MANAGED_SINGLETON_VK_OBJECT(Device)

   private:
    std::shared_ptr<PhysicalDevice> physical_device_;

   public:
    VkResult initialize_vk() { return VK_SUCCESS; }
};

template <typename H, typename I>
class IDviceObject : public IVkObject<H, I> {
   protected:
    std::shared_ptr<Device> device_;

    IDviceObject(const VkObjectType type) : IVkObject(type) {}
};

class Queue : public IDviceObject<VkQueue, VkDeviceQueueCreateInfo> {
    MANAGED_VK_OBJECT(Queue)

   public:
    VkResult initialize_vk(VkDeviceQueueCreateInfo &vk_info) { return VK_SUCCESS; }
};

class Semaphore : public IDviceObject<VkSemaphore, VkSemaphoreCreateInfo> {
   private:
    Semaphore() : IDviceObject(VK_OBJECT_TYPE_SEMAPHORE) {}

   public:
    VkResult initialize_vk(VkSemaphoreCreateInfo &vk_info) { return VK_SUCCESS; }
};

class CommandBuffer : public IDviceObject<VkCommandBuffer, VkCommandBufferAllocateInfo> {
   public:
    CommandBuffer() : IDviceObject(VK_OBJECT_TYPE_COMMAND_BUFFER) {}

    VkResult initialize_vk(VkCommandBufferAllocateInfo &vk_info) { return VK_SUCCESS; }
};

class Fence : public IDviceObject<VkFence, VkFenceCreateInfo> {
   public:
    Fence() : IDviceObject(VK_OBJECT_TYPE_FENCE) {}

    VkResult initialize_vk(VkFenceCreateInfo &vk_info) { return VK_SUCCESS; }
};

class DeviceMemory : public IDviceObject<VkDeviceMemory, VkMemoryAllocateInfo> {
   public:
    DeviceMemory() : IDviceObject(VK_OBJECT_TYPE_DEVICE_MEMORY) {}

    VkResult initialize_vk(VkMemoryAllocateInfo &vk_info) { return VK_SUCCESS; }
};

class Buffer : public IDviceObject<VkBuffer, VkBufferCreateInfo> {
    MANAGED_VK_OBJECT(Buffer)

   public:
    VkResult initialize_vk(VkBufferCreateInfo &vk_info) { return VK_SUCCESS; }
};

class Image : public IDviceObject<VkImage, VkImageCreateInfo> {
   public:
    Image() : IDviceObject(VK_OBJECT_TYPE_IMAGE) {}

    VkResult initialize_vk(VkImageCreateInfo &vk_info) { return VK_SUCCESS; }
};

class Event : public IDviceObject<VkEvent, VkEventCreateInfo> {
   public:
    Event() : IDviceObject(VK_OBJECT_TYPE_EVENT) {}

    VkResult initialize_vk(VkEventCreateInfo &vk_info) { return VK_SUCCESS; }
};

class QueryPool : public IDviceObject<VkQueryPool, VkQueryPoolCreateInfo> {
   private:
    std::vector<std::shared_ptr<Queue>> queue_list_;

   public:
    QueryPool() : IDviceObject(VK_OBJECT_TYPE_QUERY_POOL) {}

    VkResult initialize_vk(VkEventCreateInfo &vk_info) { return VK_SUCCESS; }
};

class BufferView : public IDviceObject<VkBufferView, VkBufferViewCreateInfo> {
   public:
    BufferView() : IDviceObject(VK_OBJECT_TYPE_BUFFER_VIEW) {}

    VkResult initialize_vk(VkBufferViewCreateInfo &vk_info) { return VK_SUCCESS; }
};

class ImageView : public IDviceObject<VkImageView, VkImageViewCreateInfo> {
   public:
    ImageView() : IDviceObject(VK_OBJECT_TYPE_IMAGE_VIEW) {}

    VkResult initialize_vk(VkImageViewCreateInfo &vk_info) { return VK_SUCCESS; }
};

class ShaderModule : public IDviceObject<VkShaderModule, VkShaderModuleCreateInfo> {
   public:
    ShaderModule() : IDviceObject(VK_OBJECT_TYPE_SHADER_MODULE) {}

    VkResult initialize_vk(VkShaderModuleCreateInfo &vk_info) { return VK_SUCCESS; }
};

class PipelineCache : public IDviceObject<VkPipelineCache, VkPipelineCacheCreateInfo> {
   public:
    PipelineCache() : IDviceObject(VK_OBJECT_TYPE_PIPELINE_CACHE) {}

    VkResult initialize_vk(VkPipelineCacheCreateInfo &vk_info) { return VK_SUCCESS; }
};

class PipelineLayout : public IDviceObject<VkPipelineLayout, VkPipelineLayoutCreateInfo> {
   public:
    PipelineLayout() : IDviceObject(VK_OBJECT_TYPE_PIPELINE_LAYOUT) {}

    VkResult initialize_vk(VkPipelineLayoutCreateInfo &vk_info) { return VK_SUCCESS; }
};

class RenderPass : public IDviceObject<VkRenderPass, VkRenderPassCreateInfo> {
   public:
    RenderPass() : IDviceObject(VK_OBJECT_TYPE_RENDER_PASS) {}

    VkResult initialize_vk(VkRenderPassCreateInfo &vk_info) { return VK_SUCCESS; }
};

class ComputePipeline : public IDviceObject<VkPipeline, VkComputePipelineCreateInfo> {
   public:
    ComputePipeline() : IDviceObject(VK_OBJECT_TYPE_PIPELINE) {}

    VkResult initialize_vk(VkComputePipelineCreateInfo &vk_info) { return VK_SUCCESS; }
};

class GraphicsPipeline : public IDviceObject<VkPipeline, VkGraphicsPipelineCreateInfo> {
   public:
    GraphicsPipeline() : IDviceObject(VK_OBJECT_TYPE_PIPELINE) {}

    VkResult initialize_vk(VkGraphicsPipelineCreateInfo &vk_info) { return VK_SUCCESS; }
};

class RayTracingPipeline : public IDviceObject<VkPipeline, VkRayTracingPipelineCreateInfoKHR> {
   public:
    RayTracingPipeline() : IDviceObject(VK_OBJECT_TYPE_PIPELINE) {}

    VkResult initialize_vk(VkRayTracingPipelineCreateInfoKHR &vk_info) { return VK_SUCCESS; }
};

class DescriptorSetLayout : public IDviceObject<VkDescriptorSetLayout, VkDescriptorSetLayoutCreateInfo> {
   public:
    DescriptorSetLayout() : IDviceObject(VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT) {}

    VkResult initialize_vk(VkDescriptorSetLayoutCreateInfo &vk_info) { return VK_SUCCESS; }
};

class Sampler : public IDviceObject<VkSampler, VkSamplerCreateInfo> {
   public:
    Sampler() : IDviceObject(VK_OBJECT_TYPE_SAMPLER) {}

    VkResult initialize_vk(VkSamplerCreateInfo &vk_info) { return VK_SUCCESS; }
};

class DescriptorSet;
class DescriptorPool : public IDviceObject<VkDescriptorPool, VkDescriptorPoolCreateInfo> {
   private:
    std::vector<std::shared_ptr<DescriptorSet>> descriptor_set_list_;

   public:
    DescriptorPool() : IDviceObject(VK_OBJECT_TYPE_DESCRIPTOR_POOL) {}

    VkResult initialize_vk(VkDescriptorPoolCreateInfo &vk_info) { return VK_SUCCESS; }
};

class DescriptorSet : public IDviceObject<VkDescriptorSet, VkDescriptorSetAllocateInfo> {
   public:
    DescriptorSet() : IDviceObject(VK_OBJECT_TYPE_DESCRIPTOR_SET) {}

    VkResult initialize_vk(VkDescriptorSetAllocateInfo &vk_info) { return VK_SUCCESS; }
};

class Framebuffer : public IDviceObject<VkFramebuffer, VkFramebufferCreateInfo> {
   public:
    Framebuffer() : IDviceObject(VK_OBJECT_TYPE_FRAMEBUFFER) {}

    VkResult initialize_vk(VkFramebufferCreateInfo &vk_info) { return VK_SUCCESS; }
};

class CommandPool : public IDviceObject<VkCommandPool, VkCommandPoolCreateInfo> {
   private:
    std::vector<std::shared_ptr<CommandBuffer>> command_buffer_list_;

   public:
    CommandPool() : IDviceObject(VK_OBJECT_TYPE_COMMAND_POOL) {}

    VkResult initialize_vk(VkCommandPoolCreateInfo &vk_info) { return VK_SUCCESS; }
};

class SamplerYcbcrConversion : public IDviceObject<VkSamplerYcbcrConversion, VkSamplerYcbcrConversionCreateInfo> {
   public:
    SamplerYcbcrConversion() : IDviceObject(VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION) {}

    VkResult initialize_vk(VkSamplerYcbcrConversionCreateInfo &vk_info) { return VK_SUCCESS; }
};

class DescriptorUpdateTemplate : public IDviceObject<VkDescriptorUpdateTemplate, VkDescriptorUpdateTemplateCreateInfo> {
   public:
    DescriptorUpdateTemplate() : IDviceObject(VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE) {}

    VkResult initialize_vk(VkDescriptorUpdateTemplateCreateInfo &vk_info) { return VK_SUCCESS; }
};

#ifdef VK_USE_PLATFORM_WIN32_KHR
class Surface : public IDviceObject<VkSurfaceKHR, VkWin32SurfaceCreateInfoKHR> {
    MANAGED_SINGLETON_OBJECT(Surface, VkObjectKey)

   public:
    VkResult initialize_vk(VkWin32SurfaceCreateInfoKHR &vk_info) { return VK_SUCCESS; }
};
#endif  // VK_USE_PLATFORM_WIN32_KHR

class Swapchain : public IDviceObject<VkSwapchainKHR, VkSwapchainCreateInfoKHR> {
   public:
    Swapchain() : IDviceObject(VK_OBJECT_TYPE_SWAPCHAIN_KHR) {}

    VkResult initialize_vk(VkSwapchainCreateInfoKHR &vk_info) { return VK_SUCCESS; }
};

class Display : public IDviceObject<VkDisplayKHR, VkDisplayEventInfoEXT> {
   public:
    Display() : IDviceObject(VK_OBJECT_TYPE_DISPLAY_KHR) {}

    VkResult initialize_vk(VkDisplayEventInfoEXT &vk_info) { return VK_SUCCESS; }
};

class DisplayMode : public IDviceObject<VkDisplayModeKHR, VkDisplayModePropertiesKHR> {
   public:
    DisplayMode() : IDviceObject(VK_OBJECT_TYPE_DISPLAY_MODE_KHR) {}

    VkResult initialize_vk(VkDisplayModePropertiesKHR &vk_info) { return VK_SUCCESS; }
};

class DebugReportCallback : public IDviceObject<VkDebugReportCallbackEXT, VkDebugReportCallbackCreateInfoEXT> {
    MANAGED_SINGLETON_OBJECT(DebugReportCallback, VkObjectKey)

   public:
    VkResult initialize_vk(VkDebugReportCallbackCreateInfoEXT &vk_info) { return VK_SUCCESS; }
};

class DebugUtilsMessenger : public IDviceObject<VkDebugUtilsMessengerEXT, VkDebugUtilsMessengerCreateInfoEXT> {
   public:
    DebugUtilsMessenger() : IDviceObject(VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT) {}

    VkResult initialize_vk(VkDebugUtilsMessengerCreateInfoEXT &vk_info) { return VK_SUCCESS; }
};

class AccelerationStructure : public IDviceObject<VkAccelerationStructureKHR, VkAccelerationStructureCreateInfoKHR> {
   public:
    AccelerationStructure() : IDviceObject(VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR) {}

    VkResult initialize_vk(VkAccelerationStructureCreateInfoKHR &vk_info) { return VK_SUCCESS; }
};

class ValidationCache : public IDviceObject<VkValidationCacheEXT, VkValidationCacheCreateInfoEXT> {
   public:
    ValidationCache() : IDviceObject(VK_OBJECT_TYPE_VALIDATION_CACHE_EXT) {}

    VkResult initialize_vk(VkValidationCacheCreateInfoEXT &vk_info) { return VK_SUCCESS; }
};

class DeferredOperation : public IDviceObject<VkDeferredOperationKHR, VkDeferredOperationKHR> {
   public:
    DeferredOperation() : IDviceObject(VK_OBJECT_TYPE_DEFERRED_OPERATION_KHR) {}

    VkResult initialize_vk(VkDeferredOperationKHR &vk_info) { return VK_SUCCESS; }
};

class PrivateDataSlot : public IDviceObject<VkPrivateDataSlotEXT, VkPrivateDataSlotCreateInfoEXT> {
   public:
    PrivateDataSlot() : IDviceObject(VK_OBJECT_TYPE_PRIVATE_DATA_SLOT_EXT) {}

    VkResult initialize_vk(VkPrivateDataSlotCreateInfoEXT &vk_info) { return VK_SUCCESS; }
};

#define MANAGE_VK_OBJECT(class_name) MANAGE_OBJECT(Manager, ID, class_name)


class Manager {
    INITIALIZE_MANAGER(Manager, VkObjectKey)
    MANAGE_VK_OBJECT(Buffer)
};
#define VK_MGR vk::Manager::get_instance()

END_NAMESPACE(vk)
END_NAMESPACE(gpu)
END_NAMESPACE(ae)

#endif  // AE_GPU_VULKAN_OBJECTS_H
