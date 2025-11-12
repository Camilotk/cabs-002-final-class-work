#include "ArrayListStructure.h"

void ArrayListStructure::insert(size_t index, int value) {
    if (index > m_elements.size()) return;
    if (m_elements.size() >= m_capacity) {
        size_t newCap = m_capacity == 0 ? 1 : m_capacity * 2;
        while (newCap <= m_elements.size()) newCap *= 2;
        m_capacity = newCap;
    }
    shiftElements(index, +1);
    m_elements.insert(m_elements.begin() + index, value);
    notify();
}

void ArrayListStructure::remove(size_t index) {
    if (index >= m_elements.size()) return;
    m_elements.erase(m_elements.begin() + index);
    shiftElements(index, -1);
    notify();
}

std::optional<int> ArrayListStructure::access(size_t index) const {
    if (index >= m_elements.size()) return std::nullopt;
    return m_elements[index];
}

void ArrayListStructure::shiftElements(size_t fromIndex, int dir) {
    (void)fromIndex; (void)dir;
}

void ArrayListStructure::clear() {
    m_elements.clear();
    notify();
}
