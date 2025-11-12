#pragma once
#include "AbstractDataStructure.h"
#include "Node.h"

class LinkedListStructure : public AbstractDataStructure {
public:
  LinkedListStructure();
  ~LinkedListStructure();
  void insert(size_t index, int value) override;
  void remove(size_t index) override;
  void clear() override;
  std::optional<int> access(size_t index) const override;

private:
  Node *head = nullptr;
  void reconnectNodes();
};
