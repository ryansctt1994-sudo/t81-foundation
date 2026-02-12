# cmake

CMake support modules and configuration templates for the T81 build system.

## Contents
- `T81Config.cmake.in`: Template for the `T81Config.cmake` package configuration file generated during build/install. This file allows downstream projects to find and use T81 components via `find_package(T81)`.

## Usage
These files are processed by CMake during the configuration phase to generate build artifacts and installation scripts. They are not intended for manual editing unless modifying the build system's export behavior.
