--------------------------------------------------------------------------------
--This testbench has been created by Ambikeshwar Srivastava, Rahul Paknikar 
--------------------------- FOSSEE, IIT Bombay ---------------------------------
--------------------------------------------------------------------------------

        library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;
        library work;
        use work.Vhpi_Foreign.all;
        use work.Utility_Package.all;
        use work.sock_pkg.all;

        entity attiny_85_nghdl_tb is
        generic( hex_path_tb : string );
end entity;

architecture attiny_85_nghdl_tb_beh of attiny_85_nghdl_tb is
	component attiny_85_nghdl is
        generic( hex_path : string );
		port(
				VCC: in std_logic;
				GND: in std_logic;
				clk: in std_logic;
				PB2: in std_logic_vector(0 downto 0);
				PB3: in std_logic_vector(0 downto 0);
				PB4: in std_logic_vector(0 downto 0);
				PB5: in std_logic_vector(0 downto 0);
				PB0: out std_logic_vector(0 downto 0);
				PB1: out std_logic_vector(0 downto 0)
				);
	end component;

	signal clk_s : std_logic := '0';
	signal VCC: std_logic;
	signal GND: std_logic;
	signal clk: std_logic;
	signal PB2: std_logic_vector(0 downto 0);
	signal PB3: std_logic_vector(0 downto 0);
	signal PB4: std_logic_vector(0 downto 0);
	signal PB5: std_logic_vector(0 downto 0);
	signal PB0: std_logic_vector(0 downto 0);
	signal PB1: std_logic_vector(0 downto 0);


begin

	u1 : attiny_85_nghdl 
		generic map(hex_path => hex_path_tb)
		port map(
				VCC => VCC,
				GND => GND,
				clk => clk,
				PB2 => PB2,
				PB3 => PB3,
				PB4 => PB4,
				PB5 => PB5,
				PB0 => PB0,
				PB1 => PB1
			);
	clk_s <= not clk_s after 5 us;
process
begin
report "Hex path in attiny 85" & hex_path_tb;
wait;
end process;
process
		variable sock_port : integer;
		type string_ptr is access string;
		variable sock_ip : string_ptr;
		begin
		sock_port := sock_port_fun;
		sock_ip := new string'(sock_ip_fun);
		Vhpi_Initialize(sock_port,Pack_String_To_Vhpi_String(sock_ip.all));
		wait until clk_s = '1';
		while true loop
			wait until clk_s = '0';
			Vhpi_Listen;
			wait for 1 us;
			Vhpi_Send;
		end loop;
		wait;
	end process;

	process
		variable count : integer:=0;
		variable VCC_v : VhpiString;
		variable GND_v : VhpiString;
		variable clk_v : VhpiString;
		variable PB2_v : VhpiString;
		variable PB3_v : VhpiString;
		variable PB4_v : VhpiString;
		variable PB5_v : VhpiString;
		variable PB0_v : VhpiString;
		variable PB1_v : VhpiString;
		variable obj_ref : VhpiString;
	begin
		while true loop
			wait until clk_s = '0';

			obj_ref := Pack_String_To_Vhpi_String("VCC");
			Vhpi_Get_Port_Value(obj_ref,VCC_v,1);
			VCC <= To_Std_Logic(VCC_v);

			obj_ref := Pack_String_To_Vhpi_String("GND");
			Vhpi_Get_Port_Value(obj_ref,GND_v,1);
			GND <= To_Std_Logic(GND_v);

			obj_ref := Pack_String_To_Vhpi_String("clk");
			Vhpi_Get_Port_Value(obj_ref,clk_v,1);
			clk <= To_Std_Logic(clk_v);

			obj_ref := Pack_String_To_Vhpi_String("PB2");
			Vhpi_Get_Port_Value(obj_ref,PB2_v,1);
			PB2 <= Unpack_String(PB2_v,1);

			obj_ref := Pack_String_To_Vhpi_String("PB3");
			Vhpi_Get_Port_Value(obj_ref,PB3_v,1);
			PB3 <= Unpack_String(PB3_v,1);

			obj_ref := Pack_String_To_Vhpi_String("PB4");
			Vhpi_Get_Port_Value(obj_ref,PB4_v,1);
			PB4 <= Unpack_String(PB4_v,1);

			obj_ref := Pack_String_To_Vhpi_String("PB5");
			Vhpi_Get_Port_Value(obj_ref,PB5_v,1);
			PB5 <= Unpack_String(PB5_v,1);

			wait for 1 us;
			PB0_v := Pack_String_To_Vhpi_String(Convert_SLV_To_String(PB0));
			obj_ref := Pack_String_To_Vhpi_String("PB0");
			Vhpi_Set_Port_Value(obj_ref,PB0_v,1);

			PB1_v := Pack_String_To_Vhpi_String(Convert_SLV_To_String(PB1));
			obj_ref := Pack_String_To_Vhpi_String("PB1");
			Vhpi_Set_Port_Value(obj_ref,PB1_v,1);

			report "Iteration - "& integer'image(count) severity note;
			count := count + 1;
		end loop;
	end process;

end architecture;
