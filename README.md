# Summary Writer
### 中文|[English](./README_en.md)

在[GitHub - tensorboard_logger](https://github.com/RustingSword/tensorboard_logger)基础上修改，修改如下：
1. 对头文件中暴露的类名称进行修复`Event, Summary`，避免与其他名称冲突（例如`libtorch`）
2. 用`tensorboard`命名空间包装，将`TensorBoardLogger`包装为`SummaryWriter`，初始化由`tfevent.pb`文件名换成**初始化文件夹**位置（会自动在其中创建`tfevent.pb`文件），包含额外三个函数：
    1. `get_current_timestamp`获取当前时间戳
    2. `get_executable_path`获取当前执行文件路径
    3. `get_root_path`获取当前执行文件的两层父级路径
  
    这三个函数通常用于确定日志的保存位置

3. 修改安装位置，原安装位置直接分布在`/usr/local/[lib,include,cmake]`文件夹下，非常散乱，修改后：
    1. 头文件安装在`/usr/local/lib/tensorboard_logger`
    2. 链接文件和CMake文件在`/usr/local/share/tensorboard_logger/[lib,cmake]`文件夹下

## 安装
依赖Protobuf库：`apt-get install libprotobuf-dev protobuf-compiler` (Ubuntu)

默认安装位置为`/usr/local/`下
```bash
mkdir build
cd build
cmake ..  # -DCMAKE_INSTALL_PREFIX=/path/to/install  修改安装位置
make
sudo make install  # 默认安装到/usr/local/需要su权限
```

## 例子
创建项目包含两个文件
```bash
.
├── CMakeLists.txt
└── main.cpp
```
`CMakeLists.txt`
```cmake
cmake_minimum_required(VERSION 3.16.3)

project(main)
add_executable(main main.cpp)

find_package(tensorboard_logger REQUIRED)  # First Step
target_link_libraries(main PUBLIC ${TENSORBOARD_LOGGER_LIBRARIES} protobuf)  # Second Step
```

`main.cpp`以`add_scalar`为例（更多样例参考[test_summary_writer.cc](./tests/test_summary_writer.cc)）:

```cpp
#include <tensorboard_logger/summary_writer.h>
#include <filesystem>

using tensorboard::SummaryWriter;
using tensorboard::get_current_timestamp;
using tensorboard::get_executable_path;
using tensorboard::get_root_path;
namespace fs = std::filesystem;

// Usually the cmake executable file is in the build/ folder,
// use twice parent to return to the root directory.
fs::path PATH_ROOT = get_executable_path().parent_path().parent_path();
// fs::path PATH_ROOT = get_root_path();  // same as up

void test_add_scalar(SummaryWriter& writer) {
  printf("test add scalar\n");
  double a = (rand() % 10-5), b = rand() % 5, c = (rand() % 10 - 5) * 100;
  for (int i = -10; i < 10; i++) {
    writer.add_scalar("test/scalar", i, a*i*i-b*i+c);
  }
}

int main() {
  srand(static_cast<uint32_t>(time(NULL)));
  std::string timestamp = get_current_timestamp();
  std::cout << "timestamp=" << timestamp << '\n';
  SummaryWriter writer(PATH_ROOT / "tb_logs" / timestamp);  // init writer by directory
  // SummaryWriter writer("../../tb_logs/" + timestamp);  // (same path) path relative to executable file
  test_add_scalar(writer);
  writer.close();  // close writer
  return 0;
}
```
执行`cmake -S . -B build && cmake --build build && ./build/main`运行程序，可以看到建立了`tb_logs/`，使用Python中的tensorboard查看绘制的效果：
```bash
python tensorboard --logdir ./tb_logs
```
