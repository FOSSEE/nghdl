--by Ashutosh Jha
library ieee;
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all;
library work;
use work.ghdl_access.all;


entity piso is 
port(a : in std_logic;
     b : in std_logic;
     c : in std_logic;
     d : in std_logic;
     clk : in std_logic;
     o : out std_logic_vector(0 downto 0)
     );
end piso;

architecture bhv of piso is 
	signal count: integer := 0;
begin
process (clk) 
  begin
	if (clk = '1') then
		  count <= count + 1;
		  output(2);
		  if(count = 0) then
			o <= std_logic_vector(to_unsigned(var1.all, o'length));

		  elsif(count = 1) then
			o <= std_logic_vector(to_unsigned(var2.all, o'length));

		  elsif(count = 2) then
			o <= std_logic_vector(to_unsigned(var3.all, o'length));

		  elsif(count = 3) then
			o <= std_logic_vector(to_unsigned(var4.all, o'length));
			count <= 0;
		  end if;
	elsif(clk = '0') then
		o <= "0";
	end if;
end process;
end bhv;
