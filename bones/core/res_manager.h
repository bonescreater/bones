#ifndef BONES_CORE_RES_MANAGER_H_
#define BONES_CORE_RES_MANAGER_H_

#include "core.h"
#include "pixmap.h"
#include <string>
#include <map>

namespace bones
{

class ResManager
{
public:
    void addPixmap(const char * key, Pixmap & pm);

    void clean();

    Pixmap * getPixmap(const char * key);
private:
    std::map<std::string, Pixmap> key2pm_;
};

}


#endif