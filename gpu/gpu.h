#ifndef AE_GPU_GPU_H
#define AE_GPU_GPU_H

#include "code_generator/generated_config_struct_enum.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)


struct DeviceCreateInfo {};

// Only support single GPU and single device. 
// If it supports plural, it will become create_device and create a device id,
// and then the other functions requires device id.
AeResult initialize_device(const DeviceCreateInfo& device_ci);
AeResult cleanup();
AeResult pre_update();
AeResult post_update();

using MODEL_ID = ID;

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
