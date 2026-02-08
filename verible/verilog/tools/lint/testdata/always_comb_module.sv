// verilog_lint: waive default-nettype
module always_comb_module;
  always @* begin
    a = b + c;
  end
endmodule
