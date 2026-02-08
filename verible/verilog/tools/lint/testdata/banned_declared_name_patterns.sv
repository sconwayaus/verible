// verilog_lint: waive default-nettype
module banned_declared_name_patterns;
  module ILLEGALNAME;
  endmodule : ILLEGALNAME
endmodule : banned_declared_name_patterns
