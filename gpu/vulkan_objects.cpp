#include "vulkan_objects.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)
BEGIN_NAMESPACE(vk)

Instance::Instance(const VkObjectKey &key) : IVkObject(VK_OBJECT_TYPE_INSTANCE) {}
PhysicalDevice::PhysicalDevice(const VkObjectKey &key) : IVkObject(VK_OBJECT_TYPE_PHYSICAL_DEVICE) {}
Device::Device(const VkObjectKey &key) : IVkObject(VK_OBJECT_TYPE_PHYSICAL_DEVICE) {}
Queue::Queue(const VkObjectKey &key) : IDviceObject(VK_OBJECT_TYPE_QUEUE) {}
Buffer::Buffer(const VkObjectKey &key) : IDviceObject(VK_OBJECT_TYPE_BUFFER) {}

#ifdef VK_USE_PLATFORM_WIN32_KHR
Surface::Surface(const VkObjectKey &key) : IDviceObject(VK_OBJECT_TYPE_SURFACE_KHR) {}
#endif  // VK_USE_PLATFORM_WIN32_KHR

DebugReportCallback::DebugReportCallback(const VkObjectKey &key) : IDviceObject(VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT) {}

Manager::Manager() {}

AeResult Manager::initialize() { return AE_SUCCESS; }

AeResult Manager::cleanup() { return AE_SUCCESS; }

END_NAMESPACE(vk)
END_NAMESPACE(gpu)
END_NAMESPACE(ae)
