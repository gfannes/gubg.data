#ifndef HEADER_gubg_data_Table_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Table_hpp_ALREADY_INCLUDED

#include "gubg/data/Traits.hpp"
#include "gubg/mss.hpp"
#include "gubg/Range.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <ostream>
#include <cassert>

namespace gubg { namespace data { 

    template <typename Value>
    class Table
    {
    public:
        using Fieldnames = std::vector<std::string>;
        using Row = std::vector<Value>;

        void clear() {*this = Table{};}

        size_t nr_rows() const {return rows_.size();}
        size_t nr_cols() const {return fieldnames_.size();}

        const Fieldnames &fieldnames() const {return fieldnames_;}

        bool add_field(const std::string &name)
        {
            MSS_BEGIN(bool);
            MSS(std::find(RANGE(fieldnames_), name) == fieldnames_.end());
            fieldnames_.push_back(name);
            const auto nrc = fieldnames_.size();
            for (auto &row: rows_)
                row.resize(nrc);
            MSS_END();
        }

        Row &add_row()
        {
            rows_.emplace_back(nr_cols());
            return rows_.back();
        }

        const Row &row(size_t rix) const { return rows_[rix]; }
        Row &row(size_t rix) { return rows_[rix]; }

        template <typename DstValue>
        bool select(Table<DstValue> &dst, const std::vector<size_t> &fixs)
        {
            MSS_BEGIN(bool);
            dst.clear();
            for (auto fix: fixs)
            {
                MSS(fix < fieldnames_.size());
                MSS(dst.add_field(fieldnames_[fix]));
            }
            for (auto &src_row: rows_)
            {
                auto &dst_row = dst.add_row();
                auto dst_it = dst_row.begin();
                for (auto fix: fixs)
                {
                    MSS(Traits<DstValue>::convert(*dst_it++, src_row[fix]));
                }
            }
            MSS_END();
        }

        void stream(std::ostream &os) const
        {
            os << "[meta]{" << std::endl;
            for (size_t cix = 0; cix < nr_cols(); ++cix)
                os << "  [field](name:" << fieldnames_[cix] << ")(index:" << cix << ")" << std::endl;
            os << "}" << std::endl;
            os << "[data]{" << std::endl;
            for (size_t rix = 0; rix < nr_rows(); ++rix)
            {
                os << "  [row]";
                for (size_t cix = 0; cix < nr_cols(); ++cix)
                    os << "(" << cix << ":" << rows_[rix][cix] << ")";
                os << std::endl;
            }
            os << "}" << std::endl;
        }

    private:
        Fieldnames fieldnames_;
        std::vector<Row> rows_;
    };

    template <typename Value>
    std::ostream &operator<<(std::ostream &os, const Table<Value> &table)
    {
        table.stream(os);
        return os;
    }

} } 

#endif
