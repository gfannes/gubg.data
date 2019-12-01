#ifndef HEADER_gubg_data_Base_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Base_hpp_ALREADY_INCLUDED

#include "gubg/data/Set.hpp"
#include "gubg/mss.hpp"
#include "gubg/std/filesystem.hpp"
#include "gubg/file/system.hpp"
#include <map>
#include <string>

namespace gubg { namespace data { 

    template <typename T>
    class Base
    {
    public:
        using Set = data::Set<T>;

        template <typename Writer>
        bool write(Writer &w) const
        {
            MSS_BEGIN(bool);
            for (const auto &p: sets_) { MSS(w.object(":data.Set", p.second)); }
            MSS_END();
        }

        bool load(const std::string &content)
        {
            MSS_BEGIN(bool);
            s11n::Reader r(content);
            for (bool more = true; more; )
            {
                Set set;
                more = r.object(":data.Set", set);
                if (more)
                    sets_.emplace(set.name, std::move(set));
            }
            MSS_END();
        }
        bool load_from_file(const std::filesystem::path &path)
        {
            MSS_BEGIN(bool);
            std::string content;
            MSS(file::read(content, path));
            MSS(load(content));
            MSS_END();
        }

    private:
        std::map<std::string, Set> sets_;
    };

} } 

#endif
