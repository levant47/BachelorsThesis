library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.numeric_std_unsigned.all;

entity rom is
    generic (code : work.types.T_MEMORY);

    port (
        address : in STD_ULOGIC_VECTOR(7 downto 0);
        output : out STD_ULOGIC_VECTOR(7 downto 0)
    );
end rom;

architecture rom_architecture of rom is
begin
    output <= code(to_integer(address));
end rom_architecture;
