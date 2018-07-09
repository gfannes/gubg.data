#ifndef HEADER_gubg_data_Record_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Record_hpp_ALREADY_INCLUDED

#include "gubg/data/Field.hpp"
#include "gubg/Range.hpp"
#include "gubg/mss.hpp"
#include <list>
#include <vector>
#include <numeric>

namespace gubg { namespace data { 

    namespace details { 
        inline void read(float &v, Strange &strange) { strange.pop_float(v); }
        inline void read(double &v, Strange &strange) { strange.pop_float(v); }
        inline void read(int &v, Strange &strange) { strange.pop_decimal(v); }
        inline void read(std::string &v, Strange &strange) { v = strange.str(); }
    } 

    template <typename T>
    class Record
    {
    public:
        using Range = gubg::Range<T*>;
        using Ranges = std::vector<Range>;

        Ranges fields;

        Record(size_t nr_fields, size_t total_dim): data_(total_dim), fields(nr_fields), next_field_(fields.begin()) {}

        bool add(Range &range, size_t dim)
        {
            MSS_BEGIN(bool);
            MSS(dim <= (data_.size()-next_ix_));
            MSS(next_field_ != fields.end());
            range = *next_field_ = Range(&data_[next_ix_], &data_[next_ix_+dim]);
            next_ix_ += dim;
            ++next_field_;
            MSS_END();
        }

    private:
        using Data = std::vector<T>;
        Data data_;
        size_t next_ix_ = 0;

        typename Ranges::iterator next_field_;
    };

    template <typename T>
    bool read(Record<T> &dst, naft::Range &range, const Fields &fields, unsigned int rix)
    {
        MSS_BEGIN(bool);
        naft::Range record;
        Strange fix_strange;
        MSS(range.pop_block(record));
        auto fix = 0u;
        for (; record.pop_tag(fix_strange); ++fix)
        {
            MSS(fix == std::stoul(fix_strange.str()), std::cout << "Error: Record.field index mismatch (" << fix << " != " << fix_strange << ")" << std::endl);
            MSS(fix < fields.size(), std::cout << "Error: Too many fields for record " << rix << "" << std::endl);
            const auto dim = fields[fix].dim;
            typename Record<T>::Range rng;
            MSS(dst.add(rng, dim));
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
        MSS(fix == fields.size(), std::cout << "Error: Not enough fields given for record " << rix << " (" << fix << " != " << fields.size() << ")" << std::endl);
        MSS_END();
    }

    template <typename T>
    void write(naft::Node &n, const Record<T> &record)
    {
        auto fix = 0u;
        for (auto field: record.fields)
        {
            auto f = n.node(std::to_string(fix));
            if (!field.empty())
            {
                f.text(field.front());
                field.pop_front();
                while (!field.empty())
                {
                    f.text(' ');
                    f.text(field.front());
                    field.pop_front();
                }
            }
            ++fix;
        }
    }

    //Records
    //
    template <typename T>
    using Records = std::list<Record<T>>;

    template <typename T>
    void write(naft::Node &n, const Records<T> &records)
    {
        auto rix = 0u;
        for (const auto &record: records)
        {
            auto r = n.node(std::to_string(rix));
            data::write(r, record);
            ++rix;
        }
    }

    template <typename T>
    bool read(Records<T> &dst, naft::Range &range, const Fields &fields)
    {
        MSS_BEGIN(bool);
        const auto total_dim = std::accumulate(RANGE(fields), 0u, [](unsigned int total, const Field &field){return total+field.dim;});
        naft::Range records;
        range.pop_block(records);
        Strange rix_strange;
        for (auto rix = 0u; records.pop_tag(rix_strange); ++rix)
        {
            MSS(rix == std::stoul(rix_strange.str()), std::cout << "Error: Record index mismatch (" << rix << " != " << rix_strange << ")" << std::endl);
            Record<T> rec(fields.size(), total_dim);
            MSS(read(rec, records, fields, rix));
            dst.push_back(std::move(rec));
        }
        MSS_END();
    }

} } 

#endif
