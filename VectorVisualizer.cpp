#include "VectorVisualizer.h"
#include <format>

VectorVisualizer::VectorVisualizer(sf::Font& font, const sf::Vector2f& position)
    : m_font(font), m_position(position) {
    std::vector<int> initial_data = {10, 20, 30, 40};
    for(size_t i = 0; i < initial_data.size(); ++i) {
        m_nodes.push_back({initial_data[i], sf::Color::Cyan, getPositionForIndex(i)});
    }
}

// adiciona operação de inserção à fila
void VectorVisualizer::insert(int value, size_t index) {
    std::string desc = std::format("Vetor: Insert({}, {})", value, index);
    m_operationQueue.push_back({desc, [this, value, index]() {
        buildInsertAnimation(value, index);
    }});
}

// adiciona operação de remoção à fila
void VectorVisualizer::remove(size_t index) {
    std::string desc = std::format("Vetor: Remove({})", index);
    m_operationQueue.push_back({desc, [this, index]() {
        buildRemoveAnimation(index);
    }});
}

// cria sequência de animação da inserção
void VectorVisualizer::buildInsertAnimation(int value, size_t index) {
    if (index > m_nodes.size()) return;

    for (size_t i = index; i < m_nodes.size(); ++i)
        m_animationQueue.push_back(std::make_unique<ColorStep>(i, sf::Color::Yellow));

    for (size_t i = index; i < m_nodes.size(); ++i)
        m_animationQueue.push_back(std::make_unique<MoveStep>(i, getPositionForIndex(i + 1)));

    sf::Vector2f startPos = { getPositionForIndex(index).x, m_position.y - BOX_HEIGHT * 2 };
    m_animationQueue.push_back(std::make_unique<DataInsertStep>(value, index, startPos));
    m_animationQueue.push_back(std::make_unique<MoveStep>(index, getPositionForIndex(index)));

    for (size_t i = 0; i <= m_nodes.size(); ++i)
        m_animationQueue.push_back(std::make_unique<ColorStep>(i, sf::Color::Cyan));
}

// cria sequência de animação da remoção
void VectorVisualizer::buildRemoveAnimation(size_t index) {
    if (index >= m_nodes.size()) return;

    m_animationQueue.push_back(std::make_unique<ColorStep>(index, sf::Color::Red));
    sf::Vector2f targetPos = { getPositionForIndex(index).x, m_position.y - BOX_HEIGHT * 2 };
    m_animationQueue.push_back(std::make_unique<MoveStep>(index, targetPos, 0.4f));
    m_animationQueue.push_back(std::make_unique<DataRemoveStep>(index));

    for (size_t i = index; i < m_nodes.size(); ++i)
        m_animationQueue.push_back(std::make_unique<MoveStep>(i, getPositionForIndex(i)));
}

// retorna a fila de operações
const std::deque<Command>& VectorVisualizer::getOperationQueue() const {
    return m_operationQueue;
}

// executa próxima operação ou animação pendente
void VectorVisualizer::update(float dt) {
    if (m_animationQueue.empty() && !m_operationQueue.empty()) {
        m_operationQueue.front().action();
        m_operationQueue.pop_front();
    }

    if (!m_animationQueue.empty()) {
        if (m_animationQueue.front()->update(m_nodes, dt))
            m_animationQueue.pop_front();
    }
}

// calcula posição visual de um índice
sf::Vector2f VectorVisualizer::getPositionForIndex(size_t i) {
    return { m_position.x + i * (BOX_WIDTH + SPACING), m_position.y };
}

// desenha o vetor e seus índices
void VectorVisualizer::draw(sf::RenderWindow& window) {
    sf::Text title("std::vector (Array List)", m_font, 20);
    title.setPosition(m_position.x, m_position.y - 40);
    title.setFillColor(sf::Color::White);
    window.draw(title);

    for (size_t i = 0; i < m_nodes.size(); ++i) {
        const auto& node = m_nodes[i];
        sf::RectangleShape box(sf::Vector2f(BOX_WIDTH, BOX_HEIGHT));
        box.setPosition(node.position);
        box.setFillColor(sf::Color::Transparent);
        box.setOutlineColor(node.color);
        box.setOutlineThickness(2.f);

        sf::Text valueText(std::format("{}", node.value), m_font, FONT_SIZE);
        valueText.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = valueText.getLocalBounds();
        valueText.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        valueText.setPosition(node.position.x + BOX_WIDTH / 2.f, node.position.y + BOX_HEIGHT / 2.f);

        sf::Text indexText(std::format("[{}]", i), m_font, FONT_SIZE - 8);
        indexText.setFillColor(sf::Color(180, 180, 180));
        sf::FloatRect indexBounds = indexText.getLocalBounds();
        indexText.setOrigin(indexBounds.left + indexBounds.width / 2.f, indexBounds.top);
        indexText.setPosition(getPositionForIndex(i).x + BOX_WIDTH / 2.f, m_position.y + BOX_HEIGHT + 5);

        window.draw(box);
        window.draw(valueText);
        window.draw(indexText);
    }
}
