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

#include "verible/verilog/analysis/checkers/default-nettype-rule.h"

#include <initializer_list>

#include "gtest/gtest.h"
#include "verible/common/analysis/linter-test-utils.h"
#include "verible/common/analysis/token-stream-linter-test-utils.h"
#include "verible/verilog/analysis/verilog-analyzer.h"
#include "verible/verilog/parser/verilog-token-enum.h"

namespace verilog {
namespace analysis {
namespace {

using verible::LintTestCase;
using verible::RunApplyFixCases;
using verible::RunLintTestCases;

TEST(DefaultNetTypeRuleTest, Accepts) {
  const std::initializer_list<LintTestCase> kTestCases = {
      {"`default_nettype none\nmodule a; endmodule"},
      {"/* Some\nComment\n*/\nimport some_package::*;\n`default_nettype "
       "none\nimport another_package;\nclass#() a(); endclass"},
      {"import some_package::a_thing;\n`default_nettype "
       "none\nimport another_package;\ninterface#() a();endinterface"},
      {"// Comment\n import some_package::a_thing;\n`default_nettype "
       "none\nimport another_package;\nclass#() a();endclass"},
      {"package my_pkg; localparam int a = 0; endpackage"},
  };
  RunLintTestCases<VerilogAnalyzer, DefaultNetTypeRule>(kTestCases);
}

TEST(DefaultNetTypeRuleTest, Rejects) {
  const std::initializer_list<LintTestCase> kTestCases = {
      {{TK_module, "module"}, " a; endmodule"},
      {"/* Some\nComment\n*/\nimport some_package::*;\nimport "
       "another_package\n\n",
       {TK_module, "module"},
       "#() a();"},
      {"/* Some\nComment\n*/\nimport some_package::*;\nimport "
       "another_package\n\n",
       {TK_module, "module"},
       "#() a(); endmodule\n`default_nettype none\n"},
      {{TK_class, "class"}, " SomeClass;\nendclass"},
      {{TK_interface, "interface"}, " a_interface;\nendinterface"},

      // `default_nettype wire
      {"`default_nettype ", {TK_wire, "wire"}, "\nmodule a; endmodule"},
      {"/* Some\nComment\n*/\n`default_nettype ",
       {TK_wire, "wire"},
       "\nimport some_package::*;\n",
       "import another_package::*;\nmodule a#()(); endmodule"},
      {"/* Some\nComment\n*/\n`default_nettype ",
       {TK_wire, "wire"},
       "\nimport some_package::*;\n",
       "import another_package::*;\nmodule a#()(); endmodule\ndefault_nettype "
       "none\n"},
      {"`default_nettype ", {TK_wire, "wire"}, "\nclass SomeClass;\nendclass"},
      {"`default_nettype ",
       {TK_wire, "wire"},
       "\ninterface a_interface;\nendinterface"},
  };
  RunLintTestCases<VerilogAnalyzer, DefaultNetTypeRule>(kTestCases);
}

TEST(DefaultNetTypeRuleTest, ApplyAutoFix) {
  // Alternatives the auto fix offers
  constexpr int kDefaultNetTypeAutofix = 0;
  const std::initializer_list<verible::AutoFixInOut> kTestCases = {
      {"module a; endmodule", "`default_nettype none\nmodule a; endmodule",
       kDefaultNetTypeAutofix},
      {"`default_nettype wire\nmodule a; endmodule",
       "`default_nettype none\nmodule a; endmodule", kDefaultNetTypeAutofix},
      {"/* Some\nComment\n*/\nimport some_package::*;\nimport "
       "another_package;\nclass#() a(); endclass",
       "/* Some\nComment\n*/\n`default_nettype none\nimport "
       "some_package::*;\nimport another_package;\nclass#() a(); endclass",
       kDefaultNetTypeAutofix},
      {"import some_package::a_thing;\n`default_nettype tri\nimport "
       "another_package;\n\ninterface#() a();endinterface",
       "import some_package::a_thing;\n`default_nettype none\nimport "
       "another_package;\n\ninterface#() a();endinterface",
       kDefaultNetTypeAutofix},
      {"// Comment\n import some_package::a_thing;\n`default_nettype "
       "wand\nimport another_package;\nclass#() a();endclass",
       "// Comment\n import some_package::a_thing;\n`default_nettype "
       "none\nimport another_package;\nclass#() a();endclass",
       kDefaultNetTypeAutofix},
  };
  RunApplyFixCases<VerilogAnalyzer, DefaultNetTypeRule>(kTestCases, "");
}

}  // namespace
}  // namespace analysis
}  // namespace verilog
