#ifndef AE_GPU_VULKAN_OBJECTS_H
#define AE_GPU_VULKAN_OBJECTS_H

#if defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <memory>
#include <vulkan/vulkan.hpp>

#include "common/common.h"
#include "code_generator/generated_config_struct_enum.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)
BEGIN_NAMESPACE(vk)

OBJECT_KEY(VkObjectKey, Manager)

template <typename H, typename I>
class IVkObject {
   protected:
    H vk_handle_;                 // VkHandle
    const VkObjectType vk_type_;  // Maybe not necessary?
    I vk_info_;                   // create_info, allocate_info or so on
    bool active_;                 // set active_ in initialize and cleanup

    IVkObject(const VkObjectType vk_type) : vk_type_(vk_type), vk_handle_(VK_NULL_HANDLE), active_(false) {}

   public:
    H GetVkHandle() { return vk_handle_; }
    VkObjectType GetVkType() { return vk_type_; }
    bool Active() { return active_; }

    IVkObject(const IVkObject &) = delete;
    IVkObject &operator=(const IVkObject &) = delete;

    virtual VkResult initialize(const I &vk_info) = 0;
    virtual VkResult cleanup() = 0;
};

class Instance : public IVkObject<VkInstance, VkInstanceCreateInfo> {
    SINGLETON_OBJECT(Instance)

   public:
    virtual VkResult initialize(const VkInstanceCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class PhysicalDevice : public IVkObject<VkPhysicalDevice, VkPhysicalDeviceProperties2> {
    SINGLETON_OBJECT(PhysicalDevice)

   private:
    VkPhysicalDeviceFeatures2 physical_device_features2_;
    std::shared_ptr<Instance> instance_;

   public:
    virtual VkResult initialize(const VkPhysicalDeviceProperties2 &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class Device : public IVkObject<VkDevice, VkDeviceCreateInfo> {
    SINGLETON_OBJECT(Device)

   private:
    std::shared_ptr<PhysicalDevice> physical_device_;

   public:
    virtual VkResult initialize(const VkDeviceCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

template <typename H, typename I>
class IDviceObject : public IVkObject<H, I> {
   protected:
    std::shared_ptr<Device> device_;

    IDviceObject(const VkObjectType type) : IVkObject(type) {}
};

class Queue : public IDviceObject<VkQueue, VkDeviceQueueCreateInfo> {
    MANAGED_OBJECT(Queue, VkObjectKey)

   public:
    virtual VkResult initialize(const VkDeviceQueueCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class Semaphore : public IDviceObject<VkSemaphore, VkSemaphoreCreateInfo> {
   private:
    Semaphore() : IDviceObject(VK_OBJECT_TYPE_SEMAPHORE) {}

   public:
    virtual VkResult initialize(const VkSemaphoreCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class CommandBuffer : public IDviceObject<VkCommandBuffer, VkCommandBufferAllocateInfo> {
   public:
    CommandBuffer() : IDviceObject(VK_OBJECT_TYPE_COMMAND_BUFFER) {}

    virtual VkResult initialize(const VkCommandBufferAllocateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class Fence : public IDviceObject<VkFence, VkFenceCreateInfo> {
   public:
    Fence() : IDviceObject(VK_OBJECT_TYPE_FENCE) {}

    virtual VkResult initialize(const VkFenceCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class DeviceMemory : public IDviceObject<VkDeviceMemory, VkMemoryAllocateInfo> {
   public:
    DeviceMemory() : IDviceObject(VK_OBJECT_TYPE_DEVICE_MEMORY) {}

    virtual VkResult initialize(const VkMemoryAllocateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class Buffer : public IDviceObject<VkBuffer, VkBufferCreateInfo> {
    MANAGED_OBJECT(Buffer, VkObjectKey)

   public:
    virtual VkResult initialize(const VkBufferCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class Image : public IDviceObject<VkImage, VkImageCreateInfo> {
   public:
    Image() : IDviceObject(VK_OBJECT_TYPE_IMAGE) {}

    virtual VkResult initialize(const VkImageCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class Event : public IDviceObject<VkEvent, VkEventCreateInfo> {
   public:
    Event() : IDviceObject(VK_OBJECT_TYPE_EVENT) {}

    virtual VkResult initialize(const VkEventCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class QueryPool : public IDviceObject<VkQueryPool, VkQueryPoolCreateInfo> {
   private:
    std::vector<std::shared_ptr<Queue>> queue_list_;

   public:
    QueryPool() : IDviceObject(VK_OBJECT_TYPE_QUERY_POOL) {}

    virtual VkResult initialize(const VkEventCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class BufferView : public IDviceObject<VkBufferView, VkBufferViewCreateInfo> {
   public:
    BufferView() : IDviceObject(VK_OBJECT_TYPE_BUFFER_VIEW) {}

    virtual VkResult initialize(const VkBufferViewCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class ImageView : public IDviceObject<VkImageView, VkImageViewCreateInfo> {
   public:
    ImageView() : IDviceObject(VK_OBJECT_TYPE_IMAGE_VIEW) {}

    virtual VkResult initialize(const VkImageViewCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class ShaderModule : public IDviceObject<VkShaderModule, VkShaderModuleCreateInfo> {
   public:
    ShaderModule() : IDviceObject(VK_OBJECT_TYPE_SHADER_MODULE) {}

    virtual VkResult initialize(const VkShaderModuleCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class PipelineCache : public IDviceObject<VkPipelineCache, VkPipelineCacheCreateInfo> {
   public:
    PipelineCache() : IDviceObject(VK_OBJECT_TYPE_PIPELINE_CACHE) {}

    virtual VkResult initialize(const VkPipelineCacheCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class PipelineLayout : public IDviceObject<VkPipelineLayout, VkPipelineLayoutCreateInfo> {
   public:
    PipelineLayout() : IDviceObject(VK_OBJECT_TYPE_PIPELINE_LAYOUT) {}

    virtual VkResult initialize(const VkPipelineLayoutCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class RenderPass : public IDviceObject<VkRenderPass, VkRenderPassCreateInfo> {
   public:
    RenderPass() : IDviceObject(VK_OBJECT_TYPE_RENDER_PASS) {}

    virtual VkResult initialize(const VkRenderPassCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class ComputePipeline : public IDviceObject<VkPipeline, VkComputePipelineCreateInfo> {
   public:
    ComputePipeline() : IDviceObject(VK_OBJECT_TYPE_PIPELINE) {}

    virtual VkResult initialize(const VkComputePipelineCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class GraphicsPipeline : public IDviceObject<VkPipeline, VkGraphicsPipelineCreateInfo> {
   public:
    GraphicsPipeline() : IDviceObject(VK_OBJECT_TYPE_PIPELINE) {}

    virtual VkResult initialize(const VkGraphicsPipelineCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class RayTracingPipeline : public IDviceObject<VkPipeline, VkRayTracingPipelineCreateInfoKHR> {
   public:
    RayTracingPipeline() : IDviceObject(VK_OBJECT_TYPE_PIPELINE) {}

    virtual VkResult initialize(const VkRayTracingPipelineCreateInfoKHR &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class DescriptorSetLayout : public IDviceObject<VkDescriptorSetLayout, VkDescriptorSetLayoutCreateInfo> {
   public:
    DescriptorSetLayout() : IDviceObject(VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT) {}

    virtual VkResult initialize(const VkDescriptorSetLayoutCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class Sampler : public IDviceObject<VkSampler, VkSamplerCreateInfo> {
   public:
    Sampler() : IDviceObject(VK_OBJECT_TYPE_SAMPLER) {}

    virtual VkResult initialize(const VkSamplerCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class DescriptorSet;
class DescriptorPool : public IDviceObject<VkDescriptorPool, VkDescriptorPoolCreateInfo> {
   private:
    std::vector<std::shared_ptr<DescriptorSet>> descriptor_set_list_;

   public:
    DescriptorPool() : IDviceObject(VK_OBJECT_TYPE_DESCRIPTOR_POOL) {}

    virtual VkResult initialize(const VkDescriptorPoolCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class DescriptorSet : public IDviceObject<VkDescriptorSet, VkDescriptorSetAllocateInfo> {
   public:
    DescriptorSet() : IDviceObject(VK_OBJECT_TYPE_DESCRIPTOR_SET) {}

    virtual VkResult initialize(const VkDescriptorSetAllocateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class Framebuffer : public IDviceObject<VkFramebuffer, VkFramebufferCreateInfo> {
   public:
    Framebuffer() : IDviceObject(VK_OBJECT_TYPE_FRAMEBUFFER) {}

    virtual VkResult initialize(const VkFramebufferCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class CommandPool : public IDviceObject<VkCommandPool, VkCommandPoolCreateInfo> {
   private:
    std::vector<std::shared_ptr<CommandBuffer>> command_buffer_list_;

   public:
    CommandPool() : IDviceObject(VK_OBJECT_TYPE_COMMAND_POOL) {}

    virtual VkResult initialize(const VkCommandPoolCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class SamplerYcbcrConversion : public IDviceObject<VkSamplerYcbcrConversion, VkSamplerYcbcrConversionCreateInfo> {
   public:
    SamplerYcbcrConversion() : IDviceObject(VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION) {}

    virtual VkResult initialize(const VkSamplerYcbcrConversionCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class DescriptorUpdateTemplate : public IDviceObject<VkDescriptorUpdateTemplate, VkDescriptorUpdateTemplateCreateInfo> {
   public:
    DescriptorUpdateTemplate() : IDviceObject(VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE) {}

    virtual VkResult initialize(const VkDescriptorUpdateTemplateCreateInfo &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

#ifdef VK_USE_PLATFORM_WIN32_KHR
class Surface : public IDviceObject<VkSurfaceKHR, VkWin32SurfaceCreateInfoKHR> {
    SINGLETON_OBJECT(Surface)

   public:
    virtual VkResult initialize(const VkWin32SurfaceCreateInfoKHR &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};
#endif  // VK_USE_PLATFORM_WIN32_KHR

class Swapchain : public IDviceObject<VkSwapchainKHR, VkSwapchainCreateInfoKHR> {
   public:
    Swapchain() : IDviceObject(VK_OBJECT_TYPE_SWAPCHAIN_KHR) {}

    virtual VkResult initialize(const VkSwapchainCreateInfoKHR &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class Display : public IDviceObject<VkDisplayKHR, VkDisplayEventInfoEXT> {
   public:
    Display() : IDviceObject(VK_OBJECT_TYPE_DISPLAY_KHR) {}

    virtual VkResult initialize(const VkDisplayEventInfoEXT &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class DisplayMode : public IDviceObject<VkDisplayModeKHR, VkDisplayModePropertiesKHR> {
   public:
    DisplayMode() : IDviceObject(VK_OBJECT_TYPE_DISPLAY_MODE_KHR) {}

    virtual VkResult initialize(const VkDisplayModePropertiesKHR &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class DebugReportCallback : public IDviceObject<VkDebugReportCallbackEXT, VkDebugReportCallbackCreateInfoEXT> {
    SINGLETON_OBJECT(DebugReportCallback)

   public:
    virtual VkResult initialize(const VkDebugReportCallbackCreateInfoEXT &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class DebugUtilsMessenger : public IDviceObject<VkDebugUtilsMessengerEXT, VkDebugUtilsMessengerCreateInfoEXT> {
   public:
    DebugUtilsMessenger() : IDviceObject(VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT) {}

    virtual VkResult initialize(const VkDebugUtilsMessengerCreateInfoEXT &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class AccelerationStructure : public IDviceObject<VkAccelerationStructureKHR, VkAccelerationStructureCreateInfoKHR> {
   public:
    AccelerationStructure() : IDviceObject(VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR) {}

    virtual VkResult initialize(const VkAccelerationStructureCreateInfoKHR &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class ValidationCache : public IDviceObject<VkValidationCacheEXT, VkValidationCacheCreateInfoEXT> {
   public:
    ValidationCache() : IDviceObject(VK_OBJECT_TYPE_VALIDATION_CACHE_EXT) {}

    virtual VkResult initialize(const VkValidationCacheCreateInfoEXT &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class DeferredOperation : public IDviceObject<VkDeferredOperationKHR, VkDeferredOperationKHR> {
   public:
    DeferredOperation() : IDviceObject(VK_OBJECT_TYPE_DEFERRED_OPERATION_KHR) {}

    virtual VkResult initialize(const VkDeferredOperationKHR &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

class PrivateDataSlot : public IDviceObject<VkPrivateDataSlotEXT, VkPrivateDataSlotCreateInfoEXT> {
   public:
    PrivateDataSlot() : IDviceObject(VK_OBJECT_TYPE_PRIVATE_DATA_SLOT_EXT) {}

    virtual VkResult initialize(const VkPrivateDataSlotCreateInfoEXT &vk_info) { return VK_SUCCESS; }
    virtual VkResult cleanup() { return VK_SUCCESS; }
};

#define MANAGE_VK_OBJECT(vk_object, vk_handle, vk_info)                                         \
   private:                                                                                     \
    std::unordered_map<##vk_handle, std::shared_ptr<##vk_object>> active_vk_##vk_object##_map_; \
    std::list<std::shared_ptr<##vk_object>> unactive_vk_##vk_object##_list_;                    \
                                                                                                \
   public:                                                                                      \
    AeResult Create##vk_object(const vk_info &info, std::shared_ptr<##vk_object> &obj) {        \
        if (!unactive_vk_##vk_object##_list_.empty()) {                                         \
            obj = unactive_vk_##vk_object##_list_.back();                                       \
            unactive_vk_##vk_object##_list_.pop_back();                                         \
        } else {                                                                                \
            obj = vk_object## ::create(vk_object_key_);                                         \
        }                                                                                       \
        obj->initialize(info);                                                                  \
        active_vk_##vk_object##_map_[obj->GetVkHandle()] = obj;                                 \
        return AE_SUCCESS;                                                                      \
    }                                                                                           \
                                                                                                \
    AeResult Get##vk_object(const vk_handle handle, std::shared_ptr<##vk_object> &obj) {        \
        auto it = active_vk_##vk_object##_map_.find(handle);                                    \
        if (it != active_vk_##vk_object##_map_.end()) {                                         \
            obj = it->second;                                                                   \
            return AE_SUCCESS;                                                                  \
        }                                                                                       \
        return AE_ERROR_UNKNOWN;                                                                \
    }                                                                                           \
                                                                                                \
    AeResult Remove##vk_object(const vk_handle handle) {                                        \
        auto it = active_vk_##vk_object##_map_.find(handle);                                    \
        if (it != active_vk_##vk_object##_map_.end()) {                                         \
            it->second->cleanup();                                                              \
            unactive_vk_##vk_object##_list_.push_back(it->second);                              \
            active_vk_##vk_object##_map_.erase(it);                                             \
            return AE_SUCCESS;                                                                  \
        }                                                                                       \
        return AE_ERROR_UNKNOWN;                                                                \
    }

class Manager {
    SINGLETON_OBJECT(Manager)
    MANAGE_VK_OBJECT(Queue, VkQueue, VkDeviceQueueCreateInfo)
    MANAGE_VK_OBJECT(Buffer, VkBuffer, VkBufferCreateInfo)

   private:
    const VkObjectKey vk_object_key_;

   public:
    Manager(const Manager &) = delete;
    Manager &operator=(const Manager &) = delete;

    AeResult initialize() { return AE_SUCCESS; }
    AeResult cleanup() { return AE_SUCCESS; }
};
#define MGR Manager::get_instance()

END_NAMESPACE(vk)
END_NAMESPACE(gpu)
END_NAMESPACE(ae)

#endif  // AE_GPU_VULKAN_OBJECTS_H
