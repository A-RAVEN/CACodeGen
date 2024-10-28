#include "GeneratorManager.h"


void GenerateManager::AddGenerator(const char* name, ICodeGenerator* generator)
{
    m_Generators.insert(std::make_pair(std::string(name), generator));
}

GenerateManager* GenerateManager::GetInstance()
{
    static GenerateManager singleton;
    return &singleton;
}
