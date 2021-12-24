#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.cpp"
#include "tokenizer.cpp"
#include "ast_parser.cpp"
#include "binary_backend.cpp"

String read_whole_file(const char* file_path)
{
    auto file = fopen(file_path, "rb");
    if (file == NULL)
    {
        panic("File does not exist");
    }

    fseek(file, 0, SEEK_END);
    auto file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    auto contents = String::allocate(file_size);
    fread(contents.data, file_size, 1, file);
    contents.size = file_size;
    fclose(file);

    return contents;
}

String get_program_directory_from_argv(char** argv)
{
    auto result = String::allocate(64);
    result.push(argv[0]);
    for (u64 i = 0; i < result.size; i++)
    {
        if (result.data[result.size-i-1] == '/')
        {
            result.size = result.size - i - 1;
            return result;
        }
    }
    panic("Failed to get program directory");
    return result;
}

int main(s32 argc, char** argv)
{
    auto program_directory = get_program_directory_from_argv(argv);
    auto source_path = program_directory.copy();
    source_path.push("/samples/3.asm");
    source_path.make_c_string();
    auto source = read_whole_file(source_path.data);

    auto tokenization_result = tokenize(source);
    if (!tokenization_result.success)
    {
        panic("Tokenization failed");
    }

    auto ast_parsing_result = parse_ast(tokenization_result.tokens);
    if (!ast_parsing_result.success)
    {
        panic("Parsing AST failed");
    }

    auto binary_result = compile_to_binary(ast_parsing_result.ast);

    binary_result.print_vhdl();

    return 0;
}
