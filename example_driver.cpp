// Simple (fixed-pose) example headset driver implementation

#include <cstdio>
#include <cstring>

// Include the driver API
#include <ox_driver.h>

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
    snprintf(info->name, sizeof(info->name), "%s", "example VR Headset");

    snprintf(info->manufacturer, sizeof(info->manufacturer), "%s", "ox runtime");

    snprintf(info->serial, sizeof(info->serial), "%s", "example-00000");

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
    props->fov.angleLeft = -0.785398f;  // -45 degrees in radians
    props->fov.angleRight = 0.785398f;  // +45 degrees
    props->fov.angleUp = 0.785398f;
    props->fov.angleDown = -0.785398f;
}

static void example_get_tracking_capabilities(OxTrackingCapabilities* caps) {
    caps->has_orientation_tracking = 1;
    caps->has_position_tracking = 1;
}

static void example_update_view_pose(XrTime predicted_time, uint32_t eye_index, XrPosef* out_pose) {
    // Get HMD pose from device[0] (the approach drivers should use)
    // For this simple example, we'll just use a fixed pose
    out_pose->position.x = 0.0f;
    out_pose->position.y = 1.6f;  // ~1.6m eye height
    out_pose->position.z = 0.0f;
    out_pose->orientation.x = 0.0f;
    out_pose->orientation.y = 0.0f;
    out_pose->orientation.z = 0.0f;
    out_pose->orientation.w = 1.0f;

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

static void example_update_devices(XrTime predicted_time, OxDeviceState* out_states, uint32_t* out_count) {
    // Report HMD + two controllers
    *out_count = 3;

    // HMD as device[0] - /user/head
    snprintf(out_states[0].user_path, sizeof(out_states[0].user_path), "%s", "/user/head");
    out_states[0].is_active = 1;  // HMD is always active
    out_states[0].pose.position.x = 0.0f;
    out_states[0].pose.position.y = 1.6f;  // Eye height
    out_states[0].pose.position.z = 0.0f;
    out_states[0].pose.orientation.x = 0.0f;
    out_states[0].pose.orientation.y = 0.0f;
    out_states[0].pose.orientation.z = 0.0f;
    out_states[0].pose.orientation.w = 1.0f;

    // Left controller as device[1]
    snprintf(out_states[1].user_path, sizeof(out_states[1].user_path), "%s", "/user/hand/left");
    out_states[1].is_active = 1;
    out_states[1].pose.position.x = -0.3f;  // 30cm left
    out_states[1].pose.position.y = 1.2f;   // Waist height
    out_states[1].pose.position.z = -0.2f;  // 20cm forward
    out_states[1].pose.orientation.x = 0.0f;
    out_states[1].pose.orientation.y = 0.0f;
    out_states[1].pose.orientation.z = 0.0f;
    out_states[1].pose.orientation.w = 1.0f;

    // Right controller as device[2]
    snprintf(out_states[2].user_path, sizeof(out_states[2].user_path), "%s", "/user/hand/right");
    out_states[2].is_active = 1;
    out_states[2].pose.position.x = 0.3f;   // 30cm right
    out_states[2].pose.position.y = 1.2f;   // Waist height
    out_states[2].pose.position.z = -0.2f;  // 20cm forward
    out_states[2].pose.orientation.x = 0.0f;
    out_states[2].pose.orientation.y = 0.0f;
    out_states[2].pose.orientation.z = 0.0f;
    out_states[2].pose.orientation.w = 1.0f;
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

static OxComponentResult example_get_input_state_boolean(XrTime predicted_time, const char* user_path,
                                                         const char* component_path, uint32_t* out_value) {
    if (!user_path || !component_path || !out_value) {
        return OX_COMPONENT_UNAVAILABLE;
    }

    // Button A (right hand) / X (left hand) - Click
    if (std::strcmp(component_path, "/input/a/click") == 0 || std::strcmp(component_path, "/input/x/click") == 0) {
        *out_value = 0;  // Not pressed
        return OX_COMPONENT_AVAILABLE;
    }

    // Button B (right hand) / Y (left hand) - Click
    if (std::strcmp(component_path, "/input/b/click") == 0 || std::strcmp(component_path, "/input/y/click") == 0) {
        *out_value = 1;  // Pressed (for testing)
        return OX_COMPONENT_AVAILABLE;
    }

    // Button A/X Touch
    if (std::strcmp(component_path, "/input/a/touch") == 0 || std::strcmp(component_path, "/input/x/touch") == 0) {
        *out_value = 0;
        return OX_COMPONENT_AVAILABLE;
    }

    // Button B/Y Touch
    if (std::strcmp(component_path, "/input/b/touch") == 0 || std::strcmp(component_path, "/input/y/touch") == 0) {
        *out_value = 1;  // Touched (for testing)
        return OX_COMPONENT_AVAILABLE;
    }

    // Trigger click
    if (std::strcmp(component_path, "/input/trigger/click") == 0) {
        *out_value = 0;
        return OX_COMPONENT_AVAILABLE;
    }

    // Trigger touch
    if (std::strcmp(component_path, "/input/trigger/touch") == 0) {
        *out_value = 0;
        return OX_COMPONENT_AVAILABLE;
    }

    // Thumbstick click
    if (std::strcmp(component_path, "/input/thumbstick/click") == 0) {
        *out_value = 0;
        return OX_COMPONENT_AVAILABLE;
    }

    // Thumbstick touch
    if (std::strcmp(component_path, "/input/thumbstick/touch") == 0) {
        *out_value = 0;
        return OX_COMPONENT_AVAILABLE;
    }

    return OX_COMPONENT_UNAVAILABLE;
}

static OxComponentResult example_get_input_state_float(XrTime predicted_time, const char* user_path,
                                                       const char* component_path, float* out_value) {
    if (!user_path || !component_path || !out_value) {
        return OX_COMPONENT_UNAVAILABLE;
    }

    // Trigger value
    if (std::strcmp(component_path, "/input/trigger/value") == 0) {
        *out_value = 0.5f;  // Half-pressed
        return OX_COMPONENT_AVAILABLE;
    }

    // Squeeze/Grip value
    if (std::strcmp(component_path, "/input/squeeze/value") == 0) {
        *out_value = 0.4f;
        return OX_COMPONENT_AVAILABLE;
    }

    // Thumbstick x component
    if (std::strcmp(component_path, "/input/thumbstick/x") == 0) {
        *out_value = 0.0f;
        return OX_COMPONENT_AVAILABLE;
    }

    // Thumbstick y component
    if (std::strcmp(component_path, "/input/thumbstick/y") == 0) {
        *out_value = 0.0f;
        return OX_COMPONENT_AVAILABLE;
    }

    return OX_COMPONENT_UNAVAILABLE;
}

static OxComponentResult example_get_input_state_vector2f(XrTime predicted_time, const char* user_path,
                                                          const char* component_path, XrVector2f* out_value) {
    if (!user_path || !component_path || !out_value) {
        return OX_COMPONENT_UNAVAILABLE;
    }

    // Thumbstick
    if (std::strcmp(component_path, "/input/thumbstick") == 0) {
        out_value->x = 0.0f;
        out_value->y = 0.0f;
        return OX_COMPONENT_AVAILABLE;
    }

    // Trackpad
    if (std::strcmp(component_path, "/input/trackpad") == 0) {
        out_value->x = 0.0f;
        out_value->y = 0.0f;
        return OX_COMPONENT_AVAILABLE;
    }

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
    callbacks->update_view_pose = example_update_view_pose;
    callbacks->update_devices = example_update_devices;
    callbacks->get_input_state_boolean = example_get_input_state_boolean;
    callbacks->get_input_state_float = example_get_input_state_float;
    callbacks->get_input_state_vector2f = example_get_input_state_vector2f;
    callbacks->get_interaction_profiles = example_get_interaction_profiles;

    return 1;
}
