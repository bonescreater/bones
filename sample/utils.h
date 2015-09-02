#ifndef BONES_SAMPLE_UTILS_H_
#define BONES_SAMPLE_UTILS_H_

#include <sstream>


class BSStr
{
public:
    BSStr()
    {
        reset();
    }

    void reset()
    {
        s.clear();
        s.str("");
    }
    template<typename T>
    BSStr & operator<<(const T & t)
    {
        s << t;
        return *this;
    }
    std::string str()
    {
        return s.str();
    }
private:
    static std::ostringstream s;
};

#endif