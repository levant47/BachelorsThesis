enum InstructionOpCode : u8
{
    InstructionOpCodeNop = 0,
    InstructionOpCodePush = 1,
    InstructionOpCodePop = 2,
    InstructionOpCodeAdd = 3,
    InstructionOpCodeCmp = 4,
    InstructionOpCodeJl = 5,
    InstructionOpCodeJle = 6,
    InstructionOpCodeJeq = 7,
    InstructionOpCodeJge = 8,
    InstructionOpCodeJg = 9,
    InstructionOpCodeJne = 10,
    InstructionOpCodeJmp = 11,
    InstructionOpCodeDup = 12,
    InstructionOpCodeOut = 13,
    InstructionOpCodePushNothing = 14,
    InstructionOpCodeDdup = 15,
    InstructionOpCodeStore = 16,
    InstructionOpCodeLoad = 17,
};

struct BinaryResultEntry
{
    u8 value;
    bool has_comment;
    String comment;
};

struct BinaryResult
{
    u64 capacity;
    u64 size;
    BinaryResultEntry* data;

    static const u64 DEFAULT_CAPACITY = 256;

    static BinaryResult allocate()
    {
        BinaryResult result;
        result.capacity = DEFAULT_CAPACITY;
        result.size = 0;
        result.data = (BinaryResultEntry*)malloc(result.capacity * sizeof(BinaryResultEntry));
        return result;
    }

    void push(u8 byte)
    {
        if (size == capacity)
        {
            capacity *= 2;
            data = (BinaryResultEntry*)realloc(data, capacity);
        }
        data[size].value = byte;
        size++;
    }

    void push(u8 byte, String comment)
    {
        push(byte);
        data[size-1].has_comment = true;
        data[size-1].comment = comment;
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
                buffer[k] = ((data[i].value >> (8-k-1)) & 1) + '0';
            }
            printf("        b\"%.8s\"", buffer);
            if (i != size-1)
            {
                printf(",");
            }
            if (data[i].has_comment)
            {
                printf(" -- ");
                data[i].comment.print();
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
                buffer[k] = ((data[i].value >> (8-k-1)) & 1) + '0';
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
        auto comment = ast.data[i].to_string();
        comment.push(" (line ");
        comment.push(ast.data[i].line);
        comment.push(")");
        switch (ast.data[i].type)
        {
            case AstNodeTypeNop:
                result.push(InstructionOpCodeNop, comment);
                break;
            case AstNodeTypePush:
            {
                result.push(InstructionOpCodePush, comment);
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
                result.push(InstructionOpCodePop, comment);
                break;
            case AstNodeTypeAdd:
                result.push(InstructionOpCodeAdd, comment);
                break;
            case AstNodeTypeCmp:
                result.push(InstructionOpCodeCmp, comment);
                break;
            case AstNodeTypeJl:
                result.push(InstructionOpCodeJl, comment);
                break;
            case AstNodeTypeJle:
                result.push(InstructionOpCodeJle, comment);
                break;
            case AstNodeTypeJeq:
                result.push(InstructionOpCodeJeq, comment);
                break;
            case AstNodeTypeJge:
                result.push(InstructionOpCodeJge, comment);
                break;
            case AstNodeTypeJg:
                result.push(InstructionOpCodeJg, comment);
                break;
            case AstNodeTypeJne:
                result.push(InstructionOpCodeJne, comment);
                break;
            case AstNodeTypeJmp:
                result.push(InstructionOpCodeJmp, comment);
                break;
            case AstNodeTypeDup:
                result.push(InstructionOpCodeDup, comment);
                break;
            case AstNodeTypeOut:
                result.push(InstructionOpCodeOut, comment);
                break;
            case AstNodeTypePushNothing:
                result.push(InstructionOpCodePushNothing, comment);
                break;
            case AstNodeTypeDdup:
                result.push(InstructionOpCodeDdup, comment);
                break;
            case AstNodeTypeStore:
                result.push(InstructionOpCodeStore, comment);
                break;
            case AstNodeTypeLoad:
                result.push(InstructionOpCodeLoad, comment);
                break;
            case AstNodeTypeCall: // call is a short hand for push-store-jmp
            {
                auto address_after_call = result.size + 4; // 4 is size of push-store-jmp instructions
                result.push(InstructionOpCodePush, comment);
                result.push(address_after_call);
                result.push(InstructionOpCodeStore);
                result.push(InstructionOpCodeJmp);
                break;
            }
            case AstNodeTypeRet: // ret is a short hand for load-jmp
                result.push(InstructionOpCodeLoad, comment);
                result.push(InstructionOpCodeJmp);
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
        result.data[labels_to_fix.data[i].address].value = find_result.address;
    }

    labels_map.deallocate();

    return result;
}
