#pragma once
#include <climits>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

class Detector {
  std::unordered_map<int, std::set<std::filesystem::path>> suspicious_;

  const size_t kEnougWritingsForSuspicion = 10;
  const size_t kChangedFilesInDir = 5;
  const size_t kChangedDirs = 2;

 public:
  void Put(int pid, const std::string& path) { suspicious_[pid].insert(path); }

  bool IsSuspicious(int pid) {
    if (suspicious_.find(pid) == suspicious_.end()) return false;

    size_t changed_dirs = 0;

    std::map<std::filesystem::path, size_t> changed_files_in_dirs;

    for (const auto& path : suspicious_[pid]) {
      auto dir_path = std::filesystem::path(path);
      dir_path.remove_filename();

      if (changed_files_in_dirs.find(dir_path) == changed_files_in_dirs.end()) {
        changed_files_in_dirs[dir_path] = 1;
      } else {
        ++changed_files_in_dirs[dir_path];
      }

      if (changed_files_in_dirs[dir_path] >= kChangedFilesInDir) {
        ++changed_dirs;
      }
    }

    return changed_dirs >= kChangedDirs ||
           suspicious_[pid].size() >= kEnougWritingsForSuspicion;
  }

  size_t ChangedFilesNum(int pid) { return suspicious_[pid].size(); }

  auto AllChangedFiles(int pid) {
    auto result = std::vector<std::string>();

    for (const auto& path : suspicious_[pid]) {
      result.push_back(path);
    }

    return result;
  }

  std::string WorkingDirectory(int pid) {
    std::string res;

    for (const auto& path : suspicious_[pid]) {
      res = CommonPath(res, path.string());
    }

    return res;
  }

  std::string ExecutableFile(int pid) {
    std::string path = "/proc/" + std::to_string(pid) + "/exe";
    return realpath(path.c_str(), nullptr);
  }

 private:
  std::string CommonPath(const std::string& lhs, const std::string& rhs) {
    if (lhs.empty()) return rhs;
    if (rhs.empty()) return lhs;

    const size_t min_len = std::min(lhs.size(), rhs.size());

    std::string result;
    result.reserve(min_len);

    for (size_t i = 0; i < min_len; ++i) {
      if (lhs[i] != rhs[i]) {
        return result;
      }
      result.push_back(lhs[i]);
    }

    return result;
  }
};