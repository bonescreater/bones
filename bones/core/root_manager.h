#ifndef BONES_CORE_ROOT_MANAGER_H_
#define BONES_CORE_ROOT_MANAGER_H_


#include "root.h"
#include <vector>

namespace bones
{
class Root;

class RootManager
{
public:
    ~RootManager();

    Root * getRoot(int index) const;

    size_t getRootCount() const;

    void add(Root * root);

    void remove(Root * root);

    void remove();

    void update();
private:
    std::vector<RefPtr<Root>> roots_;
};

}
#endif