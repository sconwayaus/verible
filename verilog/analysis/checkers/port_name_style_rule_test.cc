// Copyright 2017-2020 The Verible Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "verilog/analysis/checkers/port_name_style_rule.h"

#include <initializer_list>

#include "absl/strings/match.h"
#include "common/analysis/linter_test_utils.h"
#include "common/analysis/syntax_tree_linter_test_utils.h"
#include "gtest/gtest.h"
#include "verilog/analysis/verilog_analyzer.h"
#include "verilog/parser/verilog_token_enum.h"

namespace verilog {
namespace analysis {
namespace {

using verible::LintTestCase;
using verible::RunConfiguredLintTestCases;
using verible::RunLintTestCases;

// Tests that PortNameStyleRule correctly accepts valid names.
TEST(PortNameStyleRule, AcceptTests) {
  const std::initializer_list<LintTestCase> kTestCases = {
      {""},
      {"module t (input logic name_i); endmodule;"},
      {"module t (output logic abc_o); endmodule;"},
      {"module t (inout logic xyz_io); endmodule;"},
      {"module t (input logic long_name_i); endmodule;"},
      {"module t (output logic long_abc_o); endmodule;"},
      {"module t (inout logic long_xyz_io); endmodule;"},
      {"module t (input logic name_ni); endmodule;"},
      {"module t (output logic abc_no); endmodule;"},
      {"module t (inout logic xyz_nio); endmodule;"},
      {"module t (input logic name_pi); endmodule;"},
      {"module t (output logic abc_po); endmodule;"},
      {"module t (inout logic xyz_pio); endmodule;"},
      {"module t (input logic [7:0] name_i); endmodule;"},
      {"module t (output logic [2:0] abc_o); endmodule;"},
      {"module t (inout logic [3:0] xyz_io); endmodule;"},
      {"module t (input logic [7:0] name_ni); endmodule;"},
      {"module t (output logic [2:0] abc_no); endmodule;"},
      {"module t (inout logic [3:0] xyz_nio); endmodule;"},
      {"module t (input logic [7:0] name_pi); endmodule;"},
      {"module t (output logic [2:0] abc_po); endmodule;"},
      {"module t (inout logic [3:0] xyz_pio); endmodule;"},
      {"module t (input bit name_i); endmodule;"},
      {"module t (output bit abc_o); endmodule;"},
      {"module t (inout bit xyz_io); endmodule;"},

      {"module t (some_interface.a b_a); endmodule;"},
      {"module t (some_interface.some_mport c_some_mport); endmodule;"},
      {"module t (some_interface intf); endmodule;"},  // interface without
                                                       // modport
      {"module t (interface d); endmodule;"},          // A generic interface

      {"module t (input logic name_i,\n"
       "output logic abc_o,\n"
       "inout logic xyz_io,\n"
       "input logic [7:0] namea_i,\n"
       "output logic [2:0] abca_o,\n"
       "inout logic [3:0] xyza_io,\n"
       "input bit nameb_i,\n"
       "output bit abcb_o,\n"
       "inout bit xyzb_io);\n"
       "endmodule;"},
  };
  RunLintTestCases<VerilogAnalyzer, PortNameStyleRule>(kTestCases);
}

// Tests that PortNameStyleRule rejects invalid names.
TEST(PortNameStyleRule, RejectTests) {
  constexpr int kToken = SymbolIdentifier;
  const std::initializer_list<LintTestCase> kTestCases = {
      // General tests
      {"module t (input logic ", {kToken, "name"}, "); endmodule;"},
      {"module t (output logic ", {kToken, "abc"}, "); endmodule;"},
      {"module t (inout logic ", {kToken, "xyz"}, "); endmodule;"},
      {"module t (input logic [7:0] ", {kToken, "name"}, "); endmodule;"},
      {"module t (output logic [2:0] ", {kToken, "abc"}, "); endmodule;"},
      {"module t (inout logic [3:0] ", {kToken, "xyz"}, "); endmodule;"},
      {"module t (input bit ", {kToken, "name"}, "); endmodule;"},
      {"module t (output bit ", {kToken, "abc"}, "); endmodule;"},
      {"module t (inout bit ", {kToken, "xyz"}, "); endmodule;"},

      {"module t (input logic ", {kToken, "_i"}, "); endmodule;"},
      {"module t (output logic ", {kToken, "_o"}, "); endmodule;"},
      {"module t (inout logic ", {kToken, "_io"}, "); endmodule;"},

      {"module t (input logic ", {kToken, "namei"}, "); endmodule;"},
      {"module t (input logic ", {kToken, "nam_ei"}, "); endmodule;"},
      {"module t (input logic ", {kToken, "name_o"}, "); endmodule;"},
      {"module t (input logic ", {kToken, "name_io"}, "); endmodule;"},
      {"module t (input logic ", {kToken, "name_no"}, "); endmodule;"},
      {"module t (input logic ", {kToken, "name_nio"}, "); endmodule;"},
      {"module t (input logic ", {kToken, "name_po"}, "); endmodule;"},
      {"module t (input logic ", {kToken, "name_pio"}, "); endmodule;"},

      {"module t (output logic ", {kToken, "nameo"}, "); endmodule;"},
      {"module t (output logic ", {kToken, "nam_eo"}, "); endmodule;"},
      {"module t (output logic ", {kToken, "name_i"}, "); endmodule;"},
      {"module t (output logic ", {kToken, "name_oi"}, "); endmodule;"},
      {"module t (output logic ", {kToken, "name_ni"}, "); endmodule;"},
      {"module t (output logic ", {kToken, "name_nio"}, "); endmodule;"},
      {"module t (output logic ", {kToken, "name_pi"}, "); endmodule;"},
      {"module t (output logic ", {kToken, "name_pio"}, "); endmodule;"},

      {"module t (input logic ",
       {kToken, "name"},
       ",\n"
       "output logic abc_o,\n"
       "inout logic ",
       {kToken, "xyz"},
       ",\n"
       "input logic [7:0] namea_i,\n"
       "output logic [2:0] ",
       {kToken, "abca"},
       ",\n"
       "inout logic [3:0] xyza_io,\n"
       "input bit ",
       {kToken, "nameb"},
       ",\n"
       "output bit ",
       {kToken, "abcb"},
       ",\n"
       "inout bit xyzb_io);\n"
       "endmodule;"},
      // Invalid casing
      {"module t (input logic ", {kToken, "name_I"}, "); endmodule;"},
      {"module t (output logic ", {kToken, "abc_O"}, "); endmodule;"},
      {"module t (inout logic ", {kToken, "xyz_IO"}, "); endmodule;"},

      // Mismatched suffix tests
      {"module t (input logic ", {kToken, "name_o"}, "); endmodule;"},
      {"module t (input logic ", {kToken, "name_io"}, "); endmodule;"},
      {"module t (output logic ", {kToken, "name_i"}, "); endmodule;"},
      {"module t (output logic ", {kToken, "name_io"}, "); endmodule;"},
      {"module t (inout logic ", {kToken, "name_i"}, "); endmodule;"},
      {"module t (inout logic ", {kToken, "name_o"}, "); endmodule;"},
  };
  RunLintTestCases<VerilogAnalyzer, PortNameStyleRule>(kTestCases);
}

TEST(PortNameStyleRule, InterfaceModPortDefault) {
  const std::initializer_list<LintTestCase> kTestCases = {
      {"module t (some_interface.a b_a); endmodule;"},
      {"module t (some_interface.some_mport c_some_mport); endmodule;"},
      {"module t (some_interface intf); endmodule;"},  // interface without
                                                       // modport
      {"module t (interface d); endmodule;"},          // A generic interface

      {"module t (some_interface.a b_c); endmodule;"},
      {"module t (some_interface.d e); endmodule;"},
      {"module t (some_interface.f g_); endmodule;"},
      {"module t (some_interface.h _h_i); endmodule;"},
      {"module t (some_interface.j j_k); endmodule;"},
      {"module t (some_intf.mport xyz_mport); endmodule;"},
      {"module t (some_intf.abc name_i); endmodule;"},
      {"module t (some_intf.abc name_o); endmodule;"},
      {"module t (some_intf.abc name_io); endmodule;"},
  };
  RunLintTestCases<VerilogAnalyzer, PortNameStyleRule>(kTestCases);
}

TEST(PortNameStyleRule, InterfaceModPortEnabled) {
  constexpr int kToken = SymbolIdentifier;
  const std::initializer_list<LintTestCase> kTestCases = {
      {"module t (some_interface.a b_a); endmodule;"},
      {"module t (some_interface.some_mport c_some_mport); endmodule;"},
      {"module t (some_interface intf); endmodule;"},  // interface without
                                                       // modport
      {"module t (interface d); endmodule;"},          // A generic interface

      {"module t (some_interface.a ", {kToken, "b_c"}, "); endmodule;"},
      {"module t (some_interface.d ", {kToken, "e"}, "); endmodule;"},
      {"module t (some_interface.f ", {kToken, "g_"}, "); endmodule;"},
      {"module t (some_interface.h ", {kToken, "_h_i"}, "); endmodule;"},
      {"module t (some_interface.j ", {kToken, "j_k"}, "); endmodule;"},
      {"module t (some_intf.mport ", {kToken, "xyz_MPORT"}, "); endmodule;"},
      {"module t (some_intf.abc ", {kToken, "name_i"}, "); endmodule;"},
      {"module t (some_intf.abc ", {kToken, "name_o"}, "); endmodule;"},
      {"module t (some_intf.abc ", {kToken, "name_io"}, "); endmodule;"},
  };
  RunConfiguredLintTestCases<VerilogAnalyzer, PortNameStyleRule>(
      kTestCases, "enable_interface_modport_suffix:true");
}

TEST(PortNameStyleRule, InterfaceModPortEnabledAndUpperSnakeCase) {
  constexpr int kToken = SymbolIdentifier;
  const std::initializer_list<LintTestCase> kTestCases = {
      {"module t (some_interface.a B_A); endmodule;"},
      {"module t (some_interface.some_mport C_SOME_MPORT); endmodule;"},
      {"module t (some_interface intf); endmodule;"},  // interface without
                                                       // modport
      {"module t (interface D); endmodule;"},          // A generic interface

      {"module t (some_interface.a ", {kToken, "b_a"}, "); endmodule;"},
      {"module t (some_interface.d ", {kToken, "e"}, "); endmodule;"},
      {"module t (some_interface.f ", {kToken, "g_"}, "); endmodule;"},
      {"module t (some_interface.h ", {kToken, "_H_I"}, "); endmodule;"},
      {"module t (some_interface.j ", {kToken, "j_k"}, "); endmodule;"},
      {"module t (some_intf.mport ", {kToken, "xyz_MPORT"}, "); endmodule;"},
      {"module t (some_intf.abc ", {kToken, "name_i"}, "); endmodule;"},
      {"module t (some_intf.abc ", {kToken, "name_o"}, "); endmodule;"},
      {"module t (some_intf.abc ", {kToken, "name_io"}, "); endmodule;"},
  };
  RunConfiguredLintTestCases<VerilogAnalyzer, PortNameStyleRule>(
      kTestCases,
      "enable_interface_modport_suffix:true;interface_style_regex:[A-Z_0-9]+");
}

TEST(PortNameStyleRule, Configuration) {
  PortNameStyleRule rule;
  absl::Status status;
  EXPECT_TRUE((status = rule.Configure("")).ok()) << status.message();
  EXPECT_TRUE((status = rule.Configure("input_style_regex:[A-Z_0-9]+_IN")).ok())
      << status.message();
  EXPECT_TRUE(
      (status = rule.Configure("output_style_regex:[A-Z_0-9]+_OUT")).ok())
      << status.message();
  EXPECT_TRUE(
      (status = rule.Configure("inout_style_regex:[A-Z_0-9]+_INOUT")).ok())
      << status.message();
  EXPECT_TRUE(
      (status = rule.Configure("interface_style_regex:[A-Z_0-9]+")).ok())
      << status.message();
  EXPECT_TRUE(
      (status = rule.Configure("enable_interface_modport_suffix:true")).ok())
      << status.message();

  EXPECT_FALSE((status = rule.Configure("foo:string")).ok());
  EXPECT_TRUE(absl::StrContains(status.message(), "supported parameter"));
}

}  // namespace
}  // namespace analysis
}  // namespace verilog
