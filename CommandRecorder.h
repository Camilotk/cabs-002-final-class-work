#pragma once
#include <vector>
#include <string>
#include <optional>
#include <fstream>
#include <sstream>
#include <iomanip>

struct RecordedCommand {
    std::string op;       // INSERT / REMOVE / HIGHLIGHT
    std::string target;   // "vector" ou "list"
    size_t index = 0;     // índice usado
    int value = 0;        // valor (apenas para INSERT)
    bool hasValue = false;
    double t = 0.0;       // timestamp relativo (segundos desde start())
};

class CommandRecorder {
public:
    void start();
    void stop() { m_recording = false; }
    void toggle() { m_recording ? stop() : start(); }
    bool isRecording() const { return m_recording; }

    void record(const std::string& op, const std::string& target, size_t index, std::optional<int> value);

    const std::vector<RecordedCommand>& get() const { return m_commands; }
    void clear() { m_commands.clear(); }

    bool save(const std::string& filePath) const;
    bool load(const std::string& filePath);
    bool saveJSON(const std::string& filePath) const;
    bool loadJSON(const std::string& filePath);

    static std::optional<RecordedCommand> parseLine(const std::string& line);

private:
    bool m_recording = false;
    std::vector<RecordedCommand> m_commands;
    double m_startTime = 0.0;
    unsigned int m_seed = 0;
public:
    unsigned int seed() const { return m_seed; }
};
