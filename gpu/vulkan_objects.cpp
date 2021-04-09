#include "vulkan_objects.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)
BEGIN_NAMESPACE(vk)

Instance::Instance() : IVkObject(VK_OBJECT_TYPE_INSTANCE) {}
PhysicalDevice::PhysicalDevice() : IVkObject(VK_OBJECT_TYPE_PHYSICAL_DEVICE) {}
Device::Device() : IVkObject(VK_OBJECT_TYPE_PHYSICAL_DEVICE) {}
Queue::Queue(const VkObjectKey &key) : IDviceObject(VK_OBJECT_TYPE_QUEUE) {}
Buffer::Buffer(const VkObjectKey &key) : IDviceObject(VK_OBJECT_TYPE_BUFFER) {}

#ifdef VK_USE_PLATFORM_WIN32_KHR
Surface::Surface() : IDviceObject(VK_OBJECT_TYPE_SURFACE_KHR) {}
#endif  // VK_USE_PLATFORM_WIN32_KHR

DebugReportCallback::DebugReportCallback() : IDviceObject(VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT) {}

END_NAMESPACE(vk)
END_NAMESPACE(gpu)
END_NAMESPACE(ae)
