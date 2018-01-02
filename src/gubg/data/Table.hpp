#ifndef HEADER_gubg_data_Table_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Table_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"
#include "gubg/Range.hpp"
#include "gubg/Strange.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <ostream>
#include <cassert>

namespace gubg { namespace data { 

    template <typename Value> struct Traits
    {
        static void convert(Value &dst, const gubg::Strange &strange)
        {
            dst = strange;
        }
    }; 
    template <> struct Traits<float>
    {
        static void convert(float &dst, gubg::Strange strange)
        {
            strange.pop_float(dst);
        }
    };
    template <> struct Traits<std::string>
    {
        static void convert(std::string &dst, const gubg::Strange &strange)
        {
            dst = strange.str();
        }
    };

    template <typename Value>
    class Table
    {
    public:
        void clear() {*this = Table{};}

        size_t nr_cols() const {return fieldnames_.size();}
        size_t nr_rows() const {return rows_.size();}

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

        class Row
        {
        public:
            Row(Table &outer): outer_(&outer) {}
            Row(Row &&dying): outer_(dying.outer_)
            {
                outer_ = nullptr;
            }
            ~Row()
            {
                if (!outer_)
                    return;
                const auto rix = outer_->add_row();
                const auto nr = std::min(values_.size(), outer_->nr_cols());
                for (size_t cix = 0; cix < nr; ++cix)
                    outer_->set_value(rix, cix, values_[cix]);
            }
            template <typename V>
            bool set(size_t ix, const V &value)
            {
                values_.resize(std::max(ix+1, values_.size()));
                Traits<Value>::convert(values_[ix], value);
                return true;
            }
        private:
            Row(const Row &);
            Row &operator=(const Row &);

            Table *outer_;
            std::vector<Value> values_;
        };
        Row stage_row()
        {
            Row row(*this);
            return row;
        }

        size_t add_row()
        {
            const auto rix = rows_.size();
            rows_.emplace_back(nr_cols());
            return rix;
        }

        void set_value(size_t rix, size_t cix, const Value &value)
        {
            assert(rix < rows_.size());
            assert(cix < rows_[rix].size());
            rows_[rix][cix] = value;
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
        std::vector<std::string> fieldnames_;
        std::vector<std::vector<Value>> rows_;
    };

    template <typename Value>
    std::ostream &operator<<(std::ostream &os, const Table<Value> &table)
    {
        table.stream(os);
        return os;
    }

} } 

#endif
