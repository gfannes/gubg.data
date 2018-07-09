#ifndef HEADER_gubg_data_Field_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Field_hpp_ALREADY_INCLUDED

#include "gubg/naft/Range.hpp"
#include "gubg/mss.hpp"
#include <vector>

namespace gubg { namespace data { 

    struct Field
    {
        std::string name;
        unsigned int dim = 1;

        Field() {}
        Field(const std::string &name): name(name) {}
        Field(const std::string &name, unsigned int dim): name(name), dim(dim) {}
    };

    inline bool read(Field &dst, naft::Range &range)
    {
        MSS_BEGIN(bool);

        auto attr = range.pop_attr();
        {
            auto it = attr.find("name");
            MSS(it != attr.end());
            dst.name = it->second;
        }
        {
            auto it = attr.find("dim");
            if (it != attr.end())
                dst.dim = std::stoul(it->second);
        }

        MSS_END();
    }

    using Fields = std::vector<Field>;

    inline bool read(Fields &dst, naft::Range &range)
    {
        MSS_BEGIN(bool);
        naft::Range block;
        Strange fix;
        range.pop_block(block);
        for (auto ix = 0u; block.pop_tag(fix); ++ix)
        {
            MSS(ix == std::stoul(fix.str()), std::cout << "Error: Field order not respected (" << fix << " != " << ix << ")" << std::endl);
            Field field;
            MSS(read(field, block));
            dst.push_back(field);
        }
        MSS_END();
    }

} } 

#endif
