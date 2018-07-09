#ifndef HEADER_gubg_data_Set_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Set_hpp_ALREADY_INCLUDED

#include "gubg/data/Field.hpp"
#include "gubg/data/Record.hpp"
#include "gubg/naft/Range.hpp"
#include "gubg/mss.hpp"
#include "gubg/Range.hpp"
#include <numeric>

namespace gubg { namespace data { 

    namespace details { 
        void read(float &v, Strange &strange) { strange.pop_float(v); }
        void read(double &v, Strange &strange) { strange.pop_float(v); }
        void read(int &v, Strange &strange) { strange.pop_decimal(v); }
        void read(std::string &v, Strange &strange) { v = strange.str(); }
    } 

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

    private:
    };

    template <typename T>
    bool read(Set<T> &dst, naft::Range &range)
    {
        MSS_BEGIN(bool);

        naft::Attr attr;
        range.pop_attr(attr);
        auto it = attr.find("name");
        if (it != attr.end())
            dst.name = it->second;

        naft::Range block;
        MSS(range.pop_block(block));

        {
            MSS(block.pop_name("fields"));
            MSS(read(dst.fields, block));
        }

        const auto total_dim = std::accumulate(RANGE(dst.fields), 0u, [](unsigned int total, const Field &field){return total+field.dim;});

        {
            MSS(block.pop_name("records"));
            naft::Range records;
            block.pop_block(records);
            Strange rix_strange, fix_strange;
            for (auto rix = 0u; records.pop_tag(rix_strange); ++rix)
            {
                MSS(rix == std::stoul(rix_strange.str()), std::cout << "Error: Record index mismatch (" << rix << " != " << rix_strange << ")" << std::endl);
                naft::Range record;
                MSS(records.pop_block(record));
                Record<T> rec(dst.fields.size(), total_dim);
                auto fix = 0u;
                for (; record.pop_tag(fix_strange); ++fix)
                {
                    MSS(fix == std::stoul(fix_strange.str()), std::cout << "Error: Record.field index mismatch (" << fix << " != " << fix_strange << ")" << std::endl);
                    MSS(fix < dst.fields.size(), std::cout << "Error: Too many fields for record " << rix << "" << std::endl);
                    const auto dim = dst.fields[fix].dim;
                    typename Record<T>::Range rng;
                    MSS(rec.add(rng, dim));
                    Strange value;
                    record.pop_block(value);
                    bool all_values_could_be_added = true;
                    auto add_value_to_range = [&](auto &part)
                    {
                        if (rng.empty())
                        {
                            all_values_could_be_added = false;
                            return;
                        }
                        details::read(rng.front(), part);
                        rng.pop_front();
                    };
                    value.each_split(' ', add_value_to_range);
                    MSS(all_values_could_be_added, std::cout << "Error: Too many values for record " << rix << ", field " << fix << "" << std::endl);
                    MSS(rng.empty(), std::cout << "Error: Not enough values for record " << rix << ", field " << fix << "" << std::endl);
                }
                MSS(fix == dst.fields.size(), std::cout << "Error: Not enough fields given for record " << rix << " (" << fix << " != " << dst.fields.size() << ")" << std::endl);
                dst.records.push_back(std::move(rec));
            }
        }

        MSS_END();
    }

} } 

#endif
