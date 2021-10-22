#ifndef HEADER_gubg_data_Loader_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Loader_hpp_ALREADY_INCLUDED

#include <gubg/data/Table.hpp>
#include <gubg/mss.hpp>
#include <gubg/Strange.hpp>
#include <gubg/file/system.hpp>
#include <sstream>

namespace gubg { namespace data { 

    class Loader
    {
    public:
        Loader &delimiter(char ch)
        {
            delimiter_ = ch;
            return *this;
        }
        Loader &has_fieldnames(bool b)
        {
            has_fieldnames_ = b;
            return *this;
        }

        template <typename Value>
        bool operator()(Table<Value> &table, const std::string &filename)
        {
            MSS_BEGIN(bool, "");

            std::string content;
            MSS(file::read(content, gubg::file::Name{filename}));
            Strange strange(std::move(content));

            Strange line;
            bool names_are_set = false;
            if (has_fieldnames_)
            {
                MSS(strange.pop_line(line));
                std::string name;
                while (line.pop_until(name, delimiter_) || line.pop_all(name))
                {
                    MSS(table.add_field(name));
                }
                names_are_set = true;
            }

            gubg::Strange value;
            while (strange.pop_line(line))
            {
                if (!names_are_set)
                {
                    Strange dup = line;
                    std::string dummy;
                    std::ostringstream oss;
                    for (unsigned int cix = 0; dup.pop_until(dummy, delimiter_) || dup.pop_all(dummy); ++cix)
                    {
                        oss.str("");
                        oss << "field_" << cix;
                        MSS(table.add_field(oss.str()));
                    }
                    names_are_set = true;
                }

                auto &row = table.add_row();
                for (unsigned int cix = 0; line.pop_until(value, delimiter_) || line.pop_all(value); ++cix)
                {
                    MSS(Traits<Value>::convert(row[cix], value));
                }
            }
            MSS_END();
        }

    private:
        char delimiter_ = '\t';
        bool has_fieldnames_ = true;
    };

} } 

#endif
