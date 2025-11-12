#pragma once
#include "Visualizer.h"
#include "LayoutPolicy.h"

class VectorVisualizer final : public Visualizer {
public:
    VectorVisualizer(sf::Font& font, const sf::Vector2f& position);

    void insert(int value, size_t index);
    void remove(size_t index);
    void clearAnimated();
    
    void draw(sf::RenderWindow& window) const override;
    void reflow(float windowWidth, float panelWidth=280.f);

private:
    void buildInsertAnimation(int value, size_t index);
    void buildRemoveAnimation(size_t index);

    sf::Vector2f getPositionForIndex(size_t i) const;

    sf::Font& m_font;
    sf::Vector2f m_position;
    std::unique_ptr<ILayoutPolicy> m_layout; 

    static constexpr float BOX_WIDTH = 60.f;
    static constexpr float BOX_HEIGHT = 60.f;
    static constexpr float SPACING = 10.f;
    static constexpr int FONT_SIZE = 24;
    float m_lastLayoutWidth = 0.f;
    size_t m_lastNodeCount = 0;
};
