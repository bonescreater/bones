#include "lua_rich_edit.h"
#include "lua_context.h"
#include "lua_check.h"
#include "lua_meta_table.h"
#include "core/rich_edit.h"

namespace bones
{

static const char * kMetaTableRichEdit = "__mt_richedit";

LuaRichEdit::LuaRichEdit(RichEdit * ob)
:LuaObject(ob)
{
    LuaMetaTable::CreatLuaTable(LuaContext::State(), kMetaTableRichEdit, this);
}

}