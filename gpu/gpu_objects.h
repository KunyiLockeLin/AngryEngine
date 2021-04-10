#ifndef AE_GPU_GPU_OBJECTS_H
#define AE_GPU_GPU_OBJECTS_H

#include <unordered_map>
#include <memory>
#include <list>

#include "vulkan_objects.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)
BEGIN_NAMESPACE(object)

enum GPU_OBJECT_TYPE {
    GPU_OBJECT_DEVICE,
    GPU_OBJECT_MODEL,
};

OBJECT_KEY(GPUObjectKey, Manager)

class IGPUObject {
   protected:
    ID id_;
    const GPU_OBJECT_TYPE type_;
    bool active_;  // set active_ in initialize and cleanup

    IGPUObject(const GPU_OBJECT_TYPE type) : type_(type), id_(0), active_(false) {}

   public:
    ID GetID() { return id_; }
    GPU_OBJECT_TYPE GetType() { return type_; }
    bool Active() { return active_; }

    IGPUObject(const IGPUObject &) = delete;
    IGPUObject &operator=(const IGPUObject &) = delete;

    virtual AeResult initialize() = 0;
    virtual AeResult cleanup() = 0;
    virtual AeResult pre_update() = 0;
    virtual AeResult post_update() = 0;
};

class Device : public IGPUObject {
    SINGLETON_OBJECT(Device)

   private:
    std::shared_ptr<vk::Instance> instance_;
    std::shared_ptr<vk::PhysicalDevice> physical_device_;
    std::shared_ptr<vk::Device> device_;
    std::shared_ptr<vk::Surface> surface_;
    std::shared_ptr<vk::DebugReportCallback> debug_report_callback_;

   public:
    virtual AeResult initialize() { return AE_SUCCESS; }
    virtual AeResult cleanup() { return AE_SUCCESS; }
    virtual AeResult pre_update() { return AE_SUCCESS; }
    virtual AeResult post_update() { return AE_SUCCESS; }
};

class Model : public IGPUObject {
    MANAGED_OBJECT(Model, GPUObjectKey)

   private:
    std::shared_ptr<vk::Buffer> buffer_;

   public:
    virtual AeResult initialize() { return AE_SUCCESS; }
    virtual AeResult cleanup() { return AE_SUCCESS; }
    virtual AeResult pre_update() { return AE_SUCCESS; }
    virtual AeResult post_update() { return AE_SUCCESS; }
};

class Texture : public IGPUObject {
   private:
    std::shared_ptr<vk::Image> image_;
    std::shared_ptr<vk::ImageView> image_view_;
};

class IPipeline : public IGPUObject {
   protected:
    // std::vector<ShaderBindingSlot> shader_data_slots_;
};

class GraphicsPipeline : public IPipeline {
   private:
    std::shared_ptr<vk::RenderPass> render_pass_;
    std::shared_ptr<vk::GraphicsPipeline> pipeline_;
    std::shared_ptr<vk::Framebuffer> frambuffer_;

    // std::vector<RenderPass> render_passes_;
};

class ComputePipeline : public IPipeline {
   private:
    std::shared_ptr<vk::ComputePipeline> pipeline_;
};

#define MANAGE_GPU_OBJECT(gpu_object)                                                 \
   private:                                                                           \
    std::unordered_map<ID, std::shared_ptr<##gpu_object>> active_##gpu_object##_map_; \
    std::list<std::shared_ptr<##gpu_object>> unactive_##gpu_object##_list_;           \
                                                                                      \
   public:                                                                            \
    AeResult Create##gpu_object(std::shared_ptr<##gpu_object> &obj) {                 \
        if (!unactive_##gpu_object##_list_.empty()) {                                 \
            obj = unactive_##gpu_object##_list_.back();                               \
            unactive_##gpu_object##_list_.pop_back();                                 \
        } else {                                                                      \
            obj = gpu_object## ::create(gpu_object_key_);                             \
        }                                                                             \
        obj->initialize();                                                            \
        active_##gpu_object##_map_[obj->GetID()] = obj;                               \
        return AE_SUCCESS;                                                            \
    }                                                                                 \
                                                                                      \
    AeResult Get##gpu_object(const ID id, std::shared_ptr<##gpu_object> &obj) {       \
        auto it = active_##gpu_object##_map_.find(id);                                \
        if (it != active_##gpu_object##_map_.end()) {                                 \
            obj = it->second;                                                         \
            return AE_SUCCESS;                                                        \
        }                                                                             \
        return AE_ERROR_UNKNOWN;                                                      \
    }                                                                                 \
                                                                                      \
    AeResult Remove##gpu_object(const ID id) {                                        \
        auto it = active_##gpu_object##_map_.find(id);                                \
        if (it != active_##gpu_object##_map_.end()) {                                 \
            it->second->cleanup();                                                    \
            unactive_##gpu_object##_list_.push_back(it->second);                      \
            active_##gpu_object##_map_.erase(it);                                     \
            return AE_SUCCESS;                                                        \
        }                                                                             \
        return AE_ERROR_UNKNOWN;                                                      \
    }

class Manager {
    SINGLETON_OBJECT(Manager)
    MANAGE_GPU_OBJECT(Model)

   private:
    const GPUObjectKey gpu_object_key_;

   public:
    Manager(const Manager &) = delete;
    Manager &operator=(const Manager &) = delete;

    AeResult initialize() { return AE_SUCCESS; }
    AeResult cleanup() { return AE_SUCCESS; }
    AeResult pre_update() { return AE_SUCCESS; }
    AeResult post_update() { return AE_SUCCESS; }
};
#define MGR Manager::get_instance()

END_NAMESPACE(object)
END_NAMESPACE(gpu)
END_NAMESPACE(ae)

#endif  // AE_GPU_GPU_OBJECTS_H
