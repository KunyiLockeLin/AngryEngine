#include "gpu_objects.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)
BEGIN_NAMESPACE(object)

static inline const char* string_VkResult(VkResult err) {
    switch (err) {
        CASE_STR(VK_SUCCESS);
        CASE_STR(VK_NOT_READY);
        CASE_STR(VK_TIMEOUT);
        CASE_STR(VK_EVENT_SET);
        CASE_STR(VK_EVENT_RESET);
        CASE_STR(VK_INCOMPLETE);
        CASE_STR(VK_ERROR_OUT_OF_HOST_MEMORY);
        CASE_STR(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        CASE_STR(VK_ERROR_INITIALIZATION_FAILED);
        CASE_STR(VK_ERROR_DEVICE_LOST);
        CASE_STR(VK_ERROR_MEMORY_MAP_FAILED);
        CASE_STR(VK_ERROR_LAYER_NOT_PRESENT);
        CASE_STR(VK_ERROR_EXTENSION_NOT_PRESENT);
        CASE_STR(VK_ERROR_FEATURE_NOT_PRESENT);
        CASE_STR(VK_ERROR_INCOMPATIBLE_DRIVER);
        CASE_STR(VK_ERROR_TOO_MANY_OBJECTS);
        CASE_STR(VK_ERROR_FORMAT_NOT_SUPPORTED);
        CASE_STR(VK_ERROR_FRAGMENTED_POOL);
        CASE_STR(VK_ERROR_UNKNOWN);
        CASE_STR(VK_ERROR_OUT_OF_POOL_MEMORY);
        CASE_STR(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        CASE_STR(VK_ERROR_FRAGMENTATION);
        CASE_STR(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
        CASE_STR(VK_ERROR_SURFACE_LOST_KHR);
        CASE_STR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        CASE_STR(VK_SUBOPTIMAL_KHR);
        CASE_STR(VK_ERROR_OUT_OF_DATE_KHR);
        CASE_STR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        CASE_STR(VK_ERROR_VALIDATION_FAILED_EXT);
        CASE_STR(VK_ERROR_INVALID_SHADER_NV);
        CASE_STR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        CASE_STR(VK_ERROR_NOT_PERMITTED_EXT);
        CASE_STR(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
        default:
            return "UNKNOWN_VkResult";
    }
}

#ifndef NDEBUG
#define ASSERT_VK_SUCCESS(condition) ASSERT_RESULT(condition, VkResult, VK_SUCCESS)
#define ASSERT_VK_NULL_HANDLE(condition) ASSERT(condition, "VK_NULL_HANDLE");
#else
#define ASSERT_VK_SUCCESS(condition) condition
#define ASSERT_VK_NULL_HANDLE(condition) condition;
#endif

Device::Device(const GPUObjectKey& key)
    : IGPUObject(AE_GPU_DEVICE), instance_(VK_NULL_HANDLE), physical_device_(VK_NULL_HANDLE), device_(VK_NULL_HANDLE) {}

const VkInstance& Device::get_vk_instance() { return instance_; }
const VkPhysicalDevice& Device::get_vk_physical_device() { return physical_device_; }
const VkDevice& Device::get_vk_device() { return device_; }

AeResult Device::initialize(const DeviceInfo& device_info) {
    device_info_ = device_info;
    ASSERT_SUCCESS(create_instance());
    ASSERT_SUCCESS(pick_physical_device());
    ASSERT_SUCCESS(create_device());
    ASSERT_SUCCESS(create_debug_report_callback());
    return AE_SUCCESS;
}

AeResult Device::create_instance() {
    std::vector<std::string> app_vers = COM_ENCODE.split<std::string>(device_info_.application_version_, ".");
    std::vector<std::string> eng_vers = COM_ENCODE.split<std::string>(device_info_.engine_version_, ".");
    std::vector<std::string> api_vers = COM_ENCODE.split<std::string>(device_info_.api_version_, ".");

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = device_info_.application_name_.c_str();
    app_info.applicationVersion = VK_MAKE_VERSION(atoi(app_vers[0].c_str()), atoi(app_vers[1].c_str()), atoi(app_vers[2].c_str()));
    app_info.pEngineName = device_info_.engine_name_.c_str();
    app_info.engineVersion = VK_MAKE_VERSION(atoi(eng_vers[0].c_str()), atoi(eng_vers[1].c_str()), atoi(eng_vers[2].c_str()));
    app_info.apiVersion = VK_MAKE_VERSION(atoi(api_vers[0].c_str()), atoi(api_vers[1].c_str()), atoi(api_vers[2].c_str()));

    if (device_info_.layer_names_.empty()) {
        ASSERT_SUCCESS(check_support_instance_layers(device_info_.layer_names_));
    }

    std::vector<const char*> extensions = {VK_KHR_SURFACE_EXTENSION_NAME};

#ifdef VK_USE_PLATFORM_WIN32_KHR
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif  // VK_USE_PLATFORM_WIN32_KHR

    if (!device_info_.layer_names_.empty()) {
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }
    if (extensions.empty()) {
        ASSERT_SUCCESS(check_support_instance_extensions(extensions));
    }

    VkInstanceCreateInfo ins_ci = {};
    ins_ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ins_ci.pApplicationInfo = &app_info;
    ins_ci.enabledLayerCount = static_cast<uint32_t>(device_info_.layer_names_.size());
    ins_ci.ppEnabledLayerNames = device_info_.layer_names_.data();
    ins_ci.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    ins_ci.ppEnabledExtensionNames = extensions.data();
    ASSERT_VK_SUCCESS(vkCreateInstance(&ins_ci, nullptr, &instance_));
    return AE_SUCCESS;
}

AeResult Device::check_support_instance_layers(std::vector<const char*>& layers) {
    uint32_t count = 0;
    ASSERT_VK_SUCCESS(vkEnumerateInstanceLayerProperties(&count, nullptr));
    if (!count) {
        LOG("VkInstance doesn't support any layer.");
        layers.clear();
        return AE_SUCCESS;
    }

    std::vector<VkLayerProperties> prpos(count);
    ASSERT_VK_SUCCESS(vkEnumerateInstanceLayerProperties(&count, prpos.data()));

    auto it = layers.begin();
    while (it != layers.end()) {
        bool supported = false;
        for (const auto& prop : prpos) {
            if (strncmp(prop.layerName, *it, VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                LOG("VkInstance supports " + *it);
                supported = true;
                break;
            }
        }
        if (supported) {
            ++it;
        } else {
            LOG("VkInstance doesn't support " + *it);
            it = layers.erase(it);
        }
    }
    return AE_SUCCESS;
}

AeResult Device::check_support_instance_extensions(std::vector<const char*>& extensions) {
    uint32_t count = 0;
    ASSERT_VK_SUCCESS(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
    if (!count) {
        LOG("VkInstance doesn't support any extension.");
        extensions.clear();
        return AE_SUCCESS;
    }

    std::vector<VkExtensionProperties> prpos(count);
    ASSERT_VK_SUCCESS(vkEnumerateInstanceExtensionProperties(nullptr, &count, prpos.data()));

    auto it = extensions.begin();
    while (it != extensions.end()) {
        bool supported = false;
        for (const auto& prop : prpos) {
            if (strncmp(prop.extensionName, *it, VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                LOG("VkInstance supports " + *it);
                supported = true;
                break;
            }
        }
        if (supported) {
            ++it;
        } else {
            LOG("VkInstance doesn't support " + *it);
            it = extensions.erase(it);
        }
    }
    return AE_SUCCESS;
}

AeResult Device::pick_physical_device() {
    uint32_t count = 0;
    ASSERT_VK_SUCCESS(vkEnumeratePhysicalDevices(instance_, &count, nullptr));
    if (!count) {
        LOG("No physical devices exist.");
        return AE_ERROR_UNKNOWN;
    }

    std::vector<VkPhysicalDevice> phy(count);
    ASSERT_VK_SUCCESS(vkEnumeratePhysicalDevices(instance_, &count, phy.data()));
    physical_device_ = phy[0];

    phy_dev_props2_ = {};
    phy_dev_props2_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkGetPhysicalDeviceProperties2(physical_device_, &phy_dev_props2_);

    phy_dev_feats2_ = {};
    phy_dev_feats2_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    vkGetPhysicalDeviceFeatures2(physical_device_, &phy_dev_feats2_);
    return AE_SUCCESS;
}

AeResult Device::create_device() {
    VkPhysicalDeviceDescriptorIndexingFeatures desc_indexing_features = {};
    desc_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    desc_indexing_features.runtimeDescriptorArray = VK_TRUE;

    VkPhysicalDeviceFeatures2 features2 = {};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &desc_indexing_features;
    features2.features.samplerAnisotropy = VK_TRUE;
    features2.features.fillModeNonSolid = VK_TRUE;
    features2.features.multiViewport = VK_TRUE;
    features2.features.geometryShader = VK_TRUE;
    features2.features.tessellationShader = VK_TRUE;
    features2.features.sampleRateShading = VK_TRUE;
    features2.features.wideLines = VK_TRUE;
    features2.features.vertexPipelineStoresAndAtomics = VK_TRUE;
    features2.features.fragmentStoresAndAtomics = VK_TRUE;
    features2.features.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
    features2.features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
    features2.features.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
    features2.features.shaderStorageImageArrayDynamicIndexing = VK_TRUE;

    std::vector<const char*> layers;
    if (layers.empty()) {
        ASSERT_SUCCESS(check_support_device_layers(layers));
    }

    std::vector<const char*> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    if (extensions.empty()) {
        ASSERT_SUCCESS(check_support_device_extensions(extensions));
    }

    std::vector<VkQueueFamilyProperties> queue_props;
    ASSERT_SUCCESS(get_queue_family_props(queue_props));

    std::vector<VkDeviceQueueCreateInfo> queue_cis;
    const uint32_t queue_family_count = static_cast<uint32_t>(queue_props.size());
    queue_cis.resize(queue_family_count);
    uint32_t i = 0;
    std::array<float, 100> queue_priorities;
    for (auto& priority : queue_priorities) {
        priority = 1.f - 0.1f * i;
    }
    i = 0;
    for (auto& queue_ci : queue_cis) {
        queue_ci = {};
        queue_ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_ci.queueFamilyIndex = i;
        queue_ci.queueCount = queue_props[i].queueCount;
        queue_ci.pQueuePriorities = queue_priorities.data();
        ++i;
    }

    VkDeviceCreateInfo dev_ci = {};
    dev_ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    dev_ci.pNext = &features2;
    dev_ci.queueCreateInfoCount = static_cast<uint32_t>(queue_cis.size());
    dev_ci.pQueueCreateInfos = queue_cis.data();
    dev_ci.enabledLayerCount = static_cast<uint32_t>(layers.size());
    dev_ci.ppEnabledLayerNames = layers.data();
    dev_ci.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    dev_ci.ppEnabledExtensionNames = extensions.data();
    ASSERT_VK_SUCCESS(vkCreateDevice(physical_device_, &dev_ci, nullptr, &device_));

    std::shared_ptr<Device> device;
    ASSERT_SUCCESS(GPU_MGR.get<Device>(device));
    std::shared_ptr<Queues> queues;
    ASSERT_SUCCESS(GPU_MGR.get<Queues>(queues));
    ASSERT_SUCCESS(queues->initialize(device, queue_props));
    return AE_SUCCESS;
}

AeResult Device::check_support_device_layers(std::vector<const char*>& layers) {
    uint32_t count = 0;
    ASSERT_VK_SUCCESS(vkEnumerateDeviceLayerProperties(physical_device_, &count, nullptr));
    if (!count) {
        LOG("VkDevice doesn't support any layer.");
        layers.clear();
        return AE_SUCCESS;
    }

    std::vector<VkLayerProperties> prpos(count);
    ASSERT_VK_SUCCESS(vkEnumerateDeviceLayerProperties(physical_device_, &count, prpos.data()));

    auto it = layers.begin();
    while (it != layers.end()) {
        bool supported = false;
        for (const auto& prop : prpos) {
            if (strncmp(prop.layerName, *it, VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                LOG("VkDevice supports " + *it);
                supported = true;
                break;
            }
        }
        if (supported) {
            ++it;
        } else {
            LOG("VkDevice doesn't support " + *it);
            it = layers.erase(it);
        }
    }
    return AE_SUCCESS;
}

AeResult Device::check_support_device_extensions(std::vector<const char*>& extensions) {
    uint32_t count = 0;
    ASSERT_VK_SUCCESS(vkEnumerateDeviceExtensionProperties(physical_device_, nullptr, &count, nullptr));
    if (!count) {
        LOG("VkDevice doesn't support any extension.");
        extensions.clear();
        return AE_SUCCESS;
    }

    std::vector<VkExtensionProperties> prpos(count);
    ASSERT_VK_SUCCESS(vkEnumerateDeviceExtensionProperties(physical_device_, nullptr, &count, prpos.data()));

    auto it = extensions.begin();
    while (it != extensions.end()) {
        bool supported = false;
        for (const auto& prop : prpos) {
            if (strncmp(prop.extensionName, *it, VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                LOG("VkDevice supports " + *it);
                supported = true;
                break;
            }
        }
        if (supported) {
            ++it;
        } else {
            LOG("VkDevice doesn't support " + *it);
            it = extensions.erase(it);
        }
    }
    return AE_SUCCESS;
}

AeResult Device::get_queue_family_props(std::vector<VkQueueFamilyProperties>& queue_props) {
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &count, nullptr);
    if (!count) {
        LOG("VkPhysicalDevice doesn't support any QueueFamily.");
        return AE_ERROR_UNKNOWN;
    }

    queue_props.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &count, queue_props.data());
    return AE_SUCCESS;
}

AeResult Device::create_debug_report_callback() {
    if (!device_info_.layer_names_.empty()) {
        VkDebugReportCallbackCreateInfoEXT debug_rep_ci = {};
        debug_rep_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        debug_rep_ci.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
                             VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT |
                             VK_DEBUG_REPORT_DEBUG_BIT_EXT;
        debug_rep_ci.pfnCallback = debug_report_callback;

        auto pfn_vkCreateDebugReportCallbackEXT =
            (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance_, "vkCreateDebugReportCallbackEXT");
        ASSERT_NULL(pfn_vkCreateDebugReportCallbackEXT);
        ASSERT_VK_SUCCESS(pfn_vkCreateDebugReportCallbackEXT(instance_, &debug_rep_ci, nullptr, &debug_report_callback_));
    }
    return AE_SUCCESS;
}

VkBool32 Device::debug_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
                                       size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage,
                                       void* pUserData) {
    LOG("validation layer: " + pMessage);
    return VK_FALSE;
}

AeResult Device::initialize_rendering(const RenderingInfo& render_info, std::shared_ptr<Rendering>& rendering) {
    ASSERT_SUCCESS(GPU_MGR.get<Rendering>(rendering));
    std::shared_ptr<Device> device;
    ASSERT_SUCCESS(GPU_MGR.get<Device>(device));
    ASSERT_SUCCESS(rendering->initialize(device, render_info));
    return AE_SUCCESS;
}

Queues::Queues(const GPUObjectKey& key) : IGPUObject(AE_GPU_QUEUE) {}

AeResult Queues::initialize(const std::shared_ptr<Device>& device, const std::vector<VkQueueFamilyProperties>& queue_family_props) {
    device_ = device;
    ASSERT_SUCCESS(get_device_queues(queue_family_props));
    return AE_SUCCESS;
}

AeResult Queues::get_device_queues(const std::vector<VkQueueFamilyProperties>& queue_family_props) {
    const uint32_t family_count = static_cast<uint32_t>(queue_family_props.size());
    queue_familys_.resize(family_count);

    for (uint32_t i = 0; i < family_count; ++i) {
        const auto queue_count = queue_family_props[i].queueCount;
        queue_familys_[i].family_props_ = queue_family_props[i];
        queue_familys_[i].queues_.resize(queue_count);

        for (uint32_t j = 0; j < queue_count; ++j) {
            VkDeviceQueueInfo2 queue_info2 = {};
            queue_info2.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
            queue_info2.queueFamilyIndex = i;
            queue_info2.queueIndex = j;

            const auto device = device_->get_vk_device();
            ASSERT_VK_NULL_HANDLE(device);
            vkGetDeviceQueue2(device, &queue_info2, &queue_familys_[i].queues_[j]);
        }
    }
    return AE_SUCCESS;
}

Rendering::Rendering(const GPUObjectKey& key) : IGPUObject(AE_GPU_RENDERING), surface_(VK_NULL_HANDLE) {}

AeResult Rendering::initialize(const std::shared_ptr<Device>& device, const RenderingInfo& render_info) {
    device_ = device;
    render_info_ = render_info;
    ASSERT_SUCCESS(create_surface());
    return AE_SUCCESS;
}

#ifdef _WIN32
AeResult Rendering::create_surface() {
    VkWin32SurfaceCreateInfoKHR surface_ci = {};
    surface_ci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_ci.pNext = nullptr;
    surface_ci.flags = 0;
    surface_ci.hinstance = render_info_.wsi_info_.hinstance;
    surface_ci.hwnd = render_info_.wsi_info_.hwnd;
    const auto instance = device_->get_vk_instance();
    ASSERT_VK_NULL_HANDLE(instance);
    ASSERT_VK_SUCCESS(vkCreateWin32SurfaceKHR(instance, &surface_ci, nullptr, &surface_));
    return AE_SUCCESS;
}
#endif  // _WIN32

Manager::Manager() {}

AeResult Manager::initialize() { return AE_SUCCESS; }

AeResult Manager::cleanup() { return AE_SUCCESS; }

AeResult Manager::pre_update() { return AE_SUCCESS; }

AeResult Manager::post_update() { return AE_SUCCESS; }

END_NAMESPACE(object)
END_NAMESPACE(gpu)
END_NAMESPACE(ae)
