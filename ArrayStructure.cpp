#include "ArrayStructure.h"

void ArrayStructure::insert(size_t index, int value) {
    if (index > m_elements.size() || m_elements.size() >= m_capacity) return;
    m_elements.insert(m_elements.begin() + index, value);
    notify();
}

void ArrayStructure::remove(size_t index) {
    if (index >= m_elements.size()) return;
    m_elements.erase(m_elements.begin() + index);
    notify();
}

std::optional<int> ArrayStructure::access(size_t index) const {
    if (index >= m_elements.size()) return std::nullopt;
    return m_elements[index];
}

void ArrayStructure::resize(size_t newCap) {
    if (newCap < m_elements.size()) return;
    m_capacity = newCap;
    notify();
}
