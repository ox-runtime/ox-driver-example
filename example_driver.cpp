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

static void example_update_controller_state(int64_t predicted_time, uint32_t controller_index,
                                            OxControllerState* out_state) {
    // Static controller positions for example
    out_state->is_active = 1;  // Controllers are always active in this example

    // Position controllers at typical locations relative to user
    if (controller_index == OX_CONTROLLER_LEFT) {
        // Left controller: to the left and slightly forward
        out_state->pose.position.x = -0.3f;  // 30cm left
        out_state->pose.position.y = 1.2f;   // Waist height
        out_state->pose.position.z = -0.2f;  // 20cm forward
    } else {
        // Right controller: to the right and slightly forward
        out_state->pose.position.x = 0.3f;   // 30cm right
        out_state->pose.position.y = 1.2f;   // Waist height
        out_state->pose.position.z = -0.2f;  // 20cm forward
    }

    // Identity orientation (no rotation)
    out_state->pose.orientation.x = 0.0f;
    out_state->pose.orientation.y = 0.0f;
    out_state->pose.orientation.z = 0.0f;
    out_state->pose.orientation.w = 1.0f;
}

static uint32_t example_get_interaction_profiles(const char** profiles, uint32_t max_profiles) {
    // We pretend to be Oculus Touch controller
    static const char* supported_profiles[] = {
        "/interaction_profiles/oculus/touch_controller",
    };

    uint32_t count = sizeof(supported_profiles) / sizeof(supported_profiles[0]);

    // Fill in the provided array
    for (uint32_t i = 0; i < count && i < max_profiles; i++) {
        profiles[i] = supported_profiles[i];
    }

    return count;
}

static OxComponentResult example_get_input_component_state(int64_t predicted_time, uint32_t controller_index,
                                                           const char* component_path,
                                                           OxInputComponentState* out_state) {
    if (!component_path || !out_state) {
        return OX_COMPONENT_UNAVAILABLE;
    }

    // Extract the component part from full path (e.g., "/user/hand/left/input/trigger/value" -> "/input/trigger/value")
    const char* input_pos = std::strstr(component_path, "/input/");
    if (!input_pos) {
        // If no /input/ in path, try to use the full path as-is for backwards compatibility
        input_pos = component_path;
    }

    // Parse the component path and return dummy values
    // For testing, we'll return some example states

    // Trigger
    if (std::strcmp(input_pos, "/input/trigger/value") == 0) {
        out_state->float_value = 0.5f;  // Half-pressed
        return OX_COMPONENT_AVAILABLE;
    }

    // Squeeze/Grip
    if (std::strcmp(input_pos, "/input/squeeze/value") == 0) {
        out_state->float_value = 0.3f;
        return OX_COMPONENT_AVAILABLE;
    }

    // Thumbstick
    if (std::strcmp(input_pos, "/input/thumbstick") == 0) {
        out_state->x = 0.0f;
        out_state->y = 0.0f;
        return OX_COMPONENT_AVAILABLE;
    }

    if (std::strcmp(input_pos, "/input/thumbstick/x") == 0) {
        out_state->float_value = 0.0f;
        return OX_COMPONENT_AVAILABLE;
    }

    if (std::strcmp(input_pos, "/input/thumbstick/y") == 0) {
        out_state->float_value = 0.0f;
        return OX_COMPONENT_AVAILABLE;
    }

    // Button A (right hand) / X (left hand) - Click
    if (std::strcmp(input_pos, "/input/a/click") == 0 || std::strcmp(input_pos, "/input/x/click") == 0) {
        out_state->boolean_value = 0;  // Not pressed
        return OX_COMPONENT_AVAILABLE;
    }

    // Button B (right hand) / Y (left hand) - Click
    if (std::strcmp(input_pos, "/input/b/click") == 0 || std::strcmp(input_pos, "/input/y/click") == 0) {
        out_state->boolean_value = 1;  // Pressed (for testing)
        return OX_COMPONENT_AVAILABLE;
    }

    // Button A/X Touch
    if (std::strcmp(input_pos, "/input/a/touch") == 0 || std::strcmp(input_pos, "/input/x/touch") == 0) {
        out_state->boolean_value = 0;
        return OX_COMPONENT_AVAILABLE;
    }

    // Button B/Y Touch
    if (std::strcmp(input_pos, "/input/b/touch") == 0 || std::strcmp(input_pos, "/input/y/touch") == 0) {
        out_state->boolean_value = 1;  // Touched (for testing)
        return OX_COMPONENT_AVAILABLE;
    }

    // Component not supported
    return OX_COMPONENT_UNAVAILABLE;
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
    callbacks->update_controller_state = example_update_controller_state;
    callbacks->get_input_component_state = example_get_input_component_state;
    callbacks->get_interaction_profiles = example_get_interaction_profiles;

    return 1;
}
