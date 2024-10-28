using System;
using System.Runtime.InteropServices;

public class TestProp
{
    IntPtr handle;

    TestProp()
    {
        handle = New_TestProp();
    }

    TestProp(TestProp src)
    {
        handle = New_TestProp_Copy(src.handle);
    }

    public int x
    {
        get { return Get_TestProp_x(handle); }
        set { Set_TestProp_x(handle, value); }
    }

    public int y
    {
        get { return Get_TestProp_x(handle); }
        set { Set_TestProp_x(handle, value); }
    }


    extern static IntPtr New_TestProp();
    extern static IntPtr New_TestProp_Copy(IntPtr srcHandle);
    extern static void Set_TestProp_x(IntPtr thisHandle, int value);
    extern static int Get_TestProp_x(IntPtr thisHandle);
    extern static void Set_TestProp_y(IntPtr thisHandle, int value);
    extern static int Get_TestProp_y(IntPtr thisHandle);
}