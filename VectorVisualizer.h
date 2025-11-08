#pragma once
#include "AnimationStep.h"
#include "Command.h"
#include <deque>
#include <memory>
#include <functional>

class VectorVisualizer {
public:
    VectorVisualizer(sf::Font& font, const sf::Vector2f& position);

    void insert(int value, size_t index);
    void remove(size_t index);

    void update(float dt);
    void draw(sf::RenderWindow& window);

    const std::deque<Command>& getOperationQueue() const;

private:
    void buildInsertAnimation(int value, size_t index);
    void buildRemoveAnimation(size_t index);

    sf::Vector2f getPositionForIndex(size_t i);

    std::vector<VisualNode> m_nodes;
    sf::Font& m_font;
    sf::Vector2f m_position;

    std::deque<std::unique_ptr<AnimationStep>> m_animationQueue;
    std::deque<Command> m_operationQueue;

    const float BOX_WIDTH = 60.f;
    const float BOX_HEIGHT = 60.f;
    const float SPACING = 10.f;
    const int FONT_SIZE = 24;
};
