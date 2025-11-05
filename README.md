# 群星PNG图像转静态DDS肖像工具

为群星（Stellaris）游戏制作的静态DDS肖像工具，它能够：

+ 批量将`PNG`图像转换为群星游戏使用的`DDS`格式
+ 调整图像尺寸、偏移和缩放


## 构建须知

本仓库中提供`CMakePresets.json`，包含了构建需要的基本配置。生成预设包含Windows平台和Linux平台。

```bash
cmake --preset <生成预设>
```

### Windows

使用`MSVC`编译器，使用`vcpkg`作为包管理器。

`CMAKE_TOOLCHAIN_FILE`被设置为`$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake`，在构建前需要保证环境变量`VCPKG_ROOT`已正确配置。

+ `x64-win-debug`：Debug构建
+ `x64-win-release`：Release构建

### Linux

使用`GNU GCC`编译器。

+ `x64-linux-debug`：Debug构建
+ `x64-linux-release`：Release构建

### 第三方库

- [Qt6](https://www.qt.io)
- [SOIL2](https://github.com/SpartanJ/SOIL2)

除Qt外的第三方库依赖会优先使用`find_package`寻找，若未找到，则会从对应的仓库拉取源码进行构建。拉取操作默认使用Git SSH，若要使用Https，请手动设置`GIT_URL`变量。

```bash
cmake ... -DGIT_URL="https://github.com/" # 注意，最后的斜杠是必需的
```
