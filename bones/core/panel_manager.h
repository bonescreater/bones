#ifndef BONES_CORE_PANEL_MANAGER_H_
#define BONES_CORE_PANEL_MANAGER_H_


#include "panel.h"
#include <list>

namespace bones
{
class Panel;

class PanelManager
{
public:
    void add(Panel * panel);

    void remove(Panel * panel);

    void update();

    Panel * begin();

    Panel * next();
private:
    std::list<RefPtr<Panel>> panels_;
    std::list<RefPtr<Panel>>::iterator iter_;
};

}


#endif