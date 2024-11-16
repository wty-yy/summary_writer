#include "summary_writer.h"
#include <random>

using tensorboard::SummaryWriter;
using tensorboard::get_current_timestamp;
using tensorboard::get_executable_path;
using tensorboard::get_root_path;
namespace fs = std::filesystem;

fs::path PATH_ROOT = get_executable_path().parent_path().parent_path();

std::string read_binary_file(const std::string& filename) {
  std::ostringstream ss;
  std::ifstream fin(filename, std::ios::binary);
  if (!fin) {
      std::cerr << "failed to open file " << filename << std::endl;
      return "";
  }
  ss << fin.rdbuf();
  fin.close();
  return ss.str();
}

void test_add_scalar(SummaryWriter& writer) {
  printf("test add scalar\n");
  double a = (rand() % 10-5), b = rand() % 5, c = (rand() % 10 - 5) * 100;
  for (int i = -10; i < 10; i++) {
    writer.add_scalar("test/scalar", i, a*i*i-b*i+c);
  }
}

void test_add_histogram(SummaryWriter& writer) {
  printf("test add histogram\n");
  std::default_random_engine generator(time(NULL));
  for (int i = 0; i < 10; ++i) {
    std::normal_distribution<double> norm(i*0.1, 1.0);
    std::vector<double> values;
    for (int j = 0; j < 100000; ++j)
      values.push_back(norm(generator));
    writer.add_histogram("histogram", i, values);
  }
}

void test_add_image(SummaryWriter& writer) {
  printf("test add image\n");
  auto img1 = read_binary_file(PATH_ROOT / "assets/text.png");
  writer.add_image("Text Plugin", 1, img1, 1864, 822, 3, "TensorBoard", "Text");
  writer.add_image("Text Plugin", 2, img1, 1864, 822, 3, "TensorBoard", "Text");
  writer.add_image("Text Plugin", 3, img1, 1864, 822, 3, "TensorBoard", "Text");
}

void test_add_audio(SummaryWriter &writer) {
  printf("test add audio\n");
  auto audio = read_binary_file(PATH_ROOT / "assets/file_example_WAV_1MG.wav");
  writer.add_audio("Audio Sample", 1, audio, 8000, 2, 8000 * 16 * 2 * 33,
                  "audio/wav", "Impact Moderato",
                  "https://file-examples.com/index.php/sample-audio-files/"
                  "sample-wav-download/");
}

int test_add_text(SummaryWriter &writer) {
  printf("test log text\n");
  writer.add_text("Text Sample", 1, "Hello World1");
  writer.add_text("Text Sample", 2, "Hello World2");
  writer.add_text("Text Sample", 3, "Hello World3");
  return 0;
}


int main() {
  std::string timestamp = get_current_timestamp();
  std::cout << "timestamp=" << timestamp << '\n';
  srand(static_cast<uint32_t>(time(NULL)));
  // SummaryWriter writer(PATH_ROOT / "tb_logs" / timestamp);
  // SummaryWriter writer(fs::path("../../tb_logs/123") / timestamp);
  SummaryWriter writer(get_root_path() / "tb_logs/321" / timestamp);
  test_add_scalar(writer);
  test_add_histogram(writer);
  test_add_image(writer);
  test_add_audio(writer);
  test_add_text(writer);
  writer.close();
  return 0;
}