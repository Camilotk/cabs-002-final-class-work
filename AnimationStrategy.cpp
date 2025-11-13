#include "AnimationStrategy.h"
#include <algorithm>

std::vector<std::unique_ptr<AnimationStep>> SmoothAnimationStrategy::animate(const std::vector<int>& newState,
                                                                             const std::vector<VisualNode>& currentNodes) {
    std::vector<std::unique_ptr<AnimationStep>> steps;
    size_t minSize = std::min(newState.size(), currentNodes.size());
    for (size_t i = 0; i < minSize; ++i) {
        if (newState[i] != currentNodes[i].value) {
            steps.push_back(std::make_unique<ColorStep>(i, sf::Color::Yellow, 0.15f));
            steps.push_back(std::make_unique<ColorStep>(i, sf::Color::Cyan, 0.25f));
        }
    }
    for (size_t i = currentNodes.size(); i < newState.size(); ++i) {
        sf::Vector2f startPos = { currentNodes.empty() ? 50.f + static_cast<float>(i) * 70.f : currentNodes.back().position.x + 70.f,
                                  currentNodes.empty() ? 150.f : currentNodes.back().position.y - 100.f };
        steps.push_back(std::make_unique<DataInsertStep>(newState[i], i, startPos));
        steps.push_back(std::make_unique<MoveStep>(i, sf::Vector2f(currentNodes.empty() ? 50.f : currentNodes.back().position.x + 70.f * (i - currentNodes.size() + 1), currentNodes.empty() ? 150.f : currentNodes.back().position.y)));
        steps.push_back(std::make_unique<ColorStep>(i, sf::Color::Cyan));
    }
    if (newState.size() < currentNodes.size()) {
        for (size_t i = newState.size(); i < currentNodes.size(); ++i) {
            steps.push_back(std::make_unique<ColorStep>(i, sf::Color::Red, 0.2f));
            steps.push_back(std::make_unique<DataRemoveStep>(i));
        }
    }
    return steps;
}

std::vector<std::unique_ptr<AnimationStep>> StepByStepAnimationStrategy::animate(const std::vector<int>& newState,
                                                                                 const std::vector<VisualNode>& currentNodes) {
    std::vector<std::unique_ptr<AnimationStep>> steps;
    size_t minSize = std::min(newState.size(), currentNodes.size());
    for (size_t i = 0; i < minSize; ++i) {
        if (newState[i] != currentNodes[i].value) {
            steps.push_back(std::make_unique<ColorStep>(i, sf::Color::Red, 0.2f));
            steps.push_back(std::make_unique<ColorStep>(i, sf::Color::Cyan, 0.2f));
        }
    }
    for (size_t i = currentNodes.size(); i < newState.size(); ++i) {
        steps.push_back(std::make_unique<DataInsertStep>(newState[i], i, sf::Vector2f(50.f + i * 70.f, 50.f)));
        steps.push_back(std::make_unique<ColorStep>(i, sf::Color::Cyan));
    }
    if (newState.size() < currentNodes.size()) {
        for (size_t i = newState.size(); i < currentNodes.size(); ++i) {
            steps.push_back(std::make_unique<DataRemoveStep>(i));
        }
    }
    return steps;
}
