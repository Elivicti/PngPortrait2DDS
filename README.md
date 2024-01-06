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

> IDE：VS2022
>
> 标准：C++20

推荐使用`vcpkg`来安装第三方库：

```powershell
vcpkg install SOIL2:x64-windows
```

```pow	
vcpkg install pybind11:x64-windows
```

注意：如果使用`vcpkg`来安装这些库，你也许需要在项目中指定`Python`的库目录和包含目录。

在默认情况下，`vcpkg`将会把`Python`的包含目录放入`$(VcpkgRoot)\installed\x64-windows\include\python<版本号>`中，在引用这些头文件时需要使用多指定一级目录，而`pybind11`是直接引用的`Python`库的头文件，因此在项目中添加`Python`的包含目录能够解决找不到`Python.h`的编译错误。











