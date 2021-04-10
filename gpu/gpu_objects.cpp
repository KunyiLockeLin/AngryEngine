#include "gpu_objects.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)
BEGIN_NAMESPACE(object)

Device::Device() : IGPUObject(GPU_OBJECT_DEVICE) {}
Model::Model(const GPUObjectKey& key) : IGPUObject(GPU_OBJECT_MODEL) {}
Manager::Manager() {}

END_NAMESPACE(object)
END_NAMESPACE(gpu)
END_NAMESPACE(ae)
