# 群星PNG图像转静态DDS肖像工具

 为群星（Stellaris）游戏制作的静态DDS肖像工具，它能够：

+ 批量将`PNG`图像转换为群星游戏使用的`DDS`（`DXT5`）格式
+ 调整图像尺寸、偏移和缩放
+ 批量完成肖像的注册，并且还允许通过`Python`编写自定义的注册脚本（详见[注册脚本文档](./PngPortrait2DDS/scripts/Documentation.md)）



## 使用的第三方库

### QT

用C++实现高效易用的GUI界面。

### [SOIL2](https://github.com/SpartanJ/SOIL2)

此前曾使用的英伟达的`nvdxt.exe`程序已替换为此库，用于将`png`图像转换为`dxt5`格式的`DDS`图像，允许多线程并行转换大量图像。

### [pybind11](https://github.com/pybind/pybind11)

包装了`CPython API`的轻量级`C++`库，用于实现内嵌`Python`脚本解释器。



## 构建须知

项目使用`CMake`进行构建，在`Windows`上构建时，推荐使用`vcpkg`来安装第三方库：

```powershell
vcpkg install SOIL2:x64-windows pybind11:x64-windows
```

### 注意：

如果你的电脑上已经安装了`Python`，设置了`PYTHONHOME`环境变量，并且`Python`版本与`vcpkg`安装的`Python`依赖库的版本不同，在构建时可能会出现与`Python`相关的错误。

**简易解决方法：**卸载原本的`Python`，或删除`PYTHONHOME`环境变量，这将会让`CMake`优先选择`vcpkg`提供的版本。

程序的运行依赖于`Python`环境，缺失环境将会导致错误，如要分发，推荐使用与构建时所用依赖库相同版本的[Embeddable Package](https://www.python.org/downloads/windows/)。





