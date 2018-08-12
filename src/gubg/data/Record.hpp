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

        bool read(s11n::Reader &r, const Fields &fields_info)
        {
            MSS_BEGIN(bool);
            for (auto fix = 0u; fix < fields.size(); ++fix)
            {
                auto read_field = [&](auto &r1){
                    MSS_BEGIN(bool);

                    //Get the range where this data should be stored
                    Range rng;
                    MSS(add(rng, fields_info[fix].dim));

                    Strange values;
                    r1.text(values);
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
                    values.each_split(' ', add_value_to_range);
                    MSS(all_values_could_be_added, std::cout << "Error: Too many values for record " << r.tag() << ", field " << fix << "" << std::endl);
                    MSS(rng.empty(), std::cout << "Error: Not enough values for record " << r.tag() << ", field " << fix << "" << std::endl);

                    MSS_END();
                };
                MSS(r(fix, read_field), std::cout << "Error: Could not find field " << fix << std::endl);
            }
            MSS(r.empty(), std::cout << "Error: Too many fields for record " << r.tag() << "" << std::endl);
            MSS_END();
        }

        template <typename Writer>
        bool write(Writer &w) const
        {
            MSS_BEGIN(bool);
            auto fix = 0u;
            //Copy of field is intentional
            for (Range field: fields)
            {
                auto write_field = [&](Writer &w1){
                    MSS_BEGIN(bool);
                    if (!field.empty())
                    {
                        w.text(field.front());
                        field.pop_front();
                        while (!field.empty())
                        {
                            w.text(' ');
                            w.text(field.front());
                            field.pop_front();
                        }
                    }
                    MSS_END();
                };
                MSS(w(fix, write_field));
                ++fix;
            }
            MSS_END();
        }

    private:
        using Data = std::vector<T>;
        Data data_;
        size_t next_ix_ = 0;

        typename Ranges::iterator next_field_;
    };

    //Records
    //
    template <typename T>
    using Records = std::list<Record<T>>;

} } 

#endif
