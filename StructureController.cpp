#include "StructureController.h"
#include <cstdlib>
#include "RandomProvider.h"
#include "VectorVisualizer.h" 
#include "LinkedListVisualizer.h"

void StructureController::execute(const std::string& op) {
    if (!m_structure) return;
    if (m_visualizer) {
        if (op == "insert") {
            m_visualizer->queueOperation("Inserir", [this]() {
                size_t before = m_structure->size();
                size_t sz = before;
                size_t idx = sz == 0 ? 0 : (m_rng ? static_cast<size_t>(m_rng->nextIntRange(0, static_cast<int>(sz))) : static_cast<size_t>(rand() % (sz + 1)));
                int val = m_rng ? m_rng->nextInt() : (rand() % 100);
                m_structure->insert(idx, val);
                size_t after = m_structure->size();
                if (after == before + 1) {
                    if (auto vec = dynamic_cast<VectorVisualizer*>(m_visualizer)) {
                        vec->insert(val, idx);
                    } else if (auto list = dynamic_cast<LinkedListVisualizer*>(m_visualizer)) {
                        if (idx == 0) list->push_front(val);
                    }
                }
            });
        } else if (op == "remove") {
            m_visualizer->queueOperation("Remover", [this]() {
                if (m_structure->size() > 0) {
                    size_t before = m_structure->size();
                    m_structure->remove(0);
                    size_t after = m_structure->size();
                    if (after + 1 == before) {
                        if (auto vec = dynamic_cast<VectorVisualizer*>(m_visualizer)) {
                            vec->remove(0);
                        } else if (auto list = dynamic_cast<LinkedListVisualizer*>(m_visualizer)) {
                            list->pop_front();
                        }
                    }
                }
            });
        } else if (op == "highlight") {
            m_visualizer->queueOperation("Highlight", [this]() {
                if (m_structure->size() > 0) m_visualizer->highlight(0);
            });
        }
    } else {
        if (op == "insert") {
            size_t idx = m_structure->size() ? std::min<size_t>(1, m_structure->size()) : 0;
            int val = rand() % 100;
            m_structure->insert(idx, val);
        } else if (op == "remove") {
            if (m_structure->size() > 0) m_structure->remove(0);
        } else if (op == "highlight") {}
    }
}

void StructureController::executeAndRecord(const std::string& op, CommandRecorder* recorder, const std::string& targetName) {
    if (!m_structure) return;
    if (m_visualizer) {
        if (op == "insert") {
            m_visualizer->queueOperation("Inserir", [this, recorder, targetName]() {
                size_t before = m_structure->size();
                size_t sz = before;
                size_t idx = sz == 0 ? 0 : (m_rng ? static_cast<size_t>(m_rng->nextIntRange(0, static_cast<int>(sz))) : static_cast<size_t>(rand() % (sz + 1)));
                int val = m_rng ? m_rng->nextInt() : (rand() % 100);
                m_structure->insert(idx, val);
                size_t after = m_structure->size();
                if (after == before + 1) {
                    if (auto vec = dynamic_cast<VectorVisualizer*>(m_visualizer)) vec->insert(val, idx);
                    else if (auto list = dynamic_cast<LinkedListVisualizer*>(m_visualizer)) { if (idx == 0) list->push_front(val); }
                    if (recorder) recorder->record("INSERT", targetName, idx, val);
                }
            });
        } else if (op == "remove") {
            m_visualizer->queueOperation("Remover", [this, recorder, targetName]() {
                if (m_structure->size() > 0) {
                    size_t before = m_structure->size();
                    m_structure->remove(0);
                    size_t after = m_structure->size();
                    if (after + 1 == before) {
                        if (auto vec = dynamic_cast<VectorVisualizer*>(m_visualizer)) vec->remove(0);
                        else if (auto list = dynamic_cast<LinkedListVisualizer*>(m_visualizer)) list->pop_front();
                        if (recorder) recorder->record("REMOVE", targetName, 0, std::nullopt);
                    }
                }
            });
        } else if (op == "highlight") {
            m_visualizer->queueOperation("Highlight", [this, recorder, targetName]() {
                if (m_structure->size() > 0) {
                    m_visualizer->highlight(0);
                    if (recorder) recorder->record("HIGHLIGHT", targetName, 0, std::nullopt);
                }
            });
        }
    } else {
        if (op == "insert") {
            size_t idx = m_structure->size() ? std::min<size_t>(1, m_structure->size()) : 0;
            int val = rand() % 100;
                m_structure->insert(idx, val);
            if (recorder) recorder->record("INSERT", targetName, idx, val);
        } else if (op == "remove") {
            if (m_structure->size() > 0) {
                m_structure->remove(0);
                if (recorder) recorder->record("REMOVE", targetName, 0, std::nullopt);
            }
        } else if (op == "highlight") {
            if (m_structure->size() > 0) {
                if (recorder) recorder->record("HIGHLIGHT", targetName, 0, std::nullopt);
            }
        }
    }
}

void StructureController::insertAt(size_t idx, int val) { if (m_structure) m_structure->insert(idx, val); }
void StructureController::removeAt(size_t idx) { if (m_structure && m_structure->size() > idx) m_structure->remove(idx); }
void StructureController::highlightAt(size_t idx) { if (m_structure && m_structure->size() > idx && m_visualizer) m_visualizer->highlight(idx); }
