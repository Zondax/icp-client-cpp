# Integration with Unreal Engine

Projects with Unreal Engine and IC blockchain are now possible, using our library.

Unreal Engine projects are mostly written in C++ and integrating our library to have an interface with an IC canister only requires a few steps:
- Clone the library to your project;
- On Project.Build.cs add the static libs and include paths:

```cpp
PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../../icp-client-cpp/lib-agent-c/inc"));
PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../../icp-client-cpp/lib-agent-cpp/inc"));
PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../../icp-client-cpp/ic-agent-wrapper/target/release", "libic_agent_wrapper.a"));
PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../../icp-client-cpp/build", "libagent_c.a"));
PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../../icp-client-cpp/build", "libagent_cpp.a"));
```
