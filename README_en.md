# Summary Writer
### [中文](./README.md) | English

Modified based on [GitHub - tensorboard_logger](https://github.com/RustingSword/tensorboard_logger) with the following changes:
1. Fixed class names exposed in the header files, such as `Event` and `Summary`, to avoid conflicts with other names (e.g., `libtorch`).
2. Wrapped in the `tensorboard` namespace, and wrapped `TensorBoardLogger` into `SummaryWriter`. The initialization now uses the **folder location** instead of the `tfevent.pb` filename (which will automatically create the `tfevent.pb` file within). It includes three additional functions:
    1. `get_current_timestamp`: Gets the current timestamp.
    2. `get_executable_path`: Gets the path of the current executable.
    3. `get_root_path`: Gets the path of the two parent directories of the current executable.

   These three functions are typically used to determine the location of log storage.

3. Changed the installation location. The original installation was scattered across `/usr/local/[lib, include, cmake]`. After modification:
    1. Header files are installed in `/usr/local/lib/tensorboard_logger`.
    2. Link files and CMake files are installed in `/usr/local/share/tensorboard_logger/[lib, cmake]`.

## Installation
Only dependency Protobuf library: `apt-get install libprotobuf-dev protobuf-compiler` (Ubuntu)

The default installation location is `/usr/local/`:
```bash
mkdir build
cd build
cmake ..  # -DCMAKE_INSTALL_PREFIX=/path/to/install  Modify the installation location
make
sudo make install  # Default installation is to /usr/local, requires sudo privileges
```

## Example
Create a project with two files:
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

`main.cpp` takes `add_scalar` as an example (for more examples, refer to [test_summary_writer.cc](./tests/test_summary_writer.cc)):

```cpp
#include <tensorboard_logger/summary_writer.h>
#include <filesystem>

using tensorboard::SummaryWriter;
using tensorboard::get_current_timestamp;
using tensorboard::get_executable_path;
using tensorboard::get_root_path;
namespace fs = std::filesystem;

// Usually, the cmake executable file is in the build/ folder,
// use twice the parent path to return to the root directory.
fs::path PATH_ROOT = get_executable_path().parent_path().parent_path();
// fs::path PATH_ROOT = get_root_path();  // same as above

void test_add_scalar(SummaryWriter& writer) {
  printf("test add scalar\n");
  double a = (rand() % 10 - 5), b = rand() % 5, c = (rand() % 10 - 5) * 100;
  for (int i = -10; i < 10; i++) {
    writer.add_scalar("test/scalar", i, a*i*i - b*i + c);
  }
}

int main() {
  srand(static_cast<uint32_t>(time(NULL)));
  std::string timestamp = get_current_timestamp();
  std::cout << "timestamp=" << timestamp << '\n';
  SummaryWriter writer(PATH_ROOT / "tb_logs" / timestamp);  // Initialize writer by directory
  // SummaryWriter writer("../../tb_logs/" + timestamp);  // (same path) path relative to executable file
  test_add_scalar(writer);
  writer.close();  // Close writer
  return 0;
}
```
Run the program with `cmake -S . -B build && cmake --build build && ./build/main`. You will see the `tb_logs/` directory created. Use TensorBoard in Python to visualize the results:
```bash
python tensorboard --logdir ./tb_logs
```
