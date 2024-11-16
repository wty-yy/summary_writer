#pragma once

#include "tensorboard_logger.h"
#include <cmath>
#include <filesystem>
#include <chrono>
#include <memory>


namespace tensorboard {

namespace fs = std::filesystem;

fs::path get_executable_path();

class SummaryWriter {
 public:
  SummaryWriter(std::string _path_log_dir) {
    if (_path_log_dir[0] == '.') {  // relative path
      path_log_dir = fs::weakly_canonical(get_executable_path()/_path_log_dir);
    } else {
      path_log_dir = _path_log_dir;
    }
    if (!fs::exists(path_log_dir)) fs::create_directories(path_log_dir);
    path_log = path_log_dir / "tfevents.pb";
    logger = std::make_unique<TensorBoardLogger>(path_log);
  }

  template <typename T>
  int add_scalar(const std::string& tag, int step, T value) {
    return logger->add_scalar(tag, step, static_cast<double>(value));
  }

  template <typename T>
  int add_histogram(const std::string &tag, int step, const T *value, size_t num) {
    return logger->add_histogram(tag, step, value, num);
  }

  template <typename T>
  int add_histogram(const std::string &tag, int step, const std::vector<T> &values) {
    return logger->add_histogram(tag, step, values);
  }

  int add_image(const std::string &tag, int step,
    const std::string &encoded_image, int height,
    int width, int channel,
    const std::string &display_name,
    const std::string &description) {
    return logger->add_image(tag, step, encoded_image, height, width,
      channel, display_name, description);
  }

  int add_audio(const std::string &tag, int step,
    const std::string &encoded_audio, float sample_rate,
    int num_channels, int length_frame,
    const std::string &content_type,
    const std::string &display_name,
    const std::string &description) {
    return logger->add_audio(tag, step, encoded_audio, sample_rate,
      num_channels, length_frame, content_type, display_name, description);
  }

  int add_text(const std::string &tag, int step, const char *text) {
    return logger->add_text(tag, step, text);
  }

  int add_embedding(
    const std::string &tensor_name,
    const std::string &tensordata_path,
    const std::string &metadata_path,
    const std::vector<uint32_t> &tensor_shape,
    int step) {
    return logger->add_embedding(tensor_name, tensordata_path,
      metadata_path, tensor_shape, step);
  }

  int add_embedding(
    const std::string &tensor_name,
    const std::vector<std::vector<float>> &tensor,
    const std::string &tensordata_filename,
    const std::vector<std::string> &metadata,
    const std::string &metadata_filename, int step) {
    return logger->add_embedding(tensor_name, tensor, tensordata_filename,
      metadata, metadata_filename, step);
  }

  int add_embedding(
    const std::string &tensor_name,
    const float *tensor,
    const std::vector<uint32_t> &tensor_shape,
    const std::string &tensordata_filename,
    const std::vector<std::string> &metadata,
    const std::string &metadata_filename,
    int step) {
    return logger->add_embedding(tensor_name, tensor, tensor_shape,
      tensordata_filename, metadata, metadata_filename);
  }

  void close() {
    google::protobuf::ShutdownProtobufLibrary();
  }

 private:
  fs::path path_log_dir;
  fs::path path_log;
  std::unique_ptr<TensorBoardLogger> logger;
};


#include <stdexcept>

#ifdef _WIN32
    #include <windows.h>  // Windows-specific header
#elif __linux__
    #include <unistd.h>   // Linux-specific header
#elif __APPLE__
    #include <mach-o/dyld.h> // macOS-specific header
#endif

namespace fs = std::filesystem;

fs::path get_executable_path() {
    char buffer[1024];
#ifdef _WIN32
    // Windows 系统
    if (GetModuleFileNameA(NULL, buffer, sizeof(buffer)) == 0) {
        throw std::runtime_error("Unable to get executable path on Windows.");
    }
    return fs::path(buffer);

#elif __linux__
    // Linux 系统
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len == -1) {
        throw std::runtime_error("Unable to get executable path on Linux.");
    }
    buffer[len] = '\0';  // Null-terminate the path
    return fs::path(buffer);

#elif __APPLE__
    // macOS 系统
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0) {
        throw std::runtime_error("Buffer too small; unable to get executable path on macOS.");
    }
    return fs::canonical(fs::path(buffer));

#else
    throw std::runtime_error("Unsupported platform.");
#endif
}

fs::path get_root_path() {  // {workspace}/build/main.exe -> {workspace}
  return get_executable_path().parent_path().parent_path();
}

std::string get_current_timestamp() {
  // 获取当前时间点
  auto now = std::chrono::system_clock::now();
  // 将时间点转换为 time_t 格式
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  // 将 time_t 转换为本地时间
  std::tm local_tm = *std::localtime(&now_c);

  // 使用 stringstream 格式化时间戳
  std::ostringstream oss;
  oss << std::put_time(&local_tm, "%Y%m%d_%H%M%S"); // 使用格式：YYYYMMDD_HHMMSS
  return oss.str();
}

}