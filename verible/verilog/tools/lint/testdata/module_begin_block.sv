// verilog_lint: waive default-nettype
module module_begin_block;
  wire foobar;
  begin   // LRM-invalid syntax
    wire barfoo;
  end
endmodule
