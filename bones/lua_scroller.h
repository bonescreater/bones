#ifndef BONES_LUA_SCROLLER_H_
#define BONES_LUA_SCROLLER_H_

#include "lua_object.h"
#include "core\scroller.h"

namespace bones
{


class LuaScroller : public LuaObject<BonesScroller, BonesScroller::NotifyListener, Scroller>,
                    public Scroller::Delegate
{
public:
    LuaScroller(Scroller * ob);

    ~LuaScroller();

    NotifyListener * getNotify() const override;

    void notifyCreate() override;

    void notifyDestroy() override;

    void createMetaTable(lua_State * l) override;

    void setScrollInfo(BonesScalar total, bool horizontal) override;

    void setScrollPos(BonesScalar cur, bool horizontal) override;

    void onScrollRange(Scroller * sender,
                       Scalar min_pos,
                       Scalar max_pos,
                       Scalar view_port,
                       bool horizontal) override;

    void onScrollPos(Scroller * sender,
                     Scalar cur_pos,
                     bool horizontal) override;

    void setListener(NotifyListener * listener) override;
private:
    NotifyListener * listener_;
};

}


#endif