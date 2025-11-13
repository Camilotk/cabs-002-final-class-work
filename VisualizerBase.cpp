#include "VisualizerBase.h"

void VisualizerBase::update(float dt) {
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
