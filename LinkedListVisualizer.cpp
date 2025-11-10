#include "LinkedListVisualizer.h"
#include <format>
#include <iterator>

LinkedListVisualizer::LinkedListVisualizer(sf::Font& font, const sf::Vector2f& position)
    : m_font(font), m_position(position) {
    // cria lista inicial com 3 nós
    std::vector<int> initial_data = {77, 88, 99};
    for(size_t i = 0; i < initial_data.size(); ++i) {
        m_nodes.push_back({initial_data[i], sf::Color::Yellow, getPositionForIndex(i)});
    }
}

// calcula a posição visual do nó com base no índice
sf::Vector2f LinkedListVisualizer::getPositionForIndex(size_t i) {
    return { m_position.x + i * (NODE_WIDTH + PTR_WIDTH + SPACING), m_position.y };
}

// adiciona operação de inserção no início
void LinkedListVisualizer::push_front(int value) {
    std::string desc = std::format("Lista: PushFront({})", value);
    m_operationQueue.push_back({desc, [this, value]() {
        buildPushFrontAnimation(value);
    }});
}

// adiciona operação de remoção do início
void LinkedListVisualizer::pop_front() {
    std::string desc = "Lista: PopFront()";
    m_operationQueue.push_back({desc, [this]() {
        buildPopFrontAnimation();
    }});
}

// cria animação da inserção no início
void LinkedListVisualizer::buildPushFrontAnimation(int value) {
    for (size_t i = 0; i < m_nodes.size(); ++i) {
        m_animationQueue.push_back(std::make_unique<MoveStep>(i, getPositionForIndex(i + 1)));
    }
    sf::Vector2f startPos = { getPositionForIndex(0).x, m_position.y - NODE_HEIGHT * 2 };
    m_animationQueue.push_back(std::make_unique<DataInsertStep>(value, 0, startPos));
    m_animationQueue.push_back(std::make_unique<MoveStep>(0, getPositionForIndex(0)));
    m_animationQueue.push_back(std::make_unique<ColorStep>(0, sf::Color::Yellow));
}

// cria animação da remoção do primeiro nó
void LinkedListVisualizer::buildPopFrontAnimation() {
    if (m_nodes.empty()) return;

    m_animationQueue.push_back(std::make_unique<ColorStep>(0, sf::Color::Red));
    sf::Vector2f targetPos = { getPositionForIndex(0).x, m_position.y - NODE_HEIGHT * 2 };
    m_animationQueue.push_back(std::make_unique<MoveStep>(0, targetPos, 0.4f));
    m_animationQueue.push_back(std::make_unique<DataRemoveStep>(0));
    for (size_t i = 0; i < m_nodes.size() - 1; ++i) {
        m_animationQueue.push_back(std::make_unique<MoveStep>(i, getPositionForIndex(i)));
    }
}

// retorna a fila de operações pendentes
const std::deque<Command>& LinkedListVisualizer::getOperationQueue() const {
    return m_operationQueue;
}

// executa animações e comandos pendentes
void LinkedListVisualizer::update(float dt) {
    if (m_animationQueue.empty() && !m_operationQueue.empty()) {
        m_operationQueue.front().action();
        m_operationQueue.pop_front();
    }

    if (!m_animationQueue.empty()) {
        if (m_animationQueue.front()->update(m_nodes, dt)) {
            m_animationQueue.pop_front();
        }
    }
}

// desenha lista e setas na tela
void LinkedListVisualizer::draw(sf::RenderWindow& window) {
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

        sf::Text valueText(std::format("{}", node.value), m_font, FONT_SIZE);
        valueText.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = valueText.getLocalBounds();
        valueText.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        valueText.setPosition(node.position.x + NODE_WIDTH / 2.f, node.position.y + NODE_HEIGHT / 2.f);

        window.draw(dataBox);
        window.draw(ptrBox);
        window.draw(valueText);

        if (i + 1 < m_nodes.size()) {
            const auto& next_node = m_nodes[i+1];
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(node.position.x + NODE_WIDTH + PTR_WIDTH / 2.f, node.position.y + NODE_HEIGHT / 2.f), sf::Color::Red),
                sf::Vertex(sf::Vector2f(next_node.position.x, next_node.position.y + NODE_HEIGHT / 2.f), sf::Color::Red)
            };
            window.draw(line, 2, sf::Lines);
        } else {
            sf::Text nullText("NULL", m_font, 16);
            nullText.setFillColor(sf::Color::Red);
            nullText.setPosition(node.position.x + NODE_WIDTH + PTR_WIDTH + 5, node.position.y + NODE_HEIGHT / 3.f);
            window.draw(nullText);
        }
    }
}
