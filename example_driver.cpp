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

static void example_get_system_properties(XrSystemProperties* props) {
    std::snprintf(props->systemName, XR_MAX_SYSTEM_NAME_SIZE, "%s", "example VR Headset");
    props->vendorId = 0x0000;
    props->graphicsProperties.maxSwapchainImageWidth = 1920;
    props->graphicsProperties.maxSwapchainImageHeight = 1080;
    props->trackingProperties.orientationTracking = XR_TRUE;
    props->trackingProperties.positionTracking = XR_TRUE;
}

static void example_update_view(XrTime predicted_time, uint32_t eye_index, XrView* out_view) {
    // Get HMD pose from device[0] (the approach drivers should use)
    // For this simple example, we'll just use a fixed pose
    out_view->pose.position.x = 0.0f;
    out_view->pose.position.y = 1.6f;  // ~1.6m eye height
    out_view->pose.position.z = 0.0f;
    out_view->pose.orientation.x = 0.0f;
    out_view->pose.orientation.y = 0.0f;
    out_view->pose.orientation.z = 0.0f;
    out_view->pose.orientation.w = 1.0f;

    // Apply IPD offset (64mm standard)
    const float ipd_half = 0.032f;  // 32mm per eye

    if (eye_index == 0) {
        // Left eye
        out_view->pose.position.x -= ipd_half;
    } else {
        // Right eye
        out_view->pose.position.x += ipd_half;
    }

    // Standard ~90° symmetric FOV
    out_view->fov = {-0.785398f, 0.785398f, 0.785398f, -0.785398f};
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

static XrResult example_get_input_state_boolean(XrTime predicted_time, const char* user_path,
                                                const char* component_path, XrBool32* out_value) {
    if (!user_path || !component_path || !out_value) {
        return XR_ERROR_PATH_UNSUPPORTED;
    }

    // Button A (right hand) / X (left hand) - Click
    if (std::strcmp(component_path, "/input/a/click") == 0 || std::strcmp(component_path, "/input/x/click") == 0) {
        *out_value = XR_FALSE;
        return XR_SUCCESS;
    }

    // Button B (right hand) / Y (left hand) - Click
    if (std::strcmp(component_path, "/input/b/click") == 0 || std::strcmp(component_path, "/input/y/click") == 0) {
        *out_value = XR_TRUE;  // Pressed (for testing)
        return XR_SUCCESS;
    }

    // Button A/X Touch
    if (std::strcmp(component_path, "/input/a/touch") == 0 || std::strcmp(component_path, "/input/x/touch") == 0) {
        *out_value = XR_FALSE;
        return XR_SUCCESS;
    }

    // Button B/Y Touch
    if (std::strcmp(component_path, "/input/b/touch") == 0 || std::strcmp(component_path, "/input/y/touch") == 0) {
        *out_value = XR_TRUE;  // Touched (for testing)
        return XR_SUCCESS;
    }

    // Trigger click
    if (std::strcmp(component_path, "/input/trigger/click") == 0) {
        *out_value = XR_FALSE;
        return XR_SUCCESS;
    }

    // Trigger touch
    if (std::strcmp(component_path, "/input/trigger/touch") == 0) {
        *out_value = XR_FALSE;
        return XR_SUCCESS;
    }

    // Thumbstick click
    if (std::strcmp(component_path, "/input/thumbstick/click") == 0) {
        *out_value = XR_FALSE;
        return XR_SUCCESS;
    }

    // Thumbstick touch
    if (std::strcmp(component_path, "/input/thumbstick/touch") == 0) {
        *out_value = XR_FALSE;
        return XR_SUCCESS;
    }

    return XR_ERROR_PATH_UNSUPPORTED;
}

static XrResult example_get_input_state_float(XrTime predicted_time, const char* user_path, const char* component_path,
                                              float* out_value) {
    if (!user_path || !component_path || !out_value) {
        return XR_ERROR_PATH_UNSUPPORTED;
    }

    // Trigger value
    if (std::strcmp(component_path, "/input/trigger/value") == 0) {
        *out_value = 0.5f;  // Half-pressed
        return XR_SUCCESS;
    }

    // Squeeze/Grip value
    if (std::strcmp(component_path, "/input/squeeze/value") == 0) {
        *out_value = 0.4f;
        return XR_SUCCESS;
    }

    // Thumbstick x component
    if (std::strcmp(component_path, "/input/thumbstick/x") == 0) {
        *out_value = 0.0f;
        return XR_SUCCESS;
    }

    // Thumbstick y component
    if (std::strcmp(component_path, "/input/thumbstick/y") == 0) {
        *out_value = 0.0f;
        return XR_SUCCESS;
    }

    return XR_ERROR_PATH_UNSUPPORTED;
}

static XrResult example_get_input_state_vector2f(XrTime predicted_time, const char* user_path,
                                                 const char* component_path, XrVector2f* out_value) {
    if (!user_path || !component_path || !out_value) {
        return XR_ERROR_PATH_UNSUPPORTED;
    }

    // Thumbstick
    if (std::strcmp(component_path, "/input/thumbstick") == 0) {
        out_value->x = 0.0f;
        out_value->y = 0.0f;
        return XR_SUCCESS;
    }

    // Trackpad
    if (std::strcmp(component_path, "/input/trackpad") == 0) {
        out_value->x = 0.0f;
        out_value->y = 0.0f;
        return XR_SUCCESS;
    }

    return XR_ERROR_PATH_UNSUPPORTED;
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
    callbacks->get_system_properties = example_get_system_properties;
    callbacks->update_view = example_update_view;
    callbacks->update_devices = example_update_devices;
    callbacks->get_input_state_boolean = example_get_input_state_boolean;
    callbacks->get_input_state_float = example_get_input_state_float;
    callbacks->get_input_state_vector2f = example_get_input_state_vector2f;
    callbacks->get_interaction_profiles = example_get_interaction_profiles;

    return 1;
}
