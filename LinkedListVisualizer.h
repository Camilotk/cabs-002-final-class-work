#pragma once
#include "AnimationStep.h"
#include "Command.h"
#include <deque>
#include <functional>

class LinkedListVisualizer {
public:
    LinkedListVisualizer(sf::Font& font, const sf::Vector2f& position);

    void push_front(int value);
    void pop_front();

    void update(float dt);
    void draw(sf::RenderWindow& window);

    const std::deque<Command>& getOperationQueue() const;

private:
    void buildPushFrontAnimation(int value);
    void buildPopFrontAnimation();

    sf::Vector2f getPositionForIndex(size_t i);

    std::vector<VisualNode> m_nodes;
    sf::Font& m_font;
    sf::Vector2f m_position;

    std::deque<std::unique_ptr<AnimationStep>> m_animationQueue;
    std::deque<Command> m_operationQueue;

    const float NODE_WIDTH = 70.f;
    const float NODE_HEIGHT = 50.f;
    const float PTR_WIDTH = 25.f;
    const float SPACING = 60.f;
    const int FONT_SIZE = 22;
};
