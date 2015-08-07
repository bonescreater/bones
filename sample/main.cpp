
#include "bones/bones_api.h"
#include <Windows.h>
#include <stdio.h>

bool TestCB(BonesCObject co, BonesArg * arg, size_t arg_count, void * userdata)
{
    BonesUnregScriptCallback(co, "onClick");
    return true;
}

bool TestClose(BonesCObject co, BonesArg * arg, size_t arg_count, void * userdata)
{
    BonesCleanXML();
    ::PostQuitMessage(0);
    return true;
}
//int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int main()
{
    BonesConfig config;
    config.log_file = L"./test.log";
    config.log_level = kBONES_LOG_LEVEL_VERBOSE;

    BonesStartUp(config);
    wprintf(L"%s", BonesGetVersion());

    auto f = fopen("..\\..\\sample\\test.xml", "rb");
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    char * xml = (char *)malloc(len + 1);
    fseek(f, 0, SEEK_SET);
    fread(xml, 1, len, f);
    fclose(f);
    xml[len] = '\0';
    BonesLoadXMLString(xml);
    free(xml);

    //BonesCleanXML();
    auto close_btn = BonesGetCObjectByID("close");
    if (close_btn)
        BonesRegScriptCallback(close_btn, "onClick", &TestClose, close_btn);
        

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
