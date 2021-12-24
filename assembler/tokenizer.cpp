enum TokenType
{
    TokenTypeName,
    TokenTypeInteger,
    TokenTypeNewLine,
    TokenTypeColon,
};

struct Token
{
    TokenType type;
    union
    {
        String name;
        u8 integer;
    };

    // DEBUG
    void print()
    {
        switch (type)
        {
            case TokenTypeName:
                printf("Name ");
                name.print();
                break;
            case TokenTypeInteger:
                printf("Integer %hhu", integer);
                break;
            case TokenTypeNewLine:
                printf("NewLine");
                break;
            case TokenTypeColon:
                printf("Colon");
                break;
        }
    }
};

struct Tokens
{
    u64 capacity;
    u64 size;
    Token* data;

    static const u64 DEFAULT_CAPACITY = 100;

    static Tokens allocate()
    {
        Tokens result;
        result.capacity = DEFAULT_CAPACITY;
        result.size = 0;
        result.data = (Token*)malloc(result.capacity * sizeof(Token));
        return result;
    }

    void push(Token token)
    {
        if (size == capacity)
        {
            capacity *= 2;
            data = (Token*)realloc(data, capacity * sizeof(Token));
        }
        data[size] = token;
        size++;
    }

    // DEBUG
    void print()
    {
        for (u64 i = 0; i < size; i++)
        {
            data[i].print();
            if (i != size-1)
            {
                printf(", ");
            }
        }
        printf("\n");
    }
};

bool is_valid_first_name_char(char c)
{
    return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_';
}

bool is_valid_not_first_name_char(char c)
{
    return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_' || c >= '0' && c <= '9';
}

bool is_insignificant_whitespace(char c)
{
    return c == ' ' || c == '\t';
}

bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

struct TokenizationState
{
    String source;
    u64 source_index;
    Tokens tokens;

    bool skip_whitespace()
    {
        bool result = false;
        while (source_index != source.size && is_insignificant_whitespace(source.data[source_index]))
        {
            result = true;
            source_index++;
        }
        return result;
    }

    bool tokenize_name()
    {
        if (source_index == source.size || !is_valid_first_name_char(source.data[source_index]))
        {
            return false;
        }

        auto name = String::allocate();
        name.push(source.data[source_index]);

        source_index++;
        while (source_index != source.size && is_valid_not_first_name_char(source.data[source_index]))
        {
            name.push(source.data[source_index]);
            source_index++;
        }

        Token name_token;
        name_token.type = TokenTypeName;
        name_token.name = name;
        tokens.push(name_token);
        return true;
    }

    bool tokenize_integer()
    {
        if ((source_index > source.size-2 || source.data[source_index] != '-' || !is_digit(source.data[source_index+1]))
            && (source_index == source.size || !is_digit(source.data[source_index])))
        {
            return false;
        }

        bool is_negative = source.data[source_index] == '-';
        if (is_negative)
        {
            source_index++;
        }

        u8 integer = 0;
        do
        {
            integer = integer * 10 + source.data[source_index] - '0';
            source_index++;
        } while (source_index != source.size && is_digit(source.data[source_index]));

        if (is_negative)
        {
            integer = ~integer + 1;
        }

        Token integer_token;
        integer_token.type = TokenTypeInteger;
        integer_token.integer = integer;
        tokens.push(integer_token);
        return true;
    }

    bool tokenize_newline()
    {
        if (source_index < source.size && source.data[source_index] == '\n')
        {
            Token new_line_token;
            new_line_token.type = TokenTypeNewLine;
            tokens.push(new_line_token);
            source_index++;
            return true;
        }
        if (source_index < source.size-1 && source.data[source_index] == '\r' && source.data[source_index+1] == '\n')
        {
            Token new_line_token;
            new_line_token.type = TokenTypeNewLine;
            tokens.push(new_line_token);
            source_index += 2;
            return true;
        }
        return false;
    }

    bool tokenize_colon()
    {
        if (source_index == source.size || source.data[source_index] != ':')
        {
            return false;
        }
        Token colon_token;
        colon_token.type = TokenTypeColon;
        tokens.push(colon_token);
        source_index++;
        return true;
    }
};

struct TokenizationResult
{
    bool success;
    Tokens tokens;
};

TokenizationResult tokenize(String source)
{
    TokenizationState state;
    state.source = source;
    state.source_index = 0;
    state.tokens = Tokens::allocate();

    while (state.source_index != source.size)
    {
        if (
            state.skip_whitespace()
                || state.tokenize_name()
                || state.tokenize_integer()
                || state.tokenize_newline()
                || state.tokenize_colon()
        )
        {
            continue;
        }

        TokenizationResult result;
        result.success = false;
        return result;
    }

    TokenizationResult result;
    result.success = true;
    result.tokens = state.tokens;
    return result;
}
