#pragma once
#include <memory>
#include <string>
#include "AbstractDataStructure.h"
#include "Visualizer.h"
#include "PersistenceDAO.h"
#include "CommandRecorder.h"

class StructureController {
public:
    StructureController(std::unique_ptr<AbstractDataStructure> s, Visualizer* v, class RandomProvider* rng = nullptr)
        : m_structure(std::move(s)), m_visualizer(v), m_rng(rng) {
        if (m_structure && m_visualizer) m_structure->attach(m_visualizer);
    }
    void execute(const std::string& op);
    void executeAndRecord(const std::string& op, CommandRecorder* recorder, const std::string& targetName);
    void insertAt(size_t idx, int val);
    void removeAt(size_t idx);
    void highlightAt(size_t idx);
    void connect() { if (m_structure && m_visualizer) m_visualizer->render(m_structure->getState()); }
    void runAnimation() {  }
    void exportFrames(const std::string& path) { if (m_visualizer) m_visualizer->exportFrames(path); }
private:
    std::unique_ptr<AbstractDataStructure> m_structure;
    Visualizer* m_visualizer; 
    RandomProvider* m_rng = nullptr;
};
