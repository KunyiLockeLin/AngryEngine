#include "gpu.h"
#include "gpu_objects.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)

AeResult initialize_device(const DeviceCreateInfo& device_ci) {
    auto result = object::MGR.initialize();
    return AE_SUCCESS;
}

AeResult cleanup() {
    auto result = object::MGR.cleanup();
    return AE_SUCCESS;
}

AeResult pre_update() {
    auto result = object::MGR.pre_update();
    return AE_SUCCESS;
}

AeResult post_update() {
    auto result = object::MGR.post_update();
    return AE_SUCCESS;
}

END_NAMESPACE(gpu)
END_NAMESPACE(ae)
