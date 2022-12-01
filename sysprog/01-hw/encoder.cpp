#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

class Encoder {
  fs::path path_;
  const char kEncryptionKey = '3';
  const size_t kReadBlockSize = 1 << 18;

 public:
  Encoder(const std::string& p) : path_(p) {}

  void Encode() {
    if (path_.empty()) {
      std::cout << "No input directory for encoder\n";
      return;
    }

    WalkDir(path_);
  }

 private:
  void WalkDir(const fs::path& path) {
    for (auto const& entry : fs::recursive_directory_iterator{path}) {
      if (entry.is_regular_file()) {
        EncodeRegularFile(entry);
      }
    }
  }

  void EncodeRegularFile(const fs::path& path) {
    std::fstream file;
    file.open(path.string());
    if (!file.is_open()) return;

    char buf[kReadBlockSize];
    file.seekg(0);

    size_t read_bytes = file.readsome(buf, kReadBlockSize - 1);
    while (read_bytes > 0) {
      buf[read_bytes] = '\0';
      EncodeBuf(buf, read_bytes);

      const auto current_position_indicator = file.tellp();
      file.seekg(current_position_indicator - std::streampos(read_bytes));

      file.write(buf, read_bytes);

      file.seekg(current_position_indicator);

      read_bytes = file.readsome(buf, kReadBlockSize - 1);
    }

    file.close();
  }

  void EncodeBuf(char* buf, size_t size) {
    for (size_t i = 0; i < size - 1; ++i) {
      buf[i] ^= kEncryptionKey;
    }
  }
};

int main() {
  const std::string path = "/home/salim/acronis/trash/encoder_dir";
  auto encoder = Encoder(path);
  encoder.Encode();
  return 0;
}