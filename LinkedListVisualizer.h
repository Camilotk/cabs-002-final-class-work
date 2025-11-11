#pragma once
#include "Visualizer.h"
#include "LayoutPolicy.h"

class LinkedListVisualizer final : public Visualizer {
public:
    LinkedListVisualizer(sf::Font& font, const sf::Vector2f& position);

    void push_front(int value);
    void pop_front();
    
    void draw(sf::RenderWindow& window) const override;
    void reflow(float windowWidth, float panelWidth = 280.f);

private:
    void buildPushFrontAnimation(int value);
    void buildPopFrontAnimation();

    sf::Vector2f getPositionForIndex(size_t i);

    sf::Font& m_font;
    sf::Vector2f m_position;
    std::unique_ptr<ILayoutPolicy> m_layout; 

    static constexpr float NODE_WIDTH = 70.f;
    static constexpr float NODE_HEIGHT = 50.f;
    static constexpr float PTR_WIDTH = 25.f;
    static constexpr float SPACING = 60.f;
    static constexpr int FONT_SIZE = 22;
    float m_lastLayoutWidth = 0.f;
    size_t m_lastNodeCount = 0;
};
