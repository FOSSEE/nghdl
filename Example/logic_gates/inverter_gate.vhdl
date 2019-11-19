library ieee;
use ieee.std_logic_1164.all;

entity inverter_gate is
    port ( i: in std_logic;
           o: out std_logic);
end inverter_gate;

architecture beh of inverter_gate is
begin
    o <= not i;
end beh;


