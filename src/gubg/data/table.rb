module Gubg
    module Data
        class Row
            def initialize()
                @name__value = {}
            end
            def set(name, value)
                @name__value[name.to_sym] = value
            end
            def call(name)
                name = name.to_sym
                raise("Could not find value for name '#{name}'") unless @name__value.has_key?(name)
                @name__value[name]
            end
        end
        
        class Table
            attr_accessor(:fieldnames, :rows)
            
            def initialize(sep:, escaped: nil)
                @sep = sep
                @escaped = escaped || false
                @conditions = []
            end

            def where(&block)
                @conditions << block
            end
            
            def load(fp)
                @fieldnames = nil
                @rows = []

                File.read(fp).each_line do |line|
                    if !@fieldnames
                        @fieldnames = split_(line).map{|str|str.to_sym}
                    else
                        row = Row.new()
                        split_(line).each_with_index do |value, ix0|
                            row.set(@fieldnames[ix0], value)
                        end
                        if @conditions.all?{|condition|condition.(row)}
                            @rows << row
                        end
                    end
                end
            end

            def nr_rows()
                @rows.size()
            end
            def nr_fields()
                @fieldnames.size()
            end

            def field(fieldname)
                fieldname = fieldname.to_sym
                raise("Could not find field '#{fieldname}'") unless @fieldnames.include?(fieldname)

                @rows.map{|row|row[fieldname]}
            end

            def each(&block)
                @rows.each{|row|block.(row)}
            end

            private
            def split_(line)
                values = []

                @line = line.chomp()
                while value = pop_value_()
                    values << value
                end

                values
            end
            def pop_value_()
                return nil unless @line
                
                if @escaped
                    raise("Expected to find starting '\"'") unless @line[0] == '"'
                    @line = @line[1...]
                    if ix = @line.index("\"#{@sep}")
                        value = @line[...ix]
                        @line = @line[ix+2...]
                    else
                        value = @line[...-1]
                        @line = nil
                    end
                    value
                else
                    if ix = @line.index(@sep)
                        value = @line[...ix]
                        @line = @line[ix+1...]
                    else
                        value = @line
                        @line = nil
                    end
                end
            end

        end
    end
end
