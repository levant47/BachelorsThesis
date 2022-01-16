struct BinaryResult
{
    u64 capacity;
    u64 size;
    u8* data;

    static const u64 DEFAULT_CAPACITY = 256;

    static BinaryResult allocate()
    {
        BinaryResult result;
        result.capacity = DEFAULT_CAPACITY;
        result.size = 0;
        result.data = (u8*)malloc(result.capacity);
        return result;
    }

    void push(u8 byte)
    {
        if (size == capacity)
        {
            capacity *= 2;
            data = (u8*)realloc(data, capacity);
        }
        data[size] = byte;
        size++;
    }

    void push(u8 byte1, u8 byte2)
    {
        push(byte1);
        push(byte2);
    }

    void print_vhdl()
    {
        printf(
            "library IEEE;\n"
            "use IEEE.std_logic_1164.all;\n"
            "package program is\n"
            "    constant code : work.types.T_MEMORY := (\n"

        );
        for (u64 i = 0; i < size; i++)
        {
            char buffer[8];
            for (u8 k = 0; k < 8; k++)
            {
                buffer[k] = ((data[i] >> (8-k-1)) & 1) + '0';
            }
            printf("        b\"%.8s\"", buffer);
            if (i != size-1)
            {
                printf(",");
            }
            printf("\n");
        }
        printf(
            "    );\n"
            "end program;\n"
        );
    }

    // DEBUG
    void print()
    {
        for (u64 i = 0; i < size; i++)
        {
            char buffer[8];
            for (u8 k = 0; k < 8; k++)
            {
                buffer[k] = ((data[i] >> (8-k-1)) & 1) + '0';
            }
            printf("%.8s\n", buffer);
        }
    }
};

struct LabelAddress
{
    String label;
    u8 address;
};

struct FindLabelAddressResult
{
    bool found;
    u8 address;
};

struct LabelsMap
{
    u64 capacity;
    u64 size;
    LabelAddress* data;

    static const u64 DEFAULT_CAPACITY = 16;

    static LabelsMap allocate()
    {
        LabelsMap result;
        result.capacity = DEFAULT_CAPACITY;
        result.size = 0;
        result.data = (LabelAddress*)malloc(result.capacity * sizeof(LabelAddress));
        return result;
    }

    void deallocate()
    {
        free(data);
    }

    void push(LabelAddress item)
    {
        if (size == capacity)
        {
            capacity *= 2;
            data = (LabelAddress*)malloc(capacity * sizeof(LabelAddress));
        }
        data[size] = item;
        size++;
    }

    FindLabelAddressResult find(String label)
    {
        FindLabelAddressResult result;
        for (u64 i = 0; i < size; i++)
        {
            if (data[i].label == label)
            {
                result.found = true;
                result.address = data[i].address;
                return result;
            }
        }
        result.found = false;
        return result;
    }
};

BinaryResult compile_to_binary(Ast ast)
{
    auto result = BinaryResult::allocate();

    auto labels_map = LabelsMap::allocate();
    auto labels_to_fix = LabelsMap::allocate();

    for (u64 i = 0; i < ast.size; i++)
    {
        switch (ast.data[i].type)
        {
            case AstNodeTypeNop:
                result.push(0);
                break;
            case AstNodeTypePush:
            {
                result.push(1);
                if (ast.data[i].push_type == PushNodeTypeInteger)
                {
                    result.push(ast.data[i].integer);
                }
                else // PushNodeTypeLabel
                {
                    // TODO: this snippet needs to be extracted into its own method
                    auto find_result = labels_map.find(ast.data[i].label);
                    if (find_result.found)
                    {
                        result.push(find_result.address);
                    }
                    else
                    {
                        LabelAddress label_address;
                        label_address.label = ast.data[i].label;
                        label_address.address = result.size;
                        labels_to_fix.push(label_address);
                        result.push(0);
                    }
                }
                break;
            }
            case AstNodeTypePop:
                result.push(2);
                break;
            case AstNodeTypeAdd:
                result.push(3);
                break;
            case AstNodeTypeCmp:
                result.push(4);
                break;
            case AstNodeTypeJl:
                result.push(5);
                break;
            case AstNodeTypeJle:
                result.push(6);
                break;
            case AstNodeTypeJeq:
                result.push(7);
                break;
            case AstNodeTypeJge:
                result.push(8);
                break;
            case AstNodeTypeJg:
                result.push(9);
                break;
            case AstNodeTypeJne:
                result.push(10);
                break;
            case AstNodeTypeJmp:
                result.push(11);
                break;
            case AstNodeTypeDup:
                result.push(12);
                break;
            case AstNodeTypeOut:
                result.push(13);
                break;
            case AstNodeTypePushNothing:
                result.push(14);
                break;
            case AstNodeTypeLabel:
            {
                LabelAddress label_address;
                label_address.label = ast.data[i].label;
                label_address.address = result.size;
                labels_map.push(label_address);
                break;
            }
        }
    }

    for (u64 i = 0; i < labels_to_fix.size; i++)
    {
        auto find_result = labels_map.find(labels_to_fix.data[i].label);
        if (!find_result.found)
        {
            panic("Failed to find a label");
        }
        result.data[labels_to_fix.data[i].address] = find_result.address;
    }

    labels_map.deallocate();

    return result;
}
