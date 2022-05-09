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
    u64 line;

    // only for AstNodeTypePush
    PushNodeType push_type;
    union
    {
        u8 integer;
        String label;
    };

    String to_string()
    {
        auto result = String::allocate();
        switch (type)
        {
            case AstNodeTypeNop:
                result.push("nop");
                break;
            case AstNodeTypePush:
                result.push("push ");
                if (push_type == PushNodeTypeInteger)
                {
                    char buffer[20];
                    auto digits = int_to_string(buffer, integer);
                    for (u64 i = 0; i < digits; i++)
                    {
                        result.push(buffer[i]);
                    }
                }
                else // PushNodeTypeLabel
                {
                    result.push(label);
                }
                break;
            case AstNodeTypePop:
                result.push("pop");
                break;
            case AstNodeTypeAdd:
                result.push("add");
                break;
            case AstNodeTypeCmp:
                result.push("cmp");
                break;
            case AstNodeTypeJl:
                result.push("jl");
                break;
            case AstNodeTypeJle:
                result.push("jle");
                break;
            case AstNodeTypeJeq:
                result.push("jeq");
                break;
            case AstNodeTypeJge:
                result.push("jge");
                break;
            case AstNodeTypeJg:
                result.push("jg");
                break;
            case AstNodeTypeJne:
                result.push("jne");
                break;
            case AstNodeTypeJmp:
                result.push("jmp");
                break;
            case AstNodeTypeDup:
                result.push("dup");
                break;
            case AstNodeTypeOut:
                result.push("out");
                break;
            case AstNodeTypePushNothing:
                result.push("push");
                break;
            case AstNodeTypeDdup:
                result.push("ddup");
                break;
            case AstNodeTypeStore:
                result.push("store");
                break;
            case AstNodeTypeLoad:
                result.push("load");
                break;
            case AstNodeTypeCall:
                result.push("call");
                break;
            case AstNodeTypeRet:
                result.push("ret");
                break;
            case AstNodeTypeLabel:
                break;
        }
        return result;
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
        nop_node.line = tokens.data[token_index].line;
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
        push_node.line = tokens.data[token_index].line;
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
        pop_node.line = tokens.data[token_index].line;
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
        add_node.line = tokens.data[token_index].line;
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
        cmp_node.line = tokens.data[token_index].line;
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
        jl_node.line = tokens.data[token_index].line;
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
        jle_node.line = tokens.data[token_index].line;
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
        jeq_node.line = tokens.data[token_index].line;
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
        jge_node.line = tokens.data[token_index].line;
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
        jg_node.line = tokens.data[token_index].line;
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
        jne_node.line = tokens.data[token_index].line;
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
        jmp_node.line = tokens.data[token_index].line;
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
        dup_node.line = tokens.data[token_index].line;
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
        out_node.line = tokens.data[token_index].line;
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
        push_nothing_node.line = tokens.data[token_index].line;
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
        ddup_node.line = tokens.data[token_index].line;
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
        store_node.line = tokens.data[token_index].line;
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
        load_node.line = tokens.data[token_index].line;
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
        call_node.line = tokens.data[token_index].line;
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
        ret_node.line = tokens.data[token_index].line;
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
        label_node.line = tokens.data[token_index].line;
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
