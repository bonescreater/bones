
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
class Close : public BonesScriptListener
{
public:
    int onEvent(BonesObject * ob, BonesScriptArg * arg, size_t arg_count)
    {
        test_window->destroy();
        BonesGetCore()->cleanXML();
        return 0;
    }
};

Close close;

class LoadListener : public BonesXMLListener
{
public:
    bool onLoad(BonesCore *) override
    {
        test_window->loadRoot("main");
        test_window->loadRichEdit("rich");
        test_window->loadRichEdit("rich_opacity");
        //往关闭按钮注册回调
        //测试程序的按钮 会调用"onClick"
        ((BonesArea *)BonesGetCore()->getObject("close"))->listen("onClick", &close);
        BSStr s;
        s.reset();
        s << "{";
        s << "content:pic_lt;";
        s << "}";
        ((BonesImage *)BonesGetCore()->getObject("pic_lt"))->applyCSS(s.str().data());
        s.reset();
        s << "{";
        s << "content:pic_rt;";
        s << "}";
        ((BonesImage *)BonesGetCore()->getObject("pic_rt"))->applyCSS(s.str().data());
        s.reset();
        s << "{";
        s << "content:pic_lb;";
        s << "}";
        ((BonesImage *)BonesGetCore()->getObject("pic_lb"))->applyCSS(s.str().data());
        s.reset();
        s << "{";
        s << "content:pic_rb;";
        s << "}";
        ((BonesImage *)BonesGetCore()->getObject("pic_rb"))->applyCSS(s.str().data());
        return true;
    }

    void onUnload(BonesCore *) override
    {
        ;
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
    config.log_file = L"./test.log";
    config.log_level = kBONES_LOG_LEVEL_VERBOSE;
    config.enable_cef = true;
    BonesStartUp(config);

    std::vector<char> pic;
    ReadFile("..\\..\\sample\\lena.bmp", pic);
    auto pm_pic = BonesGetCore()->createPixmap();
    auto sub_pm_lt = BonesGetCore()->createPixmap();
    auto sub_pm_rt = BonesGetCore()->createPixmap();
    auto sub_pm_lb = BonesGetCore()->createPixmap();
    auto sub_pm_rb = BonesGetCore()->createPixmap();
    pm_pic->decode(&pic[0], pic.size());

    BonesRect rlt = { 0, 0, 128, 128 };
    BonesRect rrt = { 128, 0, 256, 128 };
    BonesRect rlb = { 0, 128, 128, 256 };
    BonesRect rrb = { 128, 128, 256, 256 };
    pm_pic->extractSubset(*sub_pm_lt, rlt);
    pm_pic->extractSubset(*sub_pm_rt, rrt);
    pm_pic->extractSubset(*sub_pm_lb, rlb);
    pm_pic->extractSubset(*sub_pm_rb, rrb);
    //content:key
    BonesGetCore()->getResManager()->clonePixmap("pic_lt", *sub_pm_lt);
    BonesGetCore()->getResManager()->clonePixmap("pic_rt", *sub_pm_rt);
    BonesGetCore()->getResManager()->clonePixmap("pic_lb", *sub_pm_lb);
    BonesGetCore()->getResManager()->clonePixmap("pic_rb", *sub_pm_rb);

    std::vector<char> xml;
    ReadFile("..\\..\\sample\\test.xml", xml);
    xml.push_back(0);//0结尾

    LoadListener load;
    BonesGetCore()->loadXMLString(&xml[0], &load);
    POINT pt = { 0, 0 };
    SIZE size = { 1024, 768 };
    test_window->setLoc(pt);
    test_window->setSize(size);

    ::ShowWindow(test_window->hwnd(), SW_NORMAL);
    //::UpdateWindow(test_window->hwnd());
        
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
            test_window->layeredDraw();
            ::SleepEx(5, true);
        }

    }
    
    BonesGetCore()->getResManager()->clean();
    BonesGetCore()->destroyPixmap(pm_pic);
    BonesGetCore()->destroyPixmap(sub_pm_lt);
    BonesGetCore()->destroyPixmap(sub_pm_rt);
    BonesGetCore()->destroyPixmap(sub_pm_lb);
    BonesGetCore()->destroyPixmap(sub_pm_rb);

    BonesShutDown();
    delete test_window;
    BSPanel::Uninitialize();
    //getchar();
    return 0;
}

