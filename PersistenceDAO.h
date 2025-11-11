#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <iomanip>
#include <filesystem>

class PersistenceDAO {
public:
    bool saveFrames(const std::vector<sf::Image>& frames,
                    const std::string& dirPath,
                    const std::string& prefix = "frame",
                    int startIndex = 0) const;

    bool exportMP4(const std::string& framesDir,
                   const std::string& outputFile,
                   int fps = 30) const;

    bool clearTempFiles(const std::string& framesDir,
                        const std::string& prefix = "frame") const;
};
