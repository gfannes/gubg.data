#ifndef HEADER_gubg_data_Set_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Set_hpp_ALREADY_INCLUDED

#include "gubg/data/Field.hpp"
#include "gubg/data/Record.hpp"
#include "gubg/mss.hpp"
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

        template <typename Writer>
        bool write(Writer &w) const
        {
            MSS_BEGIN(bool);

            MSS(w.attr("name", name));

            auto write_fields = [&](Writer &w){
                MSS_BEGIN(bool);
                auto fix = 0u;
                for (const auto &field: fields) { MSS(w.object(fix++, field)); }
                MSS_END();
            };
            MSS(w("fields", write_fields));

            auto write_records = [&](Writer &w){
                MSS_BEGIN(bool);
                auto rix = 0u;
                for (const auto &record: records) { MSS(w.object(rix++, record)); }
                MSS_END();
            };
            MSS(w("records", write_records));

            MSS_END();
        }

        bool read(s11n::Reader &r)
        {
            MSS_BEGIN(bool);

            name.clear();
            r.attr("name", name);

            auto read_fields = [&](s11n::Reader &r){
                MSS_BEGIN(bool);
                fields.clear();
                for (auto fix = 0u; ; ++fix)
                {
                    Field field;
                    if (!r.object(fix, field))
                        break;
                    fields.push_back(field);
                }
                MSS(r.empty(), std::cout << "Error: Not all fields could be interpreted" << std::endl);
                MSS_END();
            };
            MSS(r("fields", read_fields));

            auto read_records = [&](s11n::Reader &r){
                MSS_BEGIN(bool);
                const auto total_dim = std::accumulate(RANGE(fields), 0u, [](unsigned int total, const Field &field){return total+field.dim;});
                records.clear();
                for (auto fix = 0u; ; ++fix)
                {
                    Record<T> rec(fields.size(), total_dim);
                    if (!r.object(fix, rec, fields))
                        break;
                    records.push_back(std::move(rec));
                }
                MSS(r.empty(), std::cout << "Error: Not all records could be interpreted" << std::endl);
                MSS_END();
            };
            MSS(r("records", read_records));

            MSS_END();
        }

    private:
    };

} } 

#endif
