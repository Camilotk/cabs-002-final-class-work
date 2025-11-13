#pragma once
#include <SFML/Graphics.hpp>
#include <cstddef>

class ILayoutPolicy {
public:
    virtual ~ILayoutPolicy() = default;
    virtual sf::Vector2f positionForIndex(std::size_t i, sf::Vector2f origin) const = 0;
};

class LinearLayoutPolicy : public ILayoutPolicy {
public:
    LinearLayoutPolicy(float boxWidth, float spacing)
        : m_boxWidth(boxWidth), m_spacing(spacing) {}
    sf::Vector2f positionForIndex(std::size_t i, sf::Vector2f origin) const override {
        return { origin.x + static_cast<float>(i) * (m_boxWidth + m_spacing), origin.y };
    }
private:
    float m_boxWidth;
    float m_spacing;
};

class ListLayoutPolicy : public ILayoutPolicy {
public:
    ListLayoutPolicy(float nodeWidth, float ptrWidth, float spacing)
        : m_nodeWidth(nodeWidth), m_ptrWidth(ptrWidth), m_spacing(spacing) {}
    sf::Vector2f positionForIndex(std::size_t i, sf::Vector2f origin) const override {
        float stride = m_nodeWidth + m_ptrWidth + m_spacing;
        return { origin.x + static_cast<float>(i) * stride, origin.y };
    }
private:
    float m_nodeWidth;
    float m_ptrWidth;
    float m_spacing;
};
