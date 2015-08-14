#include "css_manager.h"
#include "logging.h"
#include "ref.h"
#include "view.h"
#include "widget.h"

namespace bones
{

CSSString::CSSString()
    :begin(nullptr), length(0)
{

}

CSSString::CSSString(const char * b)
    : begin(b), length(0)
{
    if (begin)
        length = strlen(begin);
}

CSSString::CSSString(const char * b, size_t l)
    :begin(b), length(l)
{

}

CSSString::operator bool() const
{
    return begin != nullptr && length != 0;
}

bool CSSString::operator<(const CSSString & right) const
{
    size_t n = this->length;
    if (n > right.length)
        n = right.length;
    int re = strncmp(this->begin, right.begin, n);
    if (re == 0)
    {//前n个字符相等
        if (n != this->length)
            re = 1;
        else if (n != right.length)
            re = -1;
    }
    return re < 0;
}

bool CSSString::operator==(const CSSString & right) const
{
    if (this->length == right.length)
    {
        if (0 == strncmp(this->begin, right.begin, this->length))
            return true;
    }
    return false;
}

class CSSClassName
{
public:
    CSSClassName(const char * class_name)
    {
        assert(class_name);
        class_name_ = class_name;
        length_ = strlen(class_name);
        current_ = 0;
    }

    const char * next(size_t & next_size)
    {
        if (current_ >= length_)
            return nullptr;

        //跳过空白字符
        while (0 != isspace(class_name_[current_]))
        {
            current_++;
            if (current_ >= length_)
                return nullptr;
        }
        auto cur = current_;
        auto ptr = class_name_ + cur;
        while (0 == isspace(class_name_[current_]))
        {
            current_++;
            if (current_ >= length_)
                break;
        }
        next_size = current_ - cur;
        return ptr;
    }
private:
    const char * class_name_;
    size_t length_;
    size_t current_;
};

enum Token
{
    kTOKEN_END = '\0',
    kTOKEN_POINT = '.',
    kTOKEN_STRING,
    kTOKEN_TEXT,
    kTOKEN_OPEN_BRACE = '{',
    kTOKEN_CLOSE_BRACE = '}',
    kTOKEN_COLON = ':',
    kTOKEN_SEMICOLON = ';',
    kTOKEN_R = '\r',
    kTOKEN_N = '\n',
};

Token ReadToken(const char * str, size_t & begin, size_t & end);
//{left:xxx yyy zzz; top:mmm}
const char * SkipBOM(const char * str)
{
    if (strlen(str) < 3)
        return str;

    if (static_cast<unsigned char>(str[0]) == 0xEF &&
        static_cast<unsigned char>(str[1]) == 0xBB &&
        static_cast<unsigned char>(str[2]) == 0xBF)
    {
        return str + 3;      // Skup utf-8 bom
    }
    return str;
}

void CSSManager::parse(const char * css, CSSEntries & entries)
{
    size_t len = 0;
    if (!css || !(len = strlen(css)) )
        return;

    size_t begin = 0;
    size_t end = 0;

    
    auto token = kTOKEN_STRING;
    token = ReadToken(css, begin, end);
    if (token != kTOKEN_OPEN_BRACE)
        goto error;
    for (;;)
    {
        //attr left
        css = css + end;
        token = ReadToken(css, begin, end);
        //一个样式读取完毕
        if (kTOKEN_CLOSE_BRACE == token)
            break;
        if (token != kTOKEN_STRING)
            goto error;
        //得到key
        CSSString item;
        item.begin = css + begin;
        item.length = end - begin;
        CSSEntry entry;
        entry.first = item;
        //:
        css += end;
        token = ReadToken(css, begin, end);
        if (kTOKEN_COLON != token)
            goto error;

        //解析出1条属性
        for (;;)
        {
            css += end;
            token = ReadToken(css, begin, end);
            if (kTOKEN_SEMICOLON == token)
                break;
            if (kTOKEN_STRING != token && kTOKEN_TEXT != token)
                goto error;
            CSSString item;
            item.begin = css + begin;
            item.length = end - begin;
            if (kTOKEN_TEXT == token)
            {
                item.begin++;
                item.length -= 2;
            }
            entry.second.push_back(item);
        }
        entries.push_back(entry);
    }
    return;
error:
    LOG_VERBOSE << "css parse error: " << token;
}

CSSManager::CSSManager()
{
    ;
}

void CSSManager::clean()
{
    styles_.clear();
}

void CSSManager::applyCSS(Ref * ob, const char * css)
{
    if (!ob || !css)
        return;
    CSSEntries entries;
    parse(css, entries);
    applyEntries(ob, entries);
}

void CSSManager::applyClass(Ref * ob, const char * class_name)
{
    if (!class_name || !ob)
        return;
    CSSClassName cn(class_name);
    CSSString str;
    while (str.begin = cn.next(str.length))
    {
        auto iter = styles_.find(str);
        if (iter == styles_.end())
            return;

        applyEntries(ob, iter->second);
    }
}

void CSSManager::append(const char * str)
{
    size_t len = 0;

    if (!str || !(len = strlen(str)))
        return;
    auto data = SkipBOM(str);
    len -= data - str;

    auto token = kTOKEN_STRING;
    size_t begin = 0;
    size_t end = 0;

    for (;;)
    {
        data += end;
        token = ReadToken(data, begin, end);
        if (kTOKEN_END == token)
            break;
        if (kTOKEN_POINT != token)
            goto error;
        //得到1个点
        data += end;
        token = ReadToken(data, begin, end);
        if (token != kTOKEN_STRING)
            goto error;
        //插入1个style
        CSSString class_name = { data + begin, end - begin };
        CSSEntries entries;
        //{
        data += end;
        token = ReadToken(data, begin, end);
        if (token != kTOKEN_OPEN_BRACE)
            goto error;

        for (;;)
        {
            //attr left
            data += end;
            token = ReadToken(data, begin, end);
            //一个样式读取完毕
            if (kTOKEN_CLOSE_BRACE == token)
                break;
            if (token != kTOKEN_STRING)
                goto error;
            CSSString item;
            item.begin = data + begin;
            item.length = end - begin;
            entries.push_back(CSSEntry(item, CSSParams()));
            auto & entry = entries.back().second;
            //:
            data += end;
            token = ReadToken(data, begin, end);
            if (kTOKEN_COLON != token)
                goto error;
            //解析出1条属性
            for (;;)
            {
                data += end;
                token = ReadToken(data, begin, end);
                if (kTOKEN_SEMICOLON == token)
                    break;
                if (kTOKEN_STRING != token && kTOKEN_TEXT != token)
                    goto error;
                CSSString item;
                item.begin = data + begin;
                item.length = end - begin;
                if (kTOKEN_TEXT == token)
                {
                    item.begin++;
                    item.length -= 2;
                }
                entry.push_back(item);
            }
        }
        styles_[class_name].swap(entries);
    }
    return;
error:
    LOG_VERBOSE << "css parse error: " << token;
}

void CSSManager::applyEntries(Ref * ob, const CSSEntries & entries)
{
    const CSSClassTables * tables = nullptr;

    if (kClassPanel == ob->getClassName())
        tables = ((Widget *)ob)->getCSSClassTables();
    else//view
        tables = ((View *)ob)->getCSSClassTables();

    for (auto iter = entries.begin(); iter != entries.end(); ++iter)
    {
        auto tmp_tables = tables;
        while (tmp_tables)
        {
            auto & the_table = tmp_tables->table;
            if (!the_table)
                continue;
            auto it = the_table->find(iter->first);
            if (it != the_table->end())
            {//找到
                (ob->*it->second)(iter->second);
                break;
            }
            if (tmp_tables->base)
                tmp_tables = (*tmp_tables->base)();
            else
                break;
        }
    }

}

//0结尾字符串
Token ReadToken(const char * str, size_t & begin, size_t & end)
{
    //检验参数
    begin = 0;
    end = 0;
    if (!str)
        return kTOKEN_END;

    size_t len = strlen(str);
    if (len == 0)
        return kTOKEN_END;

    //跳过开头的空白字符
    unsigned char cur = 0;
    while (0 != isspace(cur = str[begin]))
    {
        if (begin >= len)
        {
            end = len;
            return kTOKEN_END;
        }
        begin++;
    }
    switch (cur)
    {
    case kTOKEN_END://rapidxml 默认解析会有0结尾的字符串
    case kTOKEN_POINT:
    case kTOKEN_OPEN_BRACE:
    case kTOKEN_CLOSE_BRACE:
    case kTOKEN_COLON:
    case kTOKEN_SEMICOLON:
        end = begin + 1;
        return static_cast<Token>(cur);
    default:
        break;
    }

    end = begin;

    unsigned char next = str[end];
    if ('"' == next)
    {
        end++;//调到下一个有效字符
        bool find_text = false;
        while (len > end)
        {
            if (find_text)
                return kTOKEN_TEXT;

            next = str[end];
            if ('\n' == next)
                break;
            if ('"' == next)
                find_text = true;
                
            if ('\\' == next)
            {//碰到转义字符
                next = str[end + 1];
                //下一个字符是字符串结束标志或者转义字符则忽略
                if ('"' == next || '\\' == next)
                    end++;
            }

            end++;
        }
        //错误
        LOG_VERBOSE << "read token fail: " << str << "\n";
        return kTOKEN_END;
    }
    //parser string
    while (len > end)
    {
        //isspace 里有对参数检测 char next ->int 时 碰到中文 int为负数 会assert
        next = str[end];
        if (0 != isspace(next) ||
            kTOKEN_END == next ||
            //kTOKEN_POINT == next ||
            kTOKEN_OPEN_BRACE == next ||
            kTOKEN_CLOSE_BRACE == next ||
            kTOKEN_COLON == next ||
            kTOKEN_SEMICOLON == next)
        {
            break;
        }
        end++;
    }
    return kTOKEN_STRING;
}

}