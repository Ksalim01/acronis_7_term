#include <filesystem>
#include <string>

std::string foo() {
  try {
    return std::filesystem::canonical("sfsdfkjsjfkdsdf/sdfsdfsf");
  } catch (...) {
    return "";
  }
}


int main() {
  foo();
  return 0;
}
