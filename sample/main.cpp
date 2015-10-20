
#include "bones/bones.h"

#include <stdio.h>
#include <assert.h>
#include <vector>
#include "panel.h"
#include "utils.h"

std::ostringstream BSStr::s;
BSPanel * test_window;

void ReadFile(const char * path, std::vector<char> &stream)
{
    auto f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    stream.resize(len);
    fseek(f, 0, SEEK_SET);
    fread(&stream[0], 1, len, f);
    fclose(f);
}

//响应 关闭按钮
class Close : public BonesObject::ScriptListener
{
public:
    void onEvent(BonesObject * ob, BonesObject::ScriptArg * arg, size_t arg_count,
        BonesObject::ScriptArg ** ret, size_t ret_count) override
    {
        test_window->destroy();
        BonesGetCore()->cleanXML();
        //没有返回值
        *ret = nullptr;
        ret_count = 0;
    }
};

Close close;

class LoadListener : public BonesXMLListener
{
public:
    bool onLoad(BonesCore * core) override
    {
        test_window->loadRoot(static_cast<BonesRoot *>(core->getObject("main")));

        //往关闭按钮注册回调
        //测试程序的按钮 会调用"onClick"
        //((BonesShape *)core->getObject("close"))->listen("onClick", &close);

        //BSStr s;
        //s.reset();
        //s << "{";
        //s << "content:pic_lt;";
        //s << "}";
        //((BonesImage *)core->getObject("pic_lt"))->applyCSS(s.str().data());

        //s.reset();
        //s << "{";
        //s << "content:pic_rt;";
        //s << "}";
        //((BonesImage *)core->getObject("pic_rt"))->applyCSS(s.str().data());

        //s.reset();
        //s << "{";
        //s << "content:pic_lb;";
        //s << "}";
        //((BonesImage *)core->getObject("pic_lb"))->applyCSS(s.str().data());

        //s.reset();
        //s << "{";
        //s << "content:pic_rb;";
        //s << "}";
        //((BonesImage *)core->getObject("pic_rb"))->applyCSS(s.str().data());

        return true;
    }

    void onUnload(BonesCore *) override
    {
        ;
    }

    void onPrepare(BonesObject *) override
    {//设置各种标签C++回调

    }
};

int main()
{
    BSPanel::Initialize();

    //创建窗口
    test_window = new BSPanel;
    auto bret = test_window->create(nullptr, false);
    if (!bret)
        return 1;

    BonesConfig config;
    config.log_level = BonesConfig::kVerbose;
    config.aa_enable = true;
    config.cef_enable = false;
    config.cef_locate = "zh-CN";
    BonesStartUp(config);

    LoadListener load;
    BonesGetCore()->setXMLListener(&load);
    BonesGetCore()->loadXMLFile(L"test.xml");
    POINT pt = { 0, 0 };
    SIZE size = { 1024, 768 };
    test_window->setLoc(pt);
    test_window->setSize(size);

    ::ShowWindow(test_window->hwnd(), SW_NORMAL);
    //::UpdateWindow(test_window->hwnd());
    DWORD ret;
    MSG msg;
    while (1)
    {
        ret = ::MsgWaitForMultipleObjectsEx(0, nullptr, 10, 
            QS_ALLINPUT, MWMO_INPUTAVAILABLE | MWMO_ALERTABLE);

        if (WAIT_OBJECT_0 == ret || WAIT_IO_COMPLETION == ret)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (WM_QUIT == msg.message)
                {
                    BonesGetCore()->cleanXML();
                    break;
                }                    
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        BonesUpdate();
    }
    
    BonesGetCore()->getResourceManager()->clean();
    BonesGetCore()->setXMLListener(nullptr);

    BonesShutDown();
    delete test_window;
    BSPanel::Uninitialize();
    //getchar();
    return 0;
}

