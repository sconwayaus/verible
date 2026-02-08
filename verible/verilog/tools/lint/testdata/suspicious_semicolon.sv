// verilog_lint: waive default-nettype
module suspicious_semicolon ();
  initial begin
    // verilog_lint: waive explicit-begin
    if (x);
      $display("Hi");
  end
endmodule
