#pragma once

#include <string>
#include <memory>
#include "Types.hpp"

class ICodeGenerator
{
public:
    virtual ~ICodeGenerator() = default;
    virtual std::string Generate(SolveProblem problem) const = 0;
};
