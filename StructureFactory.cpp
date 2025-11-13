#include "StructureFactory.h"
#include "ArrayStructure.h"
#include "ArrayListStructure.h"
#include "LinkedListStructureDS.h"

std::unique_ptr<AbstractDataStructure> StructureFactory::create(const std::string& type) {
    if (type == "array") return std::make_unique<ArrayStructure>();
    if (type == "array_list") return std::make_unique<ArrayListStructure>();
    if (type == "linked_list") return std::make_unique<LinkedListStructure>();
    return nullptr;
}
