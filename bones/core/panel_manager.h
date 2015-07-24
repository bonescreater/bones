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
private:
    std::list<RefPtr<Panel>> panels_;
};

}


#endif