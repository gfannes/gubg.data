#ifndef HEADER_gubg_data_Base_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Base_hpp_ALREADY_INCLUDED

#include "gubg/data/Set.hpp"
#include "gubg/mss.hpp"
#include "gubg/std/filesystem.hpp"
#include "gubg/file/System.hpp"
#include <map>
#include <string>

namespace gubg { namespace data { 

    template <typename T>
    class Base
    {
    public:
        using Set = data::Set<T>;

        bool load(const std::string &content)
        {
            MSS_BEGIN(bool);
            naft::Range range(content);
            while (range.pop_type("data.Set"))
            {
                Set set;
                MSS(read(set, range));
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

        void write(naft::Document &doc) const
        {
            for (const auto &p: sets_)
                data::write(doc, p.second);
        }

    private:
        std::map<std::string, Set> sets_;
    };

    template <typename T>
    void write(naft::Document &doc, const Base<T> &base)
    {
        base.write(doc);
    }

} } 

#endif
