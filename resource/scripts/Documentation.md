# 注册脚本文档

现在，`PngPortrait2DDS`允许通过`Python`脚本来自定义肖像的注册工作，同时还允许实现一些额外的效果，当在主界面的**导出选项**中启用**额外效果**时，在脚本中定义的效果将被调用。



## 如何使用

当程序启动时，将会扫描`scripts`目录下的所有`Python`脚本，然后在主界面上方的**脚本**选项卡中显示所有可用的脚本。在默认情况下（即当程序刚刚启动后），将会使用`default.py`。

这个脚本实现了静态肖像的注册工作，在额外效果中，导出包含可以使领袖名称与肖像对应效果的文件，只需在主界面上方的**脚本**选项卡中选中即可使用。

默认脚本同时也包含了一部分示例与代码逻辑的参考，如果你希望自己编写脚本，可以在阅读[如何编写](#如何编写)一节的同时，看看默认脚本中是如何实现的。



## 如何编写

### 文件结构

在默认情况下，脚本文件夹（`scripts`文件夹）下仅包含两个文件，分别为：

+ `default.py`：默认脚本文件
+ `PdxPortraits.py`：相关数据结构的声明，但实际的实现部分在`C++`的源码中完成。

当程序启动时，以上两个文件将会被程序覆盖（如果不存在，则创建），因此，所有对这两个文件的**修改**都将在程序启动时**丢失**。

程序启动后，将会在创建`scripts`目录下创建`log`目录，并新建一个日志文件，用于记录日志消息，详见[调试](#调试)。

需要注意的是，所有与的脚本接口的调用都先于`png`到`dds`图像转换的操作。



### 数据结构

数据结构的声明包含在`PdxPortraits.py`文件中，尽管这个文件并不负责实现，但在你的脚本中将其导入则是必需的。

在`PdxPortraits.py`中，所有开放给`Python`脚本的接口、对象等均有详细的注释说明，如果你想要编写脚本，则应仔细阅读该文件。

~~才不是因为我懒得在Markdown里再写一遍~~



### 调试

由于主程序没有控制台窗口，任何使用`print()`方法的输出都将丢失，因此，应使用`logger`对象来将消息输出到日志。

`scripts`目录下包含一个`log`子目录，日志文件按程序启动时的日期命名：`yyyy-mm-dd.log`，所有日志消息都讲记录到该目录下对应的日志文件中。

`logger`对象定义在`PdxPortraits`模块，其类型的定义如下：

```python
class PythonLogger
```

#### 成员方法

```python
def debug(self, msg: str)
def info(self, msg: str)
def warning(self, msg: str)
def critical(self, msg: str)
def fatal(self, msg: str)
```

这些方法分别对应不同的日志消息等级，但都拥有一个统一的输出格式：

```log
[<当前时间>][<消息等级>][<脚本文件>:<行号>]: <消息内容>
```

#### 错误消息

当一个脚本因为各种错误，在程序启动时读取失败，则会在日志中写入详细的错误信息；而当点击导出后，脚本运行出错时，则会直接弹出错误消息提示。



### API接口

注册脚本总共需要定义三个函数，这三个函数将在用户点击**导出**时，根据导出选项分别被调用。

#### 预处理

```python
def Preprocess(data: list[PortraitData], name: str, need_register: bool, need_effect: bool) -> None
```

参数说明：

+ `data`：包含原始的肖像数据
+ `name`：肖像组的名称，与包含原始`png`文件的文件夹名称相同
+ `need_register`：是否在**导出选项**勾选了**肖像注册**
+ `need_effect`：是否在**导出选项**勾选了**额外效果**

`Preprocess()`函数将在勾选了**肖像注册**或**额外效果**任何一个时，最先被调用，用于对原始肖像数据进行预处理。

#### 肖像注册

```python
def Registration() -> list[str]
```

返回值：

+ `list[str]`：其中的每一项元素为**注册文件**中的**一行**，无需包含换行符

`Registration()`将在勾选了**肖像注册**时被调用，调用时间后于**预处理**，先于**额外效果**，这个函数需要将数据处理为注册文件中的每一行，但并不负责将它们写入文件，写入文件的操作由主程序完成。

实现这个接口时，无需担心复杂的序列化操作，因为最麻烦的部分已经在`C++`源码中实现了。

#### 额外效果

```python
def ExtraEffect(output_path: str) -> None
```

参数说明：

+ `output_path`：程序输出的绝对路径，与包含原始`png`文件的文件夹位于同一目录下

`ExtraEffect()`将在勾选了**额外效果**时被调用，调用时间后于**预处理**和**肖像注册**，这个函数用于实现用户定义的特殊效果。


