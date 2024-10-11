#pragma once
#include <CAMeta/CAMetaProps.h>
#include <TestHeaderInInclude.h>

namespace TestNameSpaceB
{
    using namespace TestNameSpaceA;
    class CAMETA(MetaDataInInclude(1, 3.0f)) TestProp : public BaseClassTest
    {
    public:
        int x;
        int y;
        struct InnerStruct
        {
            int a;
            int b;
        };
    };
}