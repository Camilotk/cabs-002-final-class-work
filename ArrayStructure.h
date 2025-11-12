#pragma once
#include "AbstractDataStructure.h"

class ArrayStructure : public AbstractDataStructure {
public:
    explicit ArrayStructure(size_t capacity = 16) : m_capacity(capacity) {
        m_elements.resize(std::min<size_t>(capacity/2, capacity));
        for (size_t i = 0; i < m_elements.size(); ++i) m_elements[i] = static_cast<int>(i * 10);
    }
    void insert(size_t index, int value) override;
    void remove(size_t index) override;
    std::optional<int> access(size_t index) const override;
    void clear() override;
    void resize(size_t newCap);
private:
    size_t m_capacity;
};
