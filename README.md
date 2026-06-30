# ox-driver-example

An example XR headset driver for the [ox](https://github.com/ox-runtime/ox) OpenXR runtime. You can use it as a starting template for [writing a driver](https://github.com/ox-runtime/ox/blob/main/docs/driver.md) for your headset.

## Features

This driver provides a simple fixed-pose headset that:
- Always reports as "connected"
- Returns a fixed standing pose at (0, 1.6, 0) meters
- Provides standard VR headset display properties (1920x1080 per eye, 90Hz)
- Receives pixel data from the runtime
- Demonstrates the minimal driver API implementation

## Building

```bash
cmake -B build
cmake --build build --config Release
```

Your driver will be built inside `build/example_driver`.

A driver only requires `ox_driver.h` (for the driver API) and `openxr.h` (for the data types). These headers are fetched automatically by CMake.

### Advanced Usage
If you want to build against a local [ox-runtime](https://github.com/ox-runtime/ox-runtime) checkout instead of the remote repository (that's fetched automatically), pass `-DOX_RUNTIME_REPO=/path/to/ox-runtime` when configuring cmake.

## Installation

Copy the driver (i.e. the `build/example_driver` folder) to the ox runtime's `drivers` folder:

For example:

```
ox-runtime/
├── bin/
│   ├── ox.exe (or ox on Linux/Mac)
│   └── drivers/
│       └── example_driver/
│           └── ox_driver.dll (or libox_driver.so / libox_driver.dylib)
```
