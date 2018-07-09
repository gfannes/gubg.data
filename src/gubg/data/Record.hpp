#ifndef HEADER_gubg_data_Record_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Record_hpp_ALREADY_INCLUDED

#include "gubg/Range.hpp"
#include "gubg/mss.hpp"
#include <list>
#include <vector>

namespace gubg { namespace data { 

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
    using Records = std::list<Record<T>>;

} } 

#endif
