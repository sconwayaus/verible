// UVM macro calls should not be followed by ';'
// verilog_lint: waive default-nettype
class c;
  function f();
    `uvm_info("msg_id", "message", UVM_LOW);
  endfunction
endclass
