library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity rom is
    generic (code : work.types.T_MEMORY);

    port (
        address : in BIT_VECTOR(7 downto 0);
        output : out BIT_VECTOR(7 downto 0)
    );
end rom;

architecture rom_architecture of rom is
begin
    output <= code(to_integer(unsigned(to_stdlogicvector(address)))) when to_integer(unsigned(to_stdlogicvector(address))) < code'length else b"00000000";
end rom_architecture;
