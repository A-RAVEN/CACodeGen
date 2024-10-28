#pragma once
#include <vector>
#include <map>
#include <iostream>
class ICodeGenerator;
class GenerateManager
{
public:
    template<typename T>
    class RegisterGenerator
    {
    public:
        RegisterGenerator(const char* Name)
        {
            std::cout << "Register New Generator" << Name << std::endl;
            GenerateManager::GetInstance()->AddGenerator(Name, new T());
        }
    };
    static std::map<std::string, ICodeGenerator*> const& GetGenerators() { return GetInstance()->m_Generators; }
private:
    static GenerateManager* GetInstance();
    void AddGenerator(const char* name, ICodeGenerator* generator);
    std::map<std::string, ICodeGenerator*> m_Generators;
};

#define CA_STATIC_GENERATOR(GeneratorClass) static GenerateManager::RegisterGenerator<GeneratorClass> s_##GeneratorClass##_GodeGenRegister = GenerateManager::RegisterGenerator<GeneratorClass>(#GeneratorClass);