#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include "CodeGenerators.hpp"

class FileBasedGenerator : public ICodeGenerator
{
protected:
    virtual std::string GetExtension() const = 0;

    std::string GetProblemDir(SolveProblem problem) const
    {
        switch (problem)
        {
        case SolveProblem::SelectionSort:
            return "selection_sort";
        case SolveProblem::ShakerSort:
            return "shaker_sort";
        case SolveProblem::QuickSort:
            return "quick_sort";
        default:
            return "unknown";
        }
    }

    std::string ReadFile(const std::string &filepath) const
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            return "// Error: Unable to find problem file: " + filepath;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

public:
    std::string Generate(SolveProblem problem) const override
    {
        std::string dir = GetProblemDir(problem);
        if (dir == "unknown")
            return "// Error: Unknown algorithm";

        std::string filepath = std::string(SNIPPETS_DIR) + "/" + dir + "/code" + GetExtension();
        return ReadFile(filepath);
    }
};

class PythonGenerator : public FileBasedGenerator
{
protected:
    std::string GetExtension() const override { return ".py"; }
};

class CGenerator : public FileBasedGenerator
{
protected:
    std::string GetExtension() const override { return ".c"; }
};

class CppGenerator : public FileBasedGenerator
{
protected:
    std::string GetExtension() const override { return ".cpp"; }
};

std::unique_ptr<ICodeGenerator> CreateGenerator(SolveLanguage lang)
{
    switch (lang)
    {
    case SolveLanguage::Python:
        return std::make_unique<PythonGenerator>();
    case SolveLanguage::C:
        return std::make_unique<CGenerator>();
    case SolveLanguage::Cpp:
        return std::make_unique<CppGenerator>();
    }
    return nullptr;
}

std::string GetReferenceCode(SolveProblem problem, SolveLanguage lang)
{
    if (auto generator = CreateGenerator(lang))
    {
        return generator->Generate(problem);
    }
    return "// Error: Unknown programming language";
}
