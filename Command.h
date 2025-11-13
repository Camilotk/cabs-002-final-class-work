#pragma once
#include <string>
#include <functional>

struct Command {
    std::string description;
    std::function<void()> action;
};