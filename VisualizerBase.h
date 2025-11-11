#pragma once
#include "AnimationStep.h"
#include "Command.h"
#include <deque>
#include <memory>
#include <string>
#include <functional>

class IVisualizer {
public:
    virtual ~IVisualizer() = default;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual const std::deque<Command>& getOperationQueue() const = 0;
};

class VisualizerBase : public IVisualizer {
public:
    VisualizerBase() = default;
    virtual ~VisualizerBase() = default;

    void update(float dt) override;
    const std::deque<Command>& getOperationQueue() const override { return m_operationQueue; }

    virtual void draw(sf::RenderWindow& window) const override = 0;

protected:
    std::vector<VisualNode> m_nodes;
    std::deque<std::unique_ptr<AnimationStep>> m_animationQueue;
    std::deque<Command> m_operationQueue;

    void enqueueAnimation(std::unique_ptr<AnimationStep> step) { m_animationQueue.push_back(std::move(step)); }
    void enqueueOperation(const std::string& description, std::function<void()> action) {
        m_operationQueue.push_back(Command{description, std::move(action)});
    }
    bool isIdle() const { return m_animationQueue.empty(); }
};
