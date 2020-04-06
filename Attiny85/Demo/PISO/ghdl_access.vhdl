-- By Ashutosh Jha

package ghdl_access is
  -- Defines a pointer to an integer:
  type int_access is access integer;


  function get_ptr return int_access;
    attribute foreign of get_ptr :
      function is "VHPIDIRECT get_ptr";

  function get_ptr1 return int_access;
    attribute foreign of get_ptr1 :
      function is "VHPIDIRECT get_ptr1";

  function get_ptr2 return int_access;
    attribute foreign of get_ptr2 :
      function is "VHPIDIRECT get_ptr2";

  function get_ptr3 return int_access;
    attribute foreign of get_ptr3 :
      function is "VHPIDIRECT get_ptr3";

  -- declaration of functions in C
    procedure output(f : integer);
      attribute foreign of output :
        procedure is "VHPIDIRECT output";
  

  -- create variables aliased to the variable in C 
  shared variable var1 : int_access := get_ptr;
  shared variable var2 : int_access := get_ptr1;
  shared variable var3 : int_access := get_ptr2;
  shared variable var4 : int_access := get_ptr3;
end ghdl_access;

package body ghdl_access is
  function get_ptr return int_access is
  begin
    assert false report "VHPI" severity failure;
  end get_ptr;

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
  
  procedure output(f : integer) is
  begin
    assert false report "VHPI" severity failure;
  end output;
  
end ghdl_access;
