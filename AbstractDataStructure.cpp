#include "AbstractDataStructure.h"
#include "Visualizer.h"

void AbstractDataStructure::notify() {
  if (m_visualizer) {
      m_visualizer->render(m_elements);
  }
}
