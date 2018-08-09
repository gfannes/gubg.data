#ifndef HEADER_gubg_data_Set_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Set_hpp_ALREADY_INCLUDED

#include "gubg/data/Field.hpp"
#include "gubg/data/Record.hpp"
#include "gubg/naft/Range.hpp"
#include "gubg/naft/Node.hpp"
#include "gubg/mss.hpp"
#include "gubg/Range.hpp"
#include <numeric>

namespace gubg { namespace data { 

    template <typename T>
    class Set
    {
    private:
        using Self = Set<T>;

    public:
        using Records = data::Records<T>;

        std::string name;
        Fields fields;
        Records records;

        void write(naft::Node &n) const
        {
            n.attr("name", name);
            {
                auto fs = n.node("fields");
                data::write(fs, fields);
            }
            {
                auto rs = n.node("records");
                data::write(rs, records);
            }
        }

    private:
    };

    template <typename T>
    void write(naft::Node &n, const Set<T> &set)
    {
        set.write(n);
    }

    template <typename T>
    bool read(Set<T> &dst, naft::Range &range)
    {
        MSS_BEGIN(bool);

        naft::Attrs attrs;
        range.pop_attrs(attrs);
        auto it = attrs.find("name");
        if (it != attrs.end())
            dst.name = it->second;

        naft::Range block;
        MSS(range.pop_block(block));

        {
            MSS(block.pop_name("fields"));
            MSS(read(dst.fields, block));
        }

        {
            MSS(block.pop_name("records"));
            MSS(read(dst.records, block, dst.fields));
        }

        MSS_END();
    }

} } 

#endif
