#pragma once

#include <streambuf>
#include <sstream>

namespace xpp::util
{
    class NullBuffer : public std::streambuf
    {
        public:
            inline int overflow(int c) 
            { 
                return c; 
            }
    };

    inline void copy_stream(std::ostream& out, std::istream& in)
    {
        std::stringstream temp;
        temp << in.rdbuf();
        out << temp.str();
    }
}
