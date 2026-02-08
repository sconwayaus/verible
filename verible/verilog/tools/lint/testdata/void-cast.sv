// verilog_lint: waive default-nettype
class foo;
  function void bar();
    void'(randomize());
  endfunction
endclass
