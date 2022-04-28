enum AstNodeType
{
    AstNodeTypeNop,
    AstNodeTypePush,
    AstNodeTypePop,
    AstNodeTypeAdd,
    AstNodeTypeCmp,
    AstNodeTypeJl,
    AstNodeTypeJle,
    AstNodeTypeJeq,
    AstNodeTypeJge,
    AstNodeTypeJg,
    AstNodeTypeJne,
    AstNodeTypeJmp,
    AstNodeTypeDup,
    AstNodeTypeOut,
    AstNodeTypePushNothing,
    AstNodeTypeDdup,
    AstNodeTypeStore,
    AstNodeTypeLoad,
    AstNodeTypeCall,
    AstNodeTypeRet,
    AstNodeTypeLabel,
};

enum PushNodeType
{
    PushNodeTypeInteger,
    PushNodeTypeLabel,
};

struct AstNode
{
    AstNodeType type;

    // only for AstNodeTypePush
    PushNodeType push_type;
    union
    {
        u8 integer;
        String label;
    };

    // DEBUG
    void print()
    {
        switch (type)
        {
            case AstNodeTypeNop:
                printf("Nop");
                break;
            case AstNodeTypePush:
                printf("Push ");
                if (push_type == PushNodeTypeInteger)
                {
                    printf("%hhu", integer);
                }
                else // PushNodeTypeLabel
                {
                    label.print();
                }
                break;
            case AstNodeTypePop:
                printf("Pop");
                break;
            case AstNodeTypeAdd:
                printf("Add");
                break;
            case AstNodeTypeCmp:
                printf("Cmp");
                break;
            case AstNodeTypeJl:
                printf("Jl");
                break;
            case AstNodeTypeJle:
                printf("Jle");
                break;
            case AstNodeTypeJeq:
                printf("Jeq");
                break;
            case AstNodeTypeJge:
                printf("Jge");
                break;
            case AstNodeTypeJg:
                printf("Jg");
                break;
            case AstNodeTypeJne:
                printf("Jne");
                break;
            case AstNodeTypeJmp:
                printf("Jmp");
                break;
            case AstNodeTypeDup:
                printf("Dup");
                break;
            case AstNodeTypeOut:
                printf("Out");
                break;
            case AstNodeTypePushNothing:
                printf("Push");
                break;
            case AstNodeTypeDdup:
                printf("Ddup");
                break;
            case AstNodeTypeStore:
                printf("Store");
                break;
            case AstNodeTypeLoad:
                printf("Load");
                break;
            case AstNodeTypeCall:
                printf("Call");
                break;
            case AstNodeTypeRet:
                printf("Ret");
                break;
            case AstNodeTypeLabel:
                printf("Label ");
                label.print();
                break;
        }
    }
};

struct Ast
{
    u64 capacity;
    u64 size;
    AstNode* data;

    static const u64 DEFAULT_CAPACITY = 100;

    static Ast allocate()
    {
        Ast result;
        result.capacity = DEFAULT_CAPACITY;
        result.size = 0;
        result.data = (AstNode*)malloc(result.capacity * sizeof(AstNode));
        return result;
    }

    void push(AstNode node)
    {
        if (size == capacity)
        {
            capacity *= 2;
            data = (AstNode*)realloc(data, capacity * sizeof(AstNode));
        }
        data[size] = node;
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

struct CheckLabelsResult
{
    bool all_good;
    String missing_label;
};

struct AstParsingState
{
    Tokens tokens;
    u64 token_index;
    Ast ast;
    Strings registered_labels;
    Strings labels_to_check;

    bool skip_new_lines()
    {
        bool result = false;
        while (token_index != tokens.size && tokens.data[token_index].type == TokenTypeNewLine)
        {
            result = true;
            token_index++;
        }
        return result;
    }

    bool parse_nop()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "nop"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode nop_node;
        nop_node.type = AstNodeTypeNop;
        ast.push(nop_node);
        token_index += 2;
        return true;
    }

    bool parse_push()
    {
        if (token_index > tokens.size-3
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "push"
            || tokens.data[token_index+1].type != TokenTypeInteger && tokens.data[token_index+1].type != TokenTypeName
            || tokens.data[token_index+2].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode push_node;
        push_node.type = AstNodeTypePush;
        if (tokens.data[token_index+1].type == TokenTypeInteger)
        {
            push_node.push_type = PushNodeTypeInteger;
            push_node.integer = tokens.data[token_index+1].integer;
        }
        else // TokenTypeName
        {
            push_node.push_type = PushNodeTypeLabel;
            push_node.label = tokens.data[token_index+1].name.copy();
            if (!registered_labels.contains(push_node.label))
            {
                labels_to_check.push(push_node.label);
            }
        }
        ast.push(push_node);
        token_index += 3;
        return true;
    }

    bool parse_pop()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "pop"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode pop_node;
        pop_node.type = AstNodeTypePop;
        ast.push(pop_node);
        token_index += 2;
        return true;
    }

    bool parse_add()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "add"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode add_node;
        add_node.type = AstNodeTypeAdd;
        ast.push(add_node);
        token_index += 2;
        return true;
    }

    bool parse_cmp()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "cmp"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode cmp_node;
        cmp_node.type = AstNodeTypeCmp;
        ast.push(cmp_node);
        token_index += 2;
        return true;
    }

    bool parse_jl()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "jl"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode jl_node;
        jl_node.type = AstNodeTypeJl;
        ast.push(jl_node);
        token_index += 2;
        return true;
    }

    bool parse_jle()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "jle"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode jle_node;
        jle_node.type = AstNodeTypeJle;
        ast.push(jle_node);
        token_index += 2;
        return true;
    }

    bool parse_jeq()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "jeq"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode jeq_node;
        jeq_node.type = AstNodeTypeJeq;
        ast.push(jeq_node);
        token_index += 2;
        return true;
    }

    bool parse_jge()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "jge"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode jge_node;
        jge_node.type = AstNodeTypeJge;
        ast.push(jge_node);
        token_index += 2;
        return true;
    }

    bool parse_jg()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "jg"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode jg_node;
        jg_node.type = AstNodeTypeJg;
        ast.push(jg_node);
        token_index += 2;
        return true;
    }

    bool parse_jne()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "jne"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode jne_node;
        jne_node.type = AstNodeTypeJne;
        ast.push(jne_node);
        token_index += 2;
        return true;
    }

    bool parse_jmp()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "jmp"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode jmp_node;
        jmp_node.type = AstNodeTypeJmp;
        ast.push(jmp_node);
        token_index += 2;
        return true;
    }

    bool parse_dup()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "dup"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode dup_node;
        dup_node.type = AstNodeTypeDup;
        ast.push(dup_node);
        token_index += 2;
        return true;
    }

    bool parse_out()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "out"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode out_node;
        out_node.type = AstNodeTypeOut;
        ast.push(out_node);
        token_index += 2;
        return true;
    }

    bool parse_push_nothing()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "push"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode push_nothing_node;
        push_nothing_node.type = AstNodeTypePushNothing;
        ast.push(push_nothing_node);
        token_index += 2;
        return true;
    }

    bool parse_ddup()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "ddup"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode ddup_node;
        ddup_node.type = AstNodeTypeDdup;
        ast.push(ddup_node);
        token_index += 2;
        return true;
    }

    bool parse_store()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "store"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode store_node;
        store_node.type = AstNodeTypeStore;
        ast.push(store_node);
        token_index += 2;
        return true;
    }

    bool parse_load()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "load"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode load_node;
        load_node.type = AstNodeTypeLoad;
        ast.push(load_node);
        token_index += 2;
        return true;
    }

    bool parse_call()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "call"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode call_node;
        call_node.type = AstNodeTypeCall;
        ast.push(call_node);
        token_index += 2;
        return true;
    }

    bool parse_ret()
    {
        if (token_index > tokens.size-2
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "ret"
            || tokens.data[token_index+1].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode ret_node;
        ret_node.type = AstNodeTypeRet;
        ast.push(ret_node);
        token_index += 2;
        return true;
    }

    bool parse_label()
    {
        if (token_index > tokens.size-3
            || tokens.data[token_index].type != TokenTypeName
            || tokens.data[token_index+1].type != TokenTypeColon
            || tokens.data[token_index+2].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode label_node;
        label_node.type = AstNodeTypeLabel;
        label_node.label = tokens.data[token_index].name.copy();
        ast.push(label_node);
        registered_labels.push(label_node.label);
        token_index += 3;
        return true;
    }

    CheckLabelsResult check_labels()
    {
        for (u64 i = 0; i < labels_to_check.size; i++)
        {
            if (!registered_labels.contains(labels_to_check.data[i]))
            {
                CheckLabelsResult result;
                result.all_good = false;
                result.missing_label = labels_to_check.data[i];
                return result;
            }
        }
        CheckLabelsResult result;
        result.all_good = true;
        return result;
    }
};

struct AstParsingResult
{
    bool success;
    Ast ast;
    String error;
};

AstParsingResult parse_ast(Tokens tokens)
{
    AstParsingState state;
    state.tokens = tokens;
    state.token_index = 0;
    state.ast = Ast::allocate();
    state.registered_labels = Strings::allocate();
    state.labels_to_check = Strings::allocate();

    while (state.token_index != tokens.size)
    {
        if (
            state.skip_new_lines()
                || state.parse_nop()
                || state.parse_push()
                || state.parse_pop()
                || state.parse_add()
                || state.parse_cmp()
                || state.parse_jl()
                || state.parse_jle()
                || state.parse_jeq()
                || state.parse_jge()
                || state.parse_jg()
                || state.parse_jne()
                || state.parse_jmp()
                || state.parse_dup()
                || state.parse_out()
                || state.parse_push_nothing()
                || state.parse_ddup()
                || state.parse_store()
                || state.parse_load()
                || state.parse_call()
                || state.parse_ret()
                || state.parse_label()
        )
        {
            continue;
        }

        AstParsingResult result;
        result.success = false;
        result.error = String::allocate();
        result.error.push("Expected a valid instruction on line ");
        result.error.push(state.tokens.data[state.token_index].line);
        return result;
    }

    auto check_labels_result = state.check_labels();
    if (!check_labels_result.all_good)
    {
        AstParsingResult result;
        result.success = false;
        result.error = String::allocate();
        result.error.push("Missing label: ");
        result.error.push(check_labels_result.missing_label);
        return result;
    }

    state.registered_labels.deallocate();
    state.labels_to_check.deallocate();

    AstParsingResult result;
    result.success = true;
    result.ast = state.ast;
    return result;
}
