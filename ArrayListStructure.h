#pragma once
#include "AbstractDataStructure.h"

class ArrayListStructure : public AbstractDataStructure {
public:
    explicit ArrayListStructure(size_t capacity = 8) : m_capacity(capacity) {}
    void insert(size_t index, int value) override;
    void remove(size_t index) override;
    std::optional<int> access(size_t index) const override;
    void clear() override;
    size_t capacity() const { return m_capacity; }
private:
    void shiftElements(size_t fromIndex, int dir);
    size_t m_capacity;
};
