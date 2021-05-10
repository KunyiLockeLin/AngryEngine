#ifndef AE_GPU_GPU_H
#define AE_GPU_GPU_H

#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include "common/common.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)

// WSI: window system integration
#ifdef _WIN32
struct WSIInfo {
    HINSTANCE hinstance = NULL;
    HWND hwnd = NULL;
};
#endif  // _WIN32

struct RenderingInfo {
    WSIInfo wsi_info_;
};

class IRendering {
   public:
};


struct DeviceInfo {
    std::string application_name_;
    std::string application_version_;
    std::string engine_name_;
    std::string engine_version_;
    std::string api_version_;
    std::vector<const char*> layer_names_;
};

class IDevice {
   public:
    // Only support single Rednering. If it supports plural, it will become create_rendering.
    AeResult initialize_rendering(const RenderingInfo& rednering_info, IRendering*& rendering);
};

// Only support single GPU and single device. If it supports plural, it will become create_device.
AeResult initialize_device(const DeviceInfo& device_info, IDevice* &device);
AeResult cleanup();
AeResult pre_update();
AeResult post_update();

struct DrawPointData {};
struct DrawLineData {};
struct DrawTriangleData {};
struct DrawModelData {};
struct TextureData {};
struct ShaderBindingSlot {};
struct RenderPass {};

END_NAMESPACE(gpu)
END_NAMESPACE(ae)

#endif  // AE_GPU_GPU_H
