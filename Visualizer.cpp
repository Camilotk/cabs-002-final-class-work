#include "Visualizer.h"
#include <iostream>

void Visualizer::render(const std::vector<int> &state) {
  if (m_nodes.size() == state.size()) {
    for (size_t i = 0; i < state.size(); ++i)
      m_nodes[i].value = state[i];
  }
}

void Visualizer::highlight(size_t index) {
  if (index < m_nodes.size()) {
    enqueueAnimation(
        std::make_unique<ColorStep>(index, sf::Color::Magenta, 0.25f));
    enqueueAnimation(
        std::make_unique<ColorStep>(index, sf::Color::Cyan, 0.25f));
  }
}

void Visualizer::exportFrames(const std::string &dirPath) {
  if (m_recorder.enabled()) {
    clearSavedFrames(dirPath);
  }
  if (!m_recorder.save(dirPath)) {
    std::cerr << "[Visualizer] Falha ao exportar frames." << '\n';
  }
}

void Visualizer::exportFramesWithProgress(
    const std::string &dirPath,
    const std::function<void(size_t, size_t)> &onProgress,
    const std::function<bool()> &shouldCancel) {
  if (m_recorder.enabled()) {
    clearSavedFrames(dirPath);
  }
  if (!m_recorder.save(dirPath, "frame", onProgress, shouldCancel)) {
    std::cerr << "[Visualizer] Falha ao exportar frames." << '\n';
  }
}

void Visualizer::captureFrame(const sf::RenderWindow &window) {
  m_recorder.capture(window);
}

void Visualizer::refreshPositions(
    std::function<sf::Vector2f(size_t)> positionFn) {
  for (size_t i = 0; i < m_nodes.size(); ++i) {
    m_nodes[i].position = positionFn(i);
  }
}

bool Visualizer::saveFramesDAO(const std::string &dirPath) {
  return m_recorder.save(dirPath);
}

void Visualizer::exportAsMP4(const std::string &dirPath,
                             const std::string &mp4File, int fps) {
  if (m_recorder.enabled()) {
    clearSavedFrames(dirPath);
  }
  if (!m_recorder.exportVideo(dirPath, mp4File, fps)) {
    std::cerr << "[Visualizer] Falha ao gerar MP4." << '\n';
  }
}

void Visualizer::exportAsMP4WithProgress(
    const std::string &dirPath, const std::string &mp4File, int fps,

    const std::function<void(const std::string &)> &onProgress) {
  if (m_recorder.enabled()) {
    clearSavedFrames(dirPath);
  }
  if (!m_recorder.save(dirPath)) {
    std::cerr << "[Visualizer] Falha ao salvar frames para MP4." << '\n';
    return;
  }

  PersistenceDAO dao;
  dao.exportMP4WithProgress(dirPath, mp4File, fps, onProgress);
}

void Visualizer::clearMemoryFrames() {
  m_recorder.clearMemory();
  std::cout << "[Visualizer] Frames em memória limpos." << '\n';
}

bool Visualizer::clearSavedFrames(const std::string &dirPath) {
  bool ok = m_recorder.clearSaved(dirPath);
  if (ok)
    std::cout << "[Visualizer] PNGs removidos de " << dirPath << '\n';
  return ok;
}
