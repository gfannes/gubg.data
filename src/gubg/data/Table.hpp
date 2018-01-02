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
                MSS(fix < nr_cols());
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
        //When selecting into a vector of vectors, Table will do the resizing
        template <typename DstValue>
        bool select(std::vector<std::vector<DstValue>> &rows, const std::vector<size_t> &fixs)
        {
            MSS_BEGIN(bool);
            for (auto fix: fixs)
            {
                MSS(fix < nr_cols());
            }
            rows.resize(nr_rows());
            for (size_t rix = 0; rix < nr_rows(); ++rix)
            {
                const auto &src_row = rows_[rix];
                auto &dst_row = rows[rix];
                dst_row.resize(fixs.size());
                for (size_t i = 0; i < fixs.size(); ++i)
                {
                    MSS(Traits<DstValue>::convert(dst_row[i], src_row[fixs[i]]));
                }
            }
            MSS_END();
        }
        //When selecting into something else than a vector of vectors, Table will NOT do the resizing, but fail instead
        template <typename DstValue, typename Rows>
        bool select(Rows &rows, const std::vector<size_t> &fixs)
        {
            MSS_BEGIN(bool);
            for (auto fix: fixs)
            {
                MSS(fix < nr_cols());
            }
            size_t rix = 0;
            for (auto &dst_row: rows)
            {
                const auto &src_row = rows_[rix++];
                MSS(dst_row.size() == fixs.size());
                size_t i = 0;
                for (auto &v: dst_row)
                {
                    MSS(Traits<DstValue>::convert(v, src_row[fixs[i++]]));
                }
            }
            MSS(rix == nr_rows());
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
