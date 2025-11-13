#pragma once
#include "AnimationStrategy.h"
#include "FrameRecorder.h"
#include "VisualizerBase.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Visualizer : public VisualizerBase {
public:
  void setStrategy(std::unique_ptr<AnimationStrategy> s) {
    m_strategy = std::move(s);
  }
  void render(const std::vector<int> &state);
  void highlight(size_t index);
  void exportFrames(const std::string &dirPath);
  void exportFramesWithProgress(
      const std::string &dirPath,
      const std::function<void(size_t, size_t)> &onProgress,
      const std::function<bool()> &shouldCancel = nullptr);
  void exportAsMP4(const std::string &dirPath, const std::string &mp4File,
                   int fps = 30);
  void exportAsMP4WithProgress(
      const std::string &dirPath, const std::string &mp4File, int fps,
      const std::function<void(const std::string &)> &onProgress);
  void captureFrame(const sf::RenderWindow &window);
  void refreshPositions(std::function<sf::Vector2f(size_t)> positionFn);
  bool saveFramesDAO(const std::string &dirPath);
  void clearMemoryFrames();
  bool clearSavedFrames(const std::string &dirPath);
  void toggleCapture() { m_recorder.enable(!m_recorder.enabled()); }
  bool isCaptureEnabled() const { return m_recorder.enabled(); }
  void setCaptureMaxFrames(size_t maxFrames) {
    m_recorder.setMaxFrames(maxFrames);
  }
  size_t getCapturedFrameCount() const { return m_recorder.count(); }
  size_t getCaptureLimit() const { return m_recorder.maxFrames(); }
  bool isIdle() const { return m_animationQueue.empty(); }
  void queueOperation(const std::string &description,
                      std::function<void()> action) {
    enqueueOperation(description, std::move(action));
  }

protected:
  std::unique_ptr<AnimationStrategy> m_strategy = nullptr;
  FrameRecorder m_recorder;
};
