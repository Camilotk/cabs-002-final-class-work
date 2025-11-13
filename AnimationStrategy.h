#pragma once
#include <vector>
#include <memory>
#include "AnimationStep.h"

class AnimationStrategy {
public:
    virtual ~AnimationStrategy() = default;
    virtual std::vector<std::unique_ptr<AnimationStep>> animate(const std::vector<int>& newState,
                                                                const std::vector<VisualNode>& currentNodes) = 0;
};

class SmoothAnimationStrategy : public AnimationStrategy {
public:
    std::vector<std::unique_ptr<AnimationStep>> animate(const std::vector<int>& newState,
                                                        const std::vector<VisualNode>& currentNodes) override;
};

class StepByStepAnimationStrategy : public AnimationStrategy {
public:
    std::vector<std::unique_ptr<AnimationStep>> animate(const std::vector<int>& newState,
                                                        const std::vector<VisualNode>& currentNodes) override;
};
