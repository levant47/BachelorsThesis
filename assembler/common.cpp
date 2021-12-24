typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

struct String
{
    u64 capacity;
    u64 size;
    char* data;

    static const u64 DEFAULT_CAPACITY = 16;

    static String allocate(u64 size = DEFAULT_CAPACITY)
    {
        String result;
        result.capacity = size;
        result.size = 0;
        result.data = (char*)malloc(result.capacity);
        return result;
    }

    void push(char c)
    {
        if (size == capacity)
        {
            capacity *= 2;
            data = (char*)realloc(data, capacity);
        }
        data[size] = c;
        size++;
    }

    String copy()
    {
        String result;
        result.capacity = size;
        result.size = size;
        result.data = (char*)malloc(result.capacity);
        for (u64 i = 0; i < size; i++)
        {
            result.data[i] = data[i];
        }
        return result;
    }

    // DEBUG
    void print()
    {
        printf("%.*s", (int)size, data);
    }
};

bool operator==(String left, String right)
{
    if (left.size != right.size)
    {
        return false;
    }
    for (u64 i = 0; i < left.size; i++)
    {
        if (left.data[i] != right.data[i])
        {
            return false;
        }
    }
    return true;
}

bool operator!=(String left, String right)
{
    return !(left == right);
}

bool operator==(String left, const char* right)
{
    u64 i;
    for (i = 0; i < left.size && right[i] != '\0'; i++)
    {
        if (left.data[i] != right[i])
        {
            return false;
        }
    }
    return left.size == i && right[i] == '\0';
}

bool operator!=(String left, const char* right)
{
    return !(left == right);
}

struct Strings
{
    u64 capacity;
    u64 size;
    String* data;

    static const u64 DEFAULT_CAPACITY = 16;

    static Strings allocate()
    {
        Strings result;
        result.capacity = DEFAULT_CAPACITY;
        result.size = 0;
        result.data = (String*)malloc(result.capacity*sizeof(String));
        return result;
    }

    void deallocate()
    {
        free(data);
    }

    void push(String string)
    {
        if (size == capacity)
        {
            capacity *= 2;
            data = (String*)realloc(data, capacity*sizeof(String));
        }
        data[size] = string;
        size++;
    }

    bool contains(String string)
    {
        for (u64 i = 0; i < size; i++)
        {
            if (data[i] == string)
            {
                return true;
            }
        }
        return false;
    }
};

void panic(const char* message)
{
    printf("%s\n", message);
    exit(1);
}
