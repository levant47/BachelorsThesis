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
    AstNodeTypeLabel,
};

struct AstNode
{
    AstNodeType type;
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
                printf("Push %hhu", integer);
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
                printf("Jl ");
                label.print();
                break;
            case AstNodeTypeJle:
                printf("Jle ");
                label.print();
                break;
            case AstNodeTypeJeq:
                printf("Jeq ");
                label.print();
                break;
            case AstNodeTypeJge:
                printf("Jge ");
                label.print();
                break;
            case AstNodeTypeJg:
                printf("Jg ");
                label.print();
                break;
            case AstNodeTypeJne:
                printf("Jne ");
                label.print();
                break;
            case AstNodeTypeJmp:
                printf("Jmp ");
                label.print();
                break;
            case AstNodeTypeDup:
                printf("Dup");
                break;
            case AstNodeTypeOut:
                printf("Out");
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
            || tokens.data[token_index+1].type != TokenTypeInteger
            || tokens.data[token_index+2].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode push_node;
        push_node.type = AstNodeTypePush;
        push_node.integer = tokens.data[token_index+1].integer;
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
        if (token_index > tokens.size-3
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "jl"
            || tokens.data[token_index+1].type != TokenTypeName
            || tokens.data[token_index+2].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode jl_node;
        jl_node.type = AstNodeTypeJl;
        jl_node.label = tokens.data[token_index+1].name.copy();
        ast.push(jl_node);
        if (!registered_labels.contains(jl_node.label))
        {
            labels_to_check.push(jl_node.label);
        }
        token_index += 3;
        return true;
    }

    bool parse_jle()
    {
        if (token_index > tokens.size-3
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "jle"
            || tokens.data[token_index+1].type != TokenTypeName
            || tokens.data[token_index+2].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode jle_node;
        jle_node.type = AstNodeTypeJle;
        jle_node.label = tokens.data[token_index+1].name.copy();
        ast.push(jle_node);
        if (!registered_labels.contains(jle_node.label))
        {
            labels_to_check.push(jle_node.label);
        }
        token_index += 3;
        return true;
    }

    bool parse_jeq()
    {
        if (token_index > tokens.size-3
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "jeq"
            || tokens.data[token_index+1].type != TokenTypeName
            || tokens.data[token_index+2].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode jeq_node;
        jeq_node.type = AstNodeTypeJeq;
        jeq_node.label = tokens.data[token_index+1].name.copy();
        ast.push(jeq_node);
        if (!registered_labels.contains(jeq_node.label))
        {
            labels_to_check.push(jeq_node.label);
        }
        token_index += 3;
        return true;
    }

    bool parse_jge()
    {
        if (token_index > tokens.size-3
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "jge"
            || tokens.data[token_index+1].type != TokenTypeName
            || tokens.data[token_index+2].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode jge_node;
        jge_node.type = AstNodeTypeJge;
        jge_node.label = tokens.data[token_index+1].name.copy();
        ast.push(jge_node);
        if (!registered_labels.contains(jge_node.label))
        {
            labels_to_check.push(jge_node.label);
        }
        token_index += 3;
        return true;
    }

    bool parse_jg()
    {
        if (token_index > tokens.size-3
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "jg"
            || tokens.data[token_index+1].type != TokenTypeName
            || tokens.data[token_index+2].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode jg_node;
        jg_node.type = AstNodeTypeJg;
        jg_node.label = tokens.data[token_index+1].name.copy();
        ast.push(jg_node);
        if (!registered_labels.contains(jg_node.label))
        {
            labels_to_check.push(jg_node.label);
        }
        token_index += 3;
        return true;
    }

    bool parse_jne()
    {
        if (token_index > tokens.size-3
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "jne"
            || tokens.data[token_index+1].type != TokenTypeName
            || tokens.data[token_index+2].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode jne_node;
        jne_node.type = AstNodeTypeJne;
        jne_node.label = tokens.data[token_index+1].name.copy();
        ast.push(jne_node);
        if (!registered_labels.contains(jne_node.label))
        {
            labels_to_check.push(jne_node.label);
        }
        token_index += 3;
        return true;
    }

    bool parse_jmp()
    {
        if (token_index > tokens.size-3
            || tokens.data[token_index].type != TokenTypeName || tokens.data[token_index].name != "jmp"
            || tokens.data[token_index+1].type != TokenTypeName
            || tokens.data[token_index+2].type != TokenTypeNewLine)
        {
            return false;
        }
        AstNode jmp_node;
        jmp_node.type = AstNodeTypeJmp;
        jmp_node.label = tokens.data[token_index+1].name.copy();
        ast.push(jmp_node);
        if (!registered_labels.contains(jmp_node.label))
        {
            labels_to_check.push(jmp_node.label);
        }
        token_index += 3;
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

    bool check_labels()
    {
        for (u64 i = 0; i < labels_to_check.size; i++)
        {
            if (!registered_labels.contains(labels_to_check.data[i]))
            {
                return false;
            }
        }
        return true;
    }
};

struct AstParsingResult
{
    bool success;
    Ast ast;
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
                || state.parse_label()
        )
        {
            continue;
        }

        AstParsingResult result;
        result.success = false;
        return result;
    }

    if (!state.check_labels())
    {
        AstParsingResult result;
        result.success = false;
        return result;
    }

    state.registered_labels.deallocate();
    state.labels_to_check.deallocate();

    AstParsingResult result;
    result.success = true;
    result.ast = state.ast;
    return result;
}
