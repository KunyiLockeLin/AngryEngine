#include "gpu.h"
#include "gpu_objects.h"
#include<assert.h>

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)

/*
Interface table:
IDevice     <-> object::Device
IRendering  <-> object::Rendering
*/
template <typename T, typename K>
T* convert_interface(K* object ) {
    return reinterpret_cast<T*>(object);
}

AeResult initialize_device(const DeviceInfo& device_info, IDevice* &device) {
    ASSERT_SUCCESS(object::GPU_MGR.initialize());

    std::shared_ptr<object::Device> obj_device;
    ASSERT_SUCCESS(object::GPU_MGR.get<object::Device>(obj_device));
    ASSERT_SUCCESS(obj_device->initialize(device_info));
    device = convert_interface<IDevice>(obj_device.get());
    return AE_SUCCESS;
}

AeResult cleanup() {
    ASSERT_SUCCESS(object::GPU_MGR.cleanup());
    return AE_SUCCESS;
}

AeResult pre_update() {
    ASSERT_SUCCESS(object::GPU_MGR.pre_update());
    return AE_SUCCESS;
}

AeResult post_update() {
    ASSERT_SUCCESS(object::GPU_MGR.post_update());
    return AE_SUCCESS;
}

AeResult IDevice::initialize_rendering(const RenderingInfo& rednering_info, IRendering* &rendering) {
    auto* obj_device = convert_interface<object::Device>(this);
    std::shared_ptr<object::Rendering> obj_rendering;
    obj_device->initialize_rendering(rednering_info, obj_rendering);
    rendering = convert_interface<IRendering>(obj_rendering.get());
    return AE_SUCCESS;
}

END_NAMESPACE(gpu)
END_NAMESPACE(ae)
