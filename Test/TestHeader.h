#pragma once
#include <CAMeta/CAMetaProps.h>
#include <TestHeaderInInclude.h>

namespace TestNameSpaceB
{
    using namespace TestNameSpaceA;

    class TestClass1
    {
        public:
        int c1x;
    };

    class CAMETA(MetaDataInInclude(1, 3.0f), CodeGen) TestProp : public BaseClassTest
    {
    public:
        int x;
        int y;
        struct InnerStruct
        {
            int a;
            int b;
        };
        using TType = int;
        void TestFunction(int a, float& b, int* c, TType d, TType& e
        , TestClass1 f, TestClass1& g, TestClass1* h){}


        bool TestFunction1(double aa){ return true;};
    };
}