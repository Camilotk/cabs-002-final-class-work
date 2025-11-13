#pragma once
#include <string>
#include <vector>
#ifndef SFML_FORWARD_DECL_IMAGE
#define SFML_FORWARD_DECL_IMAGE
namespace sf {
class Image;
}
#endif
#include <filesystem>
#include <functional>
#include <iomanip>

class PersistenceDAO {
public:
  bool
  saveFrames(const std::vector<sf::Image> &frames, const std::string &dirPath,
             const std::string &prefix = "frame", int startIndex = 0,
             const std::function<void(size_t, size_t)> &onProgress = nullptr,
             const std::function<bool()> &shouldCancel = nullptr) const;

  bool exportMP4(const std::string &framesDir, const std::string &outputFile,
                 int fps = 30) const;

  bool exportMP4WithProgress(
      const std::string &framesDir, const std::string &outputFile, int fps,
      const std::function<void(const std::string &)> &onProgress) const;

  bool exportMP4WithProgressCancelable(
      const std::string &framesDir, const std::string &outputFile, int fps,
      pid_t &outPid,
      const std::function<void(const std::string &)> &onProgress) const;

  static bool cancelProcess(pid_t pid);

  bool clearTempFiles(const std::string &framesDir,
                      const std::string &prefix = "frame") const;
};
