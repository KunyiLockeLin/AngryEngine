#include <windows.h>
#include "gpu.h"

void draw_a_triangle() {
    ae::gpu::DeviceCreateInfo device_ci{};
    ae::gpu::initialize_device(device_ci);

    ae::gpu::pre_update();
    ae::gpu::post_update();
    ae::gpu::cleanup();
}

enum test_list {
    test_draw_a_triangle,
};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    try {
        switch (test_draw_a_triangle) {
            case test_draw_a_triangle:
                draw_a_triangle();
                break;
            default:
                break;
        }
    } catch (const std::runtime_error &e) {
        LOG(e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
