#include "Generator.h"
#include <LanguageTypes/CodeInfoContainer.h>

void ICodeGenerator::GenerateCode(CodeInfoContainer const& codeInfoContainer)
{
    for(auto const& pclassInfo : codeInfoContainer.GetAllClasses())
    {
        auto pCodeGenAttribute = pclassInfo->getMetaData().GetPropertyInfo("CodeGen");
        if(pCodeGenAttribute)
        {
            pclassInfo->getCurosr().
            std::cout << "Generating code for class: " << pclassInfo->getCurosr().getDisplayName() << std::endl;
        }
    }
}
