# ox-driver-example

An example implementation of an [ox runtime](https://github.com/cmdr2/ox) OpenXR headset driver. You can use it as a starting template for writing a driver for your headset.

## Purpose

This driver provides a simple fixed-pose headset that:
- Always reports as "connected"
- Returns a fixed standing pose at (0, 1.6, 0) meters
- Provides standard VR headset display properties (1920x1080 per eye, 90Hz)
- Demonstrates the minimal driver API implementation

## Building

1. Clone this repository using `git clone https://github.com/cmdr2/ox-driver-example`
2. Download [ox_driver.h](https://github.com/cmdr2/ox/releases/latest/download/driver.h) and place it inside this folder.
3. Compile using `cmake`:

```bash
cmake -B build
cmake --build build --config Release
```

4. Your driver will be built inside `build/example_driver`.

## Installation

Copy the driver (i.e. the `build/example_driver` folder) to the ox runtime's `drivers` folder:

For example:

```
ox-runtime/
├── bin/
│   ├── ox-service.exe (or ox-service on Linux)
│   └── drivers/
│       └── example_driver/
│           └── driver.dll (or libdriver.so)
```
