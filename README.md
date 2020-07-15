# Jetz

A simple game.

# Requirements

* Vulkan SDK
    * Install as administrator.
    * Verify environment variables `VULKAN_SDK` and `VK_SDK_PATH` are both set to the Vulkan SDK path.
* Python 3
* Visual Studio 2019

# Building

* Clone repo.
* Open solution with Visual Studio.
* Add Vulkan `include` and `lib` paths to user properties (see notes for example).
* Set debug working directory to the output directory `$(OutDir)` (see notes).
* Build.
* Run.

# Notes

Notes for future reference.

## Example user properties

* Open Property Manager: View -> Other Windows -> Property Manager
* Edit 32-bit properties: `Microsoft.Cpp.Win32.user`
    * User macro: 
        * Name: `VULKAN_SDK_DIR`
        * Value: `path/to/VulkanSDK/1.x.x.x/`
    * VC++ Directories:
        * Add `$(VULKAN_SDK_DIR)include` to include directories.
        * Add `$(VULKAN_SDK_DIR)lib32` to library directories.
* Edit 64-bit properties: `Microsoft.Cpp.x64.user`
    * User macro: 
        * Name: `VULKAN_SDK_DIR`
        * Value: `path/to/VulkanSDK/1.x.x.x/`
    * VC++ Directories:
        * Add `$(VULKAN_SDK_DIR)include` to include directories.
        * Add `$(VULKAN_SDK_DIR)lib` to library directories.

## Setting debug working directory

* Open properties for the executable project.
* In the `Debugging` section set the working directory to `$(OutDir)`.
* This should be where the executable is output to for the current build configuration, for example `Game\Bin\x86-Debug`.

## Vulkan validation layers

On Windows, [validation layers](https://vulkan.lunarg.com/doc/sdk/1.0.39.0/windows/layers.html
) are defined in the Windows registry:

```
HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\Vulkan\ExplicitLayers
HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\Vulkan\ImplicitLayers
```

Also worth noting the 32-bit registry:
```
HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Khronos\Vulkan\ExplicitLayers
HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Khronos\Vulkan\ImplicitLayers
```

Implicit layers are created by third parties. Explicit layers are created by the Vulkan SDK installer.

After upgrading the SDK or installing a second SDK version, ensure there are only layers defined for the SDK version you are using. You may have to manually remove old explicit layer entries.

Windows has two different registries - one for 64-bit and one for 32-bit. So make sure to check the `Wow6432Node` variants as well. I had a case where the 64-bit build was working find with validation layers but the 32-bit build was not. The 32-bit registry hadn't been updated.

You don't need to have the `VK_LAYER_PATH` environment variable defined. Vulkan will use the `ExplicitLayers` and `ImplicitLayers` registry keys to location validation layers.

You must restart Visual Studio after changing environment variables.

## Shader compilation

Shader compilation to SPIR-V is handled by a project in the Visual Studio solution and should happen automatically as needed.

To add a shader to the Shaders project:

* Add file to project.
* Set build action as `Custom Build Tool`.

The custom build tool is defined in the project's properties, so settings will apply to all files that are marked `Custom Build Tool`.

The `Outputs` field in the custom build tool properties for the project should be the full path to the output file location. There is also an `%(Outputs)` variable there, not sure what that does. This is what is used to tell if the project is up to date and needs to be rebuilt or not.

## Google Test

Using Visual Studio's Google Test project (nuget package).