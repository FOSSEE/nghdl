--	This helper function was written by Ashutosh Jha for ATtiny series of microcontrollers
--	This codes facilitates exchange of functions and variables betweent the VHDL and C models
--	Latest edit(AJ) - 3:27 AM, 4/3/2020
--      This was modified by SUMANTO KAR for ATtiny85 microcontroller

package ghdl_access is


  -- Defines a pointer to an integer:
type int_access is access integer;


function get_ptr0 return int_access;
	attribute foreign of get_ptr0 :
		function is "VHPIDIRECT get_ptr0";

function get_ptr1 return int_access;
	attribute foreign of get_ptr1 :
		function is "VHPIDIRECT get_ptr1";

function get_ptr2 return int_access;
	attribute foreign of get_ptr2 :
		function is "VHPIDIRECT get_ptr2";

function get_ptr3 return int_access;
	attribute foreign of get_ptr3 :
		function is "VHPIDIRECT get_ptr3";

function get_ptr4 return int_access;
	attribute foreign of get_ptr4 :
		function is "VHPIDIRECT get_ptr4";

function get_ptr5 return int_access;
	attribute foreign of get_ptr5 :
		function is "VHPIDIRECT get_ptr5";


  -- declaration of functions in C
procedure output(f : integer);
	attribute foreign of output :
        	procedure is "VHPIDIRECT output";
   
procedure MapToRam(f : integer);
	attribute foreign of MapToRam :
        	procedure is "VHPIDIRECT MapToRam";
  
procedure input(f : integer);
	attribute foreign of input :
        	procedure is "VHPIDIRECT input";

--procedure SetPins(f : integer);
--	attribute foreign of SetPins :
--        	procedure is "VHPIDIRECT SetPins";
  

  -- create variables aliased to the variable in C 
	shared variable var0 : int_access := get_ptr0;
	shared variable var1 : int_access := get_ptr1;
	shared variable var2 : int_access := get_ptr2;
	shared variable var3 : int_access := get_ptr3;
	shared variable var4 : int_access := get_ptr4;
	shared variable var5 : int_access := get_ptr5;

end ghdl_access;

package body ghdl_access is
	function get_ptr0 return int_access is
	begin
		assert false report "VHPI" severity failure;
	end get_ptr0;

	function get_ptr1 return int_access is
	begin
		assert false report "VHPI" severity failure;
	end get_ptr1;

	function get_ptr2 return int_access is
	begin
		assert false report "VHPI" severity failure;
	end get_ptr2;

	function get_ptr3 return int_access is
	begin
		assert false report "VHPI" severity failure;
	end get_ptr3;

	function get_ptr4 return int_access is
	begin
		assert false report "VHPI" severity failure;
	end get_ptr4;

	function get_ptr5 return int_access is
	begin
		assert false report "VHPI" severity failure;
	end get_ptr5;
  
	procedure output(f : integer) is
	begin
		assert false report "VHPI" severity failure;
	end output;

	procedure MapToRam(f : integer) is
	begin
		assert false report "VHPI" severity failure;
	end MapToRam;

	procedure input(f : integer) is
	begin
		assert false report "VHPI" severity failure;
	end input;

--	procedure SetPins(f : integer) is
--	begin
--		assert false report "VHPI" severity failure;
--	end SetPins;
  
end ghdl_access;
