#ifndef HEADER_gubg_data_Field_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Field_hpp_ALREADY_INCLUDED

#include "gubg/s11n/Reader.hpp"
#include "gubg/s11n/Writer.hpp"
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

        template <typename Writer>
        bool write(Writer &w) const
        {
            MSS_BEGIN(bool);
            MSS(w.attr("name", name));
            if (dim != 1)
                MSS(w.attr("dim", dim));
            MSS_END();
        }

        bool read(s11n::Reader &r)
        {
            MSS_BEGIN(bool);
            MSS(r.attr("name", name));
            if (!r.attr("dim", dim))
                dim = 1;
            MSS_END();
        }
    };

    using Fields = std::vector<Field>;

} } 

#endif
