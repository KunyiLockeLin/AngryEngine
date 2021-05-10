#ifndef AE_GPU_GPU_OBJECTS_H
#define AE_GPU_GPU_OBJECTS_H

#include <unordered_map>
#include <memory>
#include <list>

#ifdef _WIN32
#include <windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#endif  // _WIN32

#include <vulkan/vulkan.hpp>

#include "gpu.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)
BEGIN_NAMESPACE(object)

OBJECT_KEY(GPUObjectKey, Manager)

#define MANAGED_SINGLETON_GPU_OBJECT(class_name) MANAGED_SINGLETON_OBJECT(class_name, GPUObjectKey)
#define MANAGED_GPU_OBJECT(class_name) MANAGED_OBJECT(class_name, GPUObjectKey)

class IGPUObject : public common::IObject {
   private:
    const AeGPUType type_;

   protected:
    IGPUObject(const AeGPUType type) : common::IObject(), type_(type) {}

   public:
    IGPUObject(const IGPUObject &) = delete;
    IGPUObject &operator=(const IGPUObject &) = delete;

    AeGPUType get_type() { return type_; }

    virtual AeResult pre_update() { return AE_SUCCESS; }
    virtual AeResult post_update() { return AE_SUCCESS; }
};

class Rendering;
class Device : public IGPUObject {
    MANAGED_SINGLETON_GPU_OBJECT(Device)

   private:
    DeviceInfo device_info_;

    VkInstance instance_;
    AeResult create_instance();
    AeResult check_support_instance_layers(std::vector<const char *> &layers);
    AeResult check_support_instance_extensions(std::vector<const char *> &extensions);

    VkPhysicalDevice physical_device_;
    VkPhysicalDeviceProperties2 phy_dev_props2_;
    VkPhysicalDeviceFeatures2 phy_dev_feats2_;
    AeResult pick_physical_device();

    VkDevice device_;
    AeResult create_device();
    AeResult check_support_device_layers(std::vector<const char *> &layers);
    AeResult check_support_device_extensions(std::vector<const char *> &extensions);
    AeResult get_queue_family_props(std::vector<VkQueueFamilyProperties> &queue_props);

    VkDebugReportCallbackEXT debug_report_callback_;
    AeResult create_debug_report_callback();

   public:
    AeResult initialize(const DeviceInfo &device_info);
    const VkInstance &get_vk_instance();
    const VkPhysicalDevice &get_vk_physical_device();
    const VkDevice &get_vk_device();

    static VkBool32 debug_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
                                          size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage,
                                          void *pUserData);
    AeResult initialize_rendering(const RenderingInfo &render_info, std::shared_ptr<Rendering> &rendering);
};

class Queues : public IGPUObject {
    MANAGED_SINGLETON_GPU_OBJECT(Queues)

   private:
    std::shared_ptr<Device> device_;

    struct QueueFamily {
        VkQueueFamilyProperties family_props_;
        std::vector<VkQueue> queues_;
    };
    std::vector<QueueFamily> queue_familys_;
    AeResult get_device_queues(const std::vector<VkQueueFamilyProperties> &queue_family_props);

   public:
    AeResult initialize(const std::shared_ptr<Device> &device, const std::vector<VkQueueFamilyProperties> &queue_family_props);
};

class Framebuffer;
class RenderPass;
class GraphicsPipeline;
class Rendering : public IGPUObject {
    MANAGED_SINGLETON_GPU_OBJECT(Rendering)

   private:
    std::shared_ptr<Device> device_;
    RenderingInfo render_info_;

    VkSurfaceKHR surface_;
    AeResult create_surface();

    std::vector<std::shared_ptr<Framebuffer>> frambuffers_;
    std::vector<std::shared_ptr<RenderPass>> render_passes_;
    std::vector<std::shared_ptr<GraphicsPipeline>> graphics_pipelines_;

   public:
    AeResult initialize(const std::shared_ptr<Device> &device, const RenderingInfo &render_info);
};

class Framebuffer : public IGPUObject {
    MANAGED_GPU_OBJECT(Framebuffer)

   private:
    VkFramebuffer frambuffer_;
    // swapchain, depth, stencil;
};

class RenderPass : public IGPUObject {
    MANAGED_GPU_OBJECT(RenderPass)

   private:
    VkRenderPass render_pass_;
};

class IPipeline : public IGPUObject {
   protected:
    VkPipeline pipeline_;
    std::vector<VkShaderModule> shader_modules_;
    // std::vector<ShaderBindingSlot> shader_data_slots_;
};

class GraphicsPipeline : public IPipeline {
    MANAGED_GPU_OBJECT(GraphicsPipeline)

   private:
};
/*

class Model : public IGPUObject {
    MANAGED_GPU_OBJECT(Model)

   private:
    VkBuffer buffer_;
    ModelInfo model_info_;

   public:
    virtual AeResult initialize(ModelInfo &model_info);
    virtual AeResult pre_update() { return AE_SUCCESS; }
    virtual AeResult post_update() { return AE_SUCCESS; }
};

class Thread : public IGPUObject {};


class Texture : public IGPUObject {
   private:
    VkImage image_;
    VkImageView image_view_;
};

class IPipeline : public IGPUObject {
   protected:
    VkPipeline pipeline_;
    std::vector<VkShaderModule> shader_modules_
    // std::vector<ShaderBindingSlot> shader_data_slots_;
};

class Brush : public IPipeline {
   private:
    // graphics pipeline
};

class Computer : public IPipeline {
   private:
    // computer pipeline
};
*/
#define MANAGE_GPU_OBJECT(class_name) MANAGE_OBJECT(Manager, ID, class_name)

class Manager {
    INITIALIZE_MANAGER(Manager, GPUObjectKey)
    MANAGE_GPU_OBJECT(Framebuffer)
    MANAGE_GPU_OBJECT(RenderPass)
    MANAGE_GPU_OBJECT(GraphicsPipeline)

   public:
    AeResult pre_update();
    AeResult post_update();
};
#define GPU_MGR Manager::get_instance()

END_NAMESPACE(object)
END_NAMESPACE(gpu)
END_NAMESPACE(ae)

#endif  // AE_GPU_GPU_OBJECTS_H
