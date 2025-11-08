#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>

// Representa um nó visual na tela (valor, cor e posição)
struct VisualNode {
    int value;
    sf::Color color = sf::Color::Cyan;
    sf::Vector2f position;
};

// Passo genérico de animação
class AnimationStep {
public:
    virtual ~AnimationStep() = default;
    virtual bool update(std::vector<VisualNode>& nodes, float dt) = 0;
};

// Anima a mudança de cor de um nó
class ColorStep : public AnimationStep {
public:
    ColorStep(size_t index, sf::Color targetColor, float duration = 0.2f)
        : m_index(index), m_targetColor(targetColor), m_duration(duration) {}

    bool update(std::vector<VisualNode>& nodes, float dt) override {
        if (m_elapsed == 0.f) {
            m_startColor = nodes[m_index].color; // cor inicial
        }
        m_elapsed += dt;
        float ratio = std::min(m_elapsed / m_duration, 1.0f);

        // interpola entre as cores
        sf::Uint8 r = static_cast<sf::Uint8>(m_startColor.r + (m_targetColor.r - m_startColor.r) * ratio);
        sf::Uint8 g = static_cast<sf::Uint8>(m_startColor.g + (m_targetColor.g - m_startColor.g) * ratio);
        sf::Uint8 b = static_cast<sf::Uint8>(m_startColor.b + (m_targetColor.b - m_startColor.b) * ratio);
        nodes[m_index].color = sf::Color(r, g, b);

        return m_elapsed >= m_duration; // termina quando chegar na cor final
    }

private:
    size_t m_index;
    sf::Color m_startColor;
    sf::Color m_targetColor;
    float m_duration;
    float m_elapsed = 0.f;
};

// Anima o movimento de um nó até uma nova posição
class MoveStep : public AnimationStep {
public:
    MoveStep(size_t index, sf::Vector2f targetPosition, float duration = 0.5f)
        : m_index(index), m_targetPosition(targetPosition), m_duration(duration) {}

    bool update(std::vector<VisualNode>& nodes, float dt) override {
        if (m_elapsed == 0.f) {
            m_startPosition = nodes[m_index].position; // posição inicial
        }
        m_elapsed += dt;
        float ratio = std::min(m_elapsed / m_duration, 1.0f);

        // interpola posição
        nodes[m_index].position.x = m_startPosition.x + (m_targetPosition.x - m_startPosition.x) * ratio;
        nodes[m_index].position.y = m_startPosition.y + (m_targetPosition.y - m_startPosition.y) * ratio;

        return m_elapsed >= m_duration; // termina quando chegar no destino
    }
private:
    size_t m_index;
    sf::Vector2f m_startPosition;
    sf::Vector2f m_targetPosition;
    float m_duration;
    float m_elapsed = 0.f;
};

// Insere um novo nó na lista
class DataInsertStep : public AnimationStep {
public:
    DataInsertStep(int value, size_t index, sf::Vector2f startPos)
        : m_value(value), m_index(index), m_startPos(startPos) {}

    bool update(std::vector<VisualNode>& nodes, float /*dt*/) override {
        VisualNode newNode;
        newNode.value = m_value;
        newNode.position = m_startPos;
        newNode.color = sf::Color::Green;
        nodes.insert(nodes.begin() + m_index, newNode); // insere na posição
        return true; // executa apenas uma vez
    }
private:
    int m_value;
    size_t m_index;
    sf::Vector2f m_startPos;
};

// Remove um nó existente
class DataRemoveStep : public AnimationStep {
public:
    DataRemoveStep(size_t index) : m_index(index) {}

    bool update(std::vector<VisualNode>& nodes, float /*dt*/) override {
        if (m_index < nodes.size()) {
            nodes.erase(nodes.begin() + m_index); // apaga nó
        }
        return true;
    }
private:
    size_t m_index;
};
