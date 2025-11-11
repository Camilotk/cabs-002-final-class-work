#include "LinkedListVisualizer.h"

LinkedListVisualizer::LinkedListVisualizer(sf::Font& font, const sf::Vector2f& position)
    : m_font(font), m_position(position), m_layout(std::make_unique<ListLayoutPolicy>(NODE_WIDTH, PTR_WIDTH, SPACING)) {
}

sf::Vector2f LinkedListVisualizer::getPositionForIndex(size_t i) {
    if (m_layout) return m_layout->positionForIndex(i, m_position);
    return { m_position.x + i * (NODE_WIDTH + PTR_WIDTH + SPACING), m_position.y }; // fallback
}

void LinkedListVisualizer::reflow(float windowWidth, float panelWidth) {
    if (!isIdle()) return;
    bool widthChanged = (windowWidth != m_lastLayoutWidth);
    bool countChanged = (m_lastNodeCount != m_nodes.size());
    if (!widthChanged && !countChanged) return;
    m_lastLayoutWidth = windowWidth;
    m_lastNodeCount = m_nodes.size();

    float margin = 50.f;
    float available = std::max(200.f, windowWidth - panelWidth - m_position.x - margin);
    float stride = NODE_WIDTH + PTR_WIDTH + SPACING;
    int maxCols = static_cast<int>(available / stride);
    if (maxCols < 1) maxCols = 1;
    if (maxCols == 1 && available > stride * 0.8f) {
        stride = stride * 0.9f;
    }

    float rowHeight = NODE_HEIGHT + 70.f; 
    for (size_t i = 0; i < m_nodes.size(); ++i) {
        int row = static_cast<int>(i / maxCols);
        int col = static_cast<int>(i % maxCols);
        float x = m_position.x + col * stride;
        float y = m_position.y + row * rowHeight;
        m_nodes[i].position = {x, y};
    }
}

void LinkedListVisualizer::push_front(int value) {
    std::string desc = "Lista: PushFront(" + std::to_string(value) + ")";
    enqueueOperation(desc, [this, value]() {
        buildPushFrontAnimation(value);
    });
}

void LinkedListVisualizer::pop_front() {
    enqueueOperation("Lista: PopFront()", [this]() {
        buildPopFrontAnimation();
    });
}

void LinkedListVisualizer::buildPushFrontAnimation(int value) {
    size_t oldSize = m_nodes.size();
    sf::Vector2f startPos = { getPositionForIndex(0).x, m_position.y - NODE_HEIGHT * 2 };
    enqueueAnimation(std::make_unique<DataInsertStep>(value, 0, startPos));
    enqueueAnimation(std::make_unique<MoveStep>(0, getPositionForIndex(0)));
    for (size_t i = 1; i <= oldSize; ++i) {
        enqueueAnimation(std::make_unique<MoveStep>(i, getPositionForIndex(i)));
    }
    enqueueAnimation(std::make_unique<ColorStep>(0, sf::Color::Yellow));
}

void LinkedListVisualizer::buildPopFrontAnimation() {
    if (m_nodes.empty()) return;

    enqueueAnimation(std::make_unique<ColorStep>(0, sf::Color::Red));
    sf::Vector2f targetPos = { getPositionForIndex(0).x, m_position.y - NODE_HEIGHT * 2 };
    enqueueAnimation(std::make_unique<MoveStep>(0, targetPos, 0.4f));
    enqueueAnimation(std::make_unique<DataRemoveStep>(0));
    for (size_t i = 0; i < m_nodes.size() - 1; ++i) {
        enqueueAnimation(std::make_unique<MoveStep>(i, getPositionForIndex(i)));
    }
}

void LinkedListVisualizer::draw(sf::RenderWindow& window) const {
    sf::Text title("std::list (Linked List)", m_font, 20);
    title.setPosition(m_position.x, m_position.y - 40);
    title.setFillColor(sf::Color::White);
    window.draw(title);

    sf::Text headText("head", m_font, 18);
    headText.setFillColor(sf::Color::Yellow);

    if (!m_nodes.empty()) {
        sf::FloatRect headBounds = headText.getLocalBounds();
        headText.setPosition(m_nodes.front().position.x + NODE_WIDTH / 2.f - headBounds.width / 2.f, m_nodes.front().position.y - 30.f);
        window.draw(headText);
    }
    
    for (size_t i = 0; i < m_nodes.size(); ++i) {
        const auto& node = m_nodes[i];
        
        sf::RectangleShape dataBox(sf::Vector2f(NODE_WIDTH, NODE_HEIGHT));
        dataBox.setPosition(node.position);
        dataBox.setFillColor(sf::Color(20, 20, 80));
        dataBox.setOutlineColor(node.color);
        dataBox.setOutlineThickness(2.f);

        sf::RectangleShape ptrBox(sf::Vector2f(PTR_WIDTH, NODE_HEIGHT));
        ptrBox.setPosition(node.position.x + NODE_WIDTH, node.position.y);
        ptrBox.setFillColor(sf::Color(50, 50, 50));
        ptrBox.setOutlineColor(node.color);
        ptrBox.setOutlineThickness(1.f);

    sf::Text valueText(std::to_string(node.value), m_font, FONT_SIZE);
        valueText.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = valueText.getLocalBounds();
        valueText.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        valueText.setPosition(node.position.x + NODE_WIDTH / 2.f, node.position.y + NODE_HEIGHT / 2.f);
        
    window.draw(dataBox);
        window.draw(ptrBox);
        window.draw(valueText);

        if (i + 1 < m_nodes.size()) {
            const auto& next_node = m_nodes[i+1];
            bool wrapped = (next_node.position.y != node.position.y);
            if (!wrapped) {
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(node.position.x + NODE_WIDTH + PTR_WIDTH / 2.f, node.position.y + NODE_HEIGHT / 2.f), sf::Color::Red),
                    sf::Vertex(sf::Vector2f(next_node.position.x, next_node.position.y + NODE_HEIGHT / 2.f), sf::Color::Red)
                };
                window.draw(line, 2, sf::Lines);
            } else {
                float startX = node.position.x + NODE_WIDTH + PTR_WIDTH / 2.f;
                float midX = startX + 30.f; // avanço horizontal
                float midY = next_node.position.y + NODE_HEIGHT / 2.f;
                sf::Vertex elbow[] = {
                    sf::Vertex(sf::Vector2f(startX, node.position.y + NODE_HEIGHT / 2.f), sf::Color::Red),
                    sf::Vertex(sf::Vector2f(midX, node.position.y + NODE_HEIGHT / 2.f), sf::Color::Red),
                    sf::Vertex(sf::Vector2f(midX, midY), sf::Color::Red),
                    sf::Vertex(sf::Vector2f(next_node.position.x, midY), sf::Color::Red)
                };
                window.draw(elbow, 4, sf::LineStrip);
            }
        } else {
            sf::Text nullText("NULL", m_font, 16);
            nullText.setFillColor(sf::Color::Red);
            nullText.setPosition(node.position.x + NODE_WIDTH + PTR_WIDTH + 5, node.position.y + NODE_HEIGHT / 3.f);
            window.draw(nullText);
        }
    }
}
