#include "LinkedListStructureDS.h"

LinkedListStructure::LinkedListStructure() {
    insert(0, 77); insert(1, 88); insert(2, 99);
}

LinkedListStructure::~LinkedListStructure() {
    Node* cur = head; while (cur) { Node* nxt = cur->next; delete cur; cur = nxt; }
}

void LinkedListStructure::insert(size_t index, int value) {
    Node* newNode = new Node{value};
    if (index == 0 || !head) {
        newNode->next = head; head = newNode;
    } else {
        Node* cur = head; size_t i=0; while (cur->next && i+1 < index) { cur = cur->next; ++i; }
        newNode->next = cur->next; cur->next = newNode;
    }
    reconnectNodes();
    notify();
}

void LinkedListStructure::remove(size_t index) {
    if (!head) return;
    if (index == 0) {
        Node* old = head; head = head->next; delete old;
    } else {
        Node* cur = head; size_t i=0; while (cur->next && i+1 < index) { cur = cur->next; ++i; }
        if (cur->next) { Node* old = cur->next; cur->next = old->next; delete old; }
    }
    reconnectNodes();
    notify();
}

std::optional<int> LinkedListStructure::access(size_t index) const {
    Node* cur = head; size_t i=0; while (cur && i < index) { cur = cur->next; ++i; }
    if (!cur)
        return std::nullopt;
    return cur->value;
}

void LinkedListStructure::reconnectNodes() {
    m_elements.clear();
    Node* cur = head; while (cur) { m_elements.push_back(cur->value); cur = cur->next; }
}
