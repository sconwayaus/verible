// verilog_lint: waive default-nettype
module long_line;
initial begin
  aaaaaaaaaaaaaaaaaaa[12341234] <= cccccccccccccccccccccccc + ddddddddddddddddd * eeeeeeeeeeeeeeeeee;
end
endmodule
