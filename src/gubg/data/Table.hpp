#ifndef HEADER_gubg_data_Table_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Table_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"
#include "gubg/Range.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <ostream>

namespace gubg { namespace data { 

    class Table
    {
    public:
        void clear() {*this = Table{};}

        size_t nr_cols() const {return fieldnames_.size();}
        size_t nr_rows() const {return nr_rows_;}

        bool add_field(const std::string &name)
        {
            MSS_BEGIN(bool);
            MSS(std::find(RANGE(fieldnames_), name) == fieldnames_.end());
            fieldnames_.push_back(name);
            data_.resize(fieldnames_.size());
            for (auto &col: data_)
                col.resize(nr_rows_);
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
            bool set(size_t ix, const std::string &value)
            {
                values_.resize(std::max(ix+1, values_.size()));
                values_[ix] = value;
                return true;
            }
        private:
            Row(const Row &);
            Row &operator=(const Row &);

            Table *outer_;
            std::vector<std::string> values_;
        };
        Row stage_row()
        {
            Row row(*this);
            return row;
        }

        size_t add_row()
        {
            ++nr_rows_;
            for (auto &col: data_)
                col.resize(nr_rows_);
            return nr_rows_-1;
        }

        void set_value(size_t rix, size_t cix, const std::string &value)
        {
            data_[cix][rix] = value;
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
                    os << "(" << cix << ":" << data_[cix][rix] << ")";
                os << std::endl;
            }
            os << "}" << std::endl;
        }

    private:
        std::vector<std::string> fieldnames_;
        std::vector<std::vector<std::string>> data_;
        size_t nr_rows_ = 0;
    };

    inline std::ostream &operator<<(std::ostream &os, const Table &table)
    {
        table.stream(os);
        return os;
    }

} } 

#endif
