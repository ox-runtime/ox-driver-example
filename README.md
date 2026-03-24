# ox-driver-example

**WORK-IN-PROGRESS** - This is still a prototype and is not (yet) fully compliant with the OpenXR spec.

An example implementation of an [ox runtime](https://github.com/ox-runtime/ox) OpenXR headset driver. You can use it as a starting template for writing a driver for your headset.

## Purpose

This driver provides a simple fixed-pose headset that:
- Always reports as "connected"
- Returns a fixed standing pose at (0, 1.6, 0) meters
- Provides standard VR headset display properties (1920x1080 per eye, 90Hz)
- Demonstrates the minimal driver API implementation

## Building

```bash
cmake -B build
cmake --build build --config Release
```

If you want to build against a local `ox-runtime` checkout instead of the remote repository, pass `-DOX_RUNTIME_REPO=/path/to/ox-runtime` when configuring cmake.

3. Your driver will be built inside `build/example_driver`.

## Installation

Copy the driver (i.e. the `build/example_driver` folder) to the ox runtime's `drivers` folder:

For example:

```
ox-runtime/
├── bin/
│   ├── ox-service.exe (or ox-service on Linux/Mac)
│   └── drivers/
│       └── example_driver/
│           └── ox_driver.dll (or libox_driver.so / libox_driver.dylib)
```
