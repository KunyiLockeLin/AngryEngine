#include "gpu.h"

const char CONFIG_PATH[] = "data\\config.xml";
#define CONFIG COM_MGR.getXML(CONFIG_PATH)

void draw_triangle() {
    ae::gpu::DeviceInfo device_info{};
    device_info.application_name_ = "test GPU";
    device_info.application_version_ = "0.2.0";
    device_info.engine_name_ = "Angry Engine";
    device_info.engine_version_ = "0.2.0";
    device_info.api_version_ = "1.2.170";
    device_info.layer_names_.push_back("VK_LAYER_KHRONOS_validation");

    ae::gpu::IDevice* device = nullptr;
    ae::gpu::initialize_device(device_info, device);

    ae::gpu::RenderingInfo rendering_info{};

    ae::gpu::IRendering* rendering = nullptr;
    device->initialize_rendering(rendering_info, rendering);

    ae::gpu::pre_update();
    ae::gpu::post_update();
    ae::gpu::cleanup();
}

enum test_list {
    test_draw_triangle,
};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    try {
        LOGOBJ.setOutput(*CONFIG, "TestGPU_");
        switch (test_draw_triangle) {
            case test_draw_triangle:
                draw_triangle();
                break;
            default:
                break;
        }
    } catch (const std::runtime_error &e) {
        STACK(e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
