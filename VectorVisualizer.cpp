#include "VectorVisualizer.h"

VectorVisualizer::VectorVisualizer(sf::Font &font, const sf::Vector2f &position)
    : m_font(font), m_position(position),
      m_layout(std::make_unique<LinearLayoutPolicy>(BOX_WIDTH, SPACING)) {}

void VectorVisualizer::insert(int value, size_t index) {
  std::string desc = "Vetor: Insert(" + std::to_string(value) + ", " +
                     std::to_string(index) + ")";
  enqueueOperation(
      desc, [this, value, index]() { buildInsertAnimation(value, index); });
}

void VectorVisualizer::remove(size_t index) {
  std::string desc = "Vetor: Remove(" + std::to_string(index) + ")";
  enqueueOperation(desc, [this, index]() { buildRemoveAnimation(index); });
}

void VectorVisualizer::clearAnimated() {
  std::string desc = "Vetor: Clear()";
  enqueueOperation(desc, [this]() {
    for (size_t i = 0; i < m_nodes.size(); ++i) {
      enqueueAnimation(
          std::make_unique<ColorStep>(i, sf::Color(255, 100, 100), 0.15f));
    }

    enqueueAnimation(std::make_unique<ClearAllStep>());
  });
}

void VectorVisualizer::buildInsertAnimation(int value, size_t index) {
  if (index > m_nodes.size())
    return;
  size_t oldSize = m_nodes.size();
  sf::Vector2f startPos = {getPositionForIndex(index).x,
                           m_position.y - BOX_HEIGHT * 2};
  enqueueAnimation(std::make_unique<DataInsertStep>(value, index, startPos));
  enqueueAnimation(
      std::make_unique<MoveStep>(index, getPositionForIndex(index)));
  for (size_t i = index + 1; i <= oldSize; ++i) {
    enqueueAnimation(std::make_unique<MoveStep>(i, getPositionForIndex(i)));
  }
  for (size_t i = 0; i <= oldSize; ++i) {
    enqueueAnimation(std::make_unique<ColorStep>(i, sf::Color::Cyan));
  }
}

void VectorVisualizer::buildRemoveAnimation(size_t index) {
  if (index >= m_nodes.size())
    return;

  enqueueAnimation(std::make_unique<ColorStep>(index, sf::Color::Red));
  sf::Vector2f targetPos = {getPositionForIndex(index).x,
                            m_position.y - BOX_HEIGHT * 2};
  enqueueAnimation(std::make_unique<MoveStep>(index, targetPos, 0.4f));
  enqueueAnimation(std::make_unique<DataRemoveStep>(index));
  for (size_t i = index; i < m_nodes.size();
       ++i) { // O size já está menor aqui para o loop
    enqueueAnimation(std::make_unique<MoveStep>(i, getPositionForIndex(i)));
  }
}

sf::Vector2f VectorVisualizer::getPositionForIndex(size_t i) const {
  if (m_layout)
    return m_layout->positionForIndex(i, m_position);
  return {m_position.x + i * (BOX_WIDTH + SPACING), m_position.y};
}

void VectorVisualizer::reflow(float windowWidth, float panelWidth) {
  if (!isIdle())
    return;
  bool widthChanged = (windowWidth != m_lastLayoutWidth);
  bool countChanged = (m_lastNodeCount != m_nodes.size());
  if (!widthChanged && !countChanged)
    return;
  m_lastLayoutWidth = windowWidth;
  m_lastNodeCount = m_nodes.size();

  float margin = 40.f;
  float available =
      std::max(180.f, windowWidth - panelWidth - m_position.x - margin);
  float stride = BOX_WIDTH + SPACING;
  int maxCols = static_cast<int>(available / stride);
  if (maxCols < 1)
    maxCols = 1;
  float rowHeight = BOX_HEIGHT + 65.f;

  for (size_t i = 0; i < m_nodes.size(); ++i) {
    int row = static_cast<int>(i / maxCols);
    int col = static_cast<int>(i % maxCols);
    float x = m_position.x + col * stride;
    float y = m_position.y + row * rowHeight;
    m_nodes[i].position = {x, y};
  }
}

void VectorVisualizer::draw(sf::RenderWindow &window) const {
  sf::Text title("std::vector (Array List)", m_font, 20);
  title.setPosition(m_position.x, m_position.y - 40);
  title.setFillColor(sf::Color::White);
  window.draw(title);

  for (size_t i = 0; i < m_nodes.size(); ++i) {
    const auto &node = m_nodes[i];
    sf::RectangleShape box(sf::Vector2f(BOX_WIDTH, BOX_HEIGHT));
    box.setPosition(node.position);
    box.setFillColor(sf::Color::Transparent);
    box.setOutlineColor(node.color);
    box.setOutlineThickness(2.f);

    sf::Text valueText(std::to_string(node.value), m_font, FONT_SIZE);
    valueText.setFillColor(sf::Color::White);
    sf::FloatRect textBounds = valueText.getLocalBounds();
    valueText.setOrigin(textBounds.left + textBounds.width / 2.f,
                        textBounds.top + textBounds.height / 2.f);
    valueText.setPosition(node.position.x + BOX_WIDTH / 2.f,
                          node.position.y + BOX_HEIGHT / 2.f);

    sf::Text indexText("[" + std::to_string(i) + "]", m_font, FONT_SIZE - 8);
    indexText.setFillColor(sf::Color(180, 180, 180));
    sf::FloatRect indexBounds = indexText.getLocalBounds();
    indexText.setOrigin(indexBounds.left + indexBounds.width / 2.f,
                        indexBounds.top);
    indexText.setPosition(getPositionForIndex(i).x + BOX_WIDTH / 2.f,
                          m_position.y + BOX_HEIGHT + 5);

    window.draw(box);
    window.draw(valueText);
    window.draw(indexText);
  }
}
