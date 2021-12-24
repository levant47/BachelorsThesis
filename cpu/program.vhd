-- package program is
--     constant code : work.types.T_MEMORY := (
--         b"00000001", b"00000001", -- push 1
--         b"00000001", b"00000010", -- push 2
--         b"00000011", -- add
--         b"00000001", b"00000011", -- push 3
--         b"00000011" -- add
--     );
-- end program;

-- package program is
--     constant code : work.types.T_MEMORY := (
--         b"00000001", b"00001010", -- push 10
--         -- loop:
--         b"00001100", -- dup
--         b"00000001", b"00000000", -- push 0
--         b"00000100", -- cmp
--         b"00000111", b"00001101", -- jeq end_loop
--         b"00000001", b"11111111", -- push -1
--         b"00000011", -- add
--         b"00001011", b"00000010", -- jmp loop
--         -- end_loop:
--     );
-- end program;

package program is
    constant code : work.types.T_MEMORY := (
        b"00000001",
        b"00001010",
        b"00001100",
        b"00000001",
        b"00000000",
        b"00000100",
        b"00000111",
        b"00001101",
        b"00000001",
        b"11111111",
        b"00000011",
        b"00001011",
        b"00000010"
    );
end program;
