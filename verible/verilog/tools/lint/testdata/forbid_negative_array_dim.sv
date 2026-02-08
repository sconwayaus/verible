// verilog_lint: waive default-nettype
module forbid_negative_array_dim ();
  reg [-1 : 0] x;
endmodule
