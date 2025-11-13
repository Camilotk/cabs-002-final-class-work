#pragma once
#include <string>
#include <memory>
#include "AbstractDataStructure.h"

class StructureFactory {
public:
    std::unique_ptr<AbstractDataStructure> create(const std::string& type);
};
