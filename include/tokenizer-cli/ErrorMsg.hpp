#pragma once

#include <string>
#include <sstream>

namespace tokenizer_cli
{
    class ErrorMsg
    {
        std::ostringstream stream;
        std::string errFlag = "<# ";

        public:

            template <typename T>
            ErrorMsg& operator<<(const T& value)
            {
                stream << value;
                return *this;
            }

            ErrorMsg& operator<<(std::ostringstream& (*manip)(std::ostream&))
            {
                stream << manip;
                return *this;
            }

            std::string get() const { return errFlag + stream.str() + '\n'; }
    };
}