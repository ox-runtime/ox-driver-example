// Simple (fixed-pose) example headset driver implementation

#include <cstring>

// Include the driver API
#include "ox_driver.h"

static int example_initialize(void) {
    // No hardware to initialize for example
    return 1;
}

static void example_shutdown(void) {
    // Nothing to clean up
}

static int example_is_device_connected(void) {
    // example is always "connected"
    return 1;
}

static void example_get_device_info(OxDeviceInfo* info) {
    std::strncpy(info->name, "example VR Headset", sizeof(info->name) - 1);
    info->name[sizeof(info->name) - 1] = '\0';

    std::strncpy(info->manufacturer, "ox runtime", sizeof(info->manufacturer) - 1);
    info->manufacturer[sizeof(info->manufacturer) - 1] = '\0';

    std::strncpy(info->serial, "example-00000", sizeof(info->serial) - 1);
    info->serial[sizeof(info->serial) - 1] = '\0';

    info->vendor_id = 0x0000;
    info->product_id = 0x0001;
}

static void example_get_display_properties(OxDisplayProperties* props) {
    // Standard VR headset specs
    props->display_width = 1920;
    props->display_height = 1080;
    props->recommended_width = 1024;
    props->recommended_height = 1024;
    props->refresh_rate = 90.0f;

    // ~90 degree FOV
    props->fov.angle_left = -0.785398f;  // -45 degrees in radians
    props->fov.angle_right = 0.785398f;  // +45 degrees
    props->fov.angle_up = 0.785398f;
    props->fov.angle_down = -0.785398f;
}

static void example_get_tracking_capabilities(OxTrackingCapabilities* caps) {
    caps->has_orientation_tracking = 1;
    caps->has_position_tracking = 1;
}

static void example_update_pose(int64_t predicted_time, OxPose* out_pose) {
    // Fixed pose: standing at origin, looking forward
    out_pose->position.x = 0.0f;
    out_pose->position.y = 1.6f;  // ~1.6m eye height
    out_pose->position.z = 0.0f;

    // Identity quaternion (no rotation)
    out_pose->orientation.x = 0.0f;
    out_pose->orientation.y = 0.0f;
    out_pose->orientation.z = 0.0f;
    out_pose->orientation.w = 1.0f;
}

static void example_update_view_pose(int64_t predicted_time, uint32_t eye_index, OxPose* out_pose) {
    // Start with HMD pose
    example_update_pose(predicted_time, out_pose);

    // Apply IPD offset (64mm standard)
    const float ipd_half = 0.032f;  // 32mm per eye

    if (eye_index == 0) {
        // Left eye
        out_pose->position.x -= ipd_half;
    } else {
        // Right eye
        out_pose->position.x += ipd_half;
    }
}

// Driver registration function - this is the entry point called by the runtime
extern "C" OX_DRIVER_EXPORT int ox_driver_register(OxDriverCallbacks* callbacks) {
    if (!callbacks) {
        return 0;
    }

    // Fill in all callback functions
    callbacks->initialize = example_initialize;
    callbacks->shutdown = example_shutdown;
    callbacks->is_device_connected = example_is_device_connected;
    callbacks->get_device_info = example_get_device_info;
    callbacks->get_display_properties = example_get_display_properties;
    callbacks->get_tracking_capabilities = example_get_tracking_capabilities;
    callbacks->update_pose = example_update_pose;
    callbacks->update_view_pose = example_update_view_pose;

    return 1;
}
