#pragma once
#include <CAMeta/CAMetaProps.h>
#include <TestHeaderInInclude.h>

namespace TestNameSpaceB
{
    using namespace TestNameSpaceA;

    class TestClass1;
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

        InnerStruct exampleStruct{};

        void TestFunction(int a, float& b, int* c, TType d, TType& e
        , TestClass1 const& f, TestClass1& g, TestClass1* h){}


        TestProp() : x(0), y(0) {} 
        TestProp(TestProp const& other) = default;
        TestProp(int xx, int yy) : x(xx), y(yy) {} 
        //bool TestFunction1(double aa){ return true;};
        TestProp Clone() const { return *this; }
        InnerStruct Inner() const { return InnerStruct(); }
        TestClass1 GenTestClass1() const;
        private:

    };

    class CAMETA(CodeGen) TestClass1
    {
        public:
        int c1x;
    };

}