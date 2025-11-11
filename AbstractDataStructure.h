#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "AnimationStrategy.h"

class Visualizer;

class AbstractDataStructure {
public:
    virtual ~AbstractDataStructure() = default;
    void attach(Visualizer* v) { m_visualizer = v; }
    virtual void insert(size_t index, int value) = 0;
    virtual void remove(size_t index) = 0;
    virtual std::optional<int> access(size_t index) const = 0;
    const std::vector<int>& getState() const { return m_elements; }
    size_t size() const { return m_elements.size(); }
protected:
    void notify();
    std::vector<int> m_elements;
    Visualizer* m_visualizer = nullptr;
};
