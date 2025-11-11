#pragma once
#include <random>
#include <cstdint>

class RandomProvider {
public:
    RandomProvider() : m_engine(std::random_device{}()) {}
    void setSeed(uint32_t seed) { m_engine.seed(seed); m_seed = seed; m_hasSeed = true; }
    bool hasSeed() const { return m_hasSeed; }
    uint32_t seed() const { return m_seed; }
    int nextInt(int minInclusive=0, int maxInclusive=99) {
        std::uniform_int_distribution<int> dist(minInclusive, maxInclusive);
        return dist(m_engine);
    }
    int nextIntRange(int minInclusive, int maxInclusive) {
        std::uniform_int_distribution<int> dist(minInclusive, maxInclusive);
        return dist(m_engine);
    }
private:
    std::mt19937 m_engine;
    uint32_t m_seed = 0;
    bool m_hasSeed = false;
};
