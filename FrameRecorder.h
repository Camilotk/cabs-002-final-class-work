#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "PersistenceDAO.h"

class FrameRecorder {
public:
    explicit FrameRecorder(size_t maxFrames = 300) : m_maxFrames(maxFrames) {}
    void enable(bool on) { m_enabled = on; }
    bool enabled() const { return m_enabled; }
    size_t count() const { return m_frames.size(); }
    void clearMemory() { m_frames.clear(); }
    void setCircular(bool circular) { m_circular = circular; }
    bool isCircular() const { return m_circular; }
    void setMaxFrames(size_t newMax) {
        if (newMax == 0) return;
        if (newMax < m_frames.size()) {
            m_frames.resize(newMax);
        }
        m_maxFrames = newMax;
        if (m_overwriteIndex >= m_maxFrames) {
            m_overwriteIndex = 0;
        }
    }
    size_t maxFrames() const { return m_maxFrames; }
    void capture(const sf::RenderWindow& window) {
        if (!m_enabled) return;
        sf::Texture t; t.create(window.getSize().x, window.getSize().y); t.update(window);
        sf::Image img = t.copyToImage();
        if (m_frames.size() < m_maxFrames) {
            m_frames.push_back(std::move(img));
        } else if (m_circular) {
            m_frames[m_overwriteIndex] = std::move(img);
            m_overwriteIndex = (m_overwriteIndex + 1) % m_maxFrames;
        }
    }
    bool save(const std::string& dir, const std::string& prefix="frame") {
        return m_persistence.saveFrames(m_frames, dir, prefix);
    }
    bool exportVideo(const std::string& dir, const std::string& file, int fps=30, const std::string& prefix="frame") {
        if (!save(dir, prefix)) return false;
        return m_persistence.exportMP4(dir, file, fps);
    }
    bool clearSaved(const std::string& dir, const std::string& prefix="frame") {
        return m_persistence.clearTempFiles(dir, prefix);
    }
private:
    size_t m_maxFrames;
    bool m_enabled = false;
    bool m_circular = false;
    size_t m_overwriteIndex = 0;
    std::vector<sf::Image> m_frames;
    PersistenceDAO m_persistence;
};
