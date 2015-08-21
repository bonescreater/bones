
#include "bones/bones.h"
#include <Windows.h>
#include <stdio.h>

//bool TestCB(BonesCObject co, BonesArg * arg, size_t arg_count, void * userdata)
//{
//    BonesUnregScriptCallback(co, "onClick");
//    return true;
//}
//
//bool TestClose(BonesCObject co, BonesArg * arg, size_t arg_count, void * userdata)
//{
//    BonesCleanXML();
//    ::PostQuitMessage(0);
//    return true;
//}

class LoadListener : public BonesXMLListener
{
public:
    bool onLoad(BonesCore *) override
    {
        return true;
    }
};
//int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int main()
{
    BonesConfig config;
    config.log_file = L"./test.log";
    config.log_level = kBONES_LOG_LEVEL_VERBOSE;

    BonesStartUp(config);

    auto f = fopen("..\\..\\sample\\test.xml", "rb");
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    char * xml = (char *)malloc(len + 1);
    fseek(f, 0, SEEK_SET);
    fread(xml, 1, len, f);
    fclose(f);
    xml[len] = '\0';
    LoadListener load;
    BonesGetCore()->loadXMLString(xml, &load);
    free(xml);

    //BonesCleanXML();
    auto close_btn = BonesGetCore()->getObject("close");
    if (close_btn)
        close_btn->listen("onClick", nullptr);
        

    //消息循环
    MSG msg;
    while (1)
    {
        if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message != WM_QUIT)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                
                break;
            }                
        }
        else
        {
            BonesUpdate();
            ::SleepEx(5, true);
        }

    }
    BonesShutDown();
    //getchar();
    return 0;
}
