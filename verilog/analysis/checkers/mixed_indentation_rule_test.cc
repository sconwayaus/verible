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

#include "verilog/analysis/checkers/mixed_indentation_rule.h"

#include <initializer_list>

#include "common/analysis/linter_test_utils.h"
#include "common/analysis/text_structure_linter_test_utils.h"
#include "gtest/gtest.h"
#include "verilog/analysis/verilog_analyzer.h"
#include "verilog/parser/verilog_token_enum.h"

namespace verilog {
namespace analysis {
namespace {

using verible::LintTestCase;
using verible::RunLintTestCases;

TEST(MixedIndentationRuleTest, AcceptsText) {
  const std::initializer_list<LintTestCase> kTestCases = {
      // 4 Spaces
      { "/*\n"
        "*  4 Spaces: AcceptTest\n"
        "    *\t\t  */\n"
        "\n"
        "// Comment\n"
        "// Another comment   with multiple spaces         \n"
        "// Another comment\t\twith tabs	in it\t\t\n"
        "        // Comment only with leading spaces\n"
        "\n"
        "module test();\n"
        "    int a;\n"
        "\n"
        "    some_module #(\n"
        "            .DATA_W         (),\n"
        "            .ADDR_W         ())\n"
        "        rom (        // Some comment\n"
        "            .addr_in        (),\n"
        "            .data_out       ());\n"
        "\n"
        "    always_comb /* some inline comment */ begin\n"
        "        for(int i = 0; i < N; i++) begin /*\n"
        "            $display(\"     \");        // Should ignore quoted text..\n"
        "            $display(\"            \");*/\n"
        "            $display(\"\");            // Comment\n"
        "        end\n"
        "        if(a) begin\n"
        "            a++;\n"
        "        end\n"
        "    end\n"
        "\n"
        "    initial $display(\"	\");     /*	*/ // some comment\n"
        "    initial $display(\"\");   //			Some comment\n"
        "endmodule\n"},
      
      // 3 Spaces
      { "/*\n"
        "*  3 Spaces: AcceptTest\n"
        "   *\t\t  */\n"
        "\n"
        "// Comment\n"
        "// Another comment   with multiple spaces       \n"
        "// Another comment\t\twith tabs	in it\t\t\n"
        "      // Comment only with leading spaces\n"
        "\n"
        "module test();\n"
        "   int a;\n"
        "\n"
        "   some_module #(\n"
        "         .DATA_W       (),\n"
        "         .ADDR_W       ())\n"
        "      rom (      // Some comment\n"
        "         .addr_in      (),\n"
        "         .data_out      ());\n"
        "\n"
        "   always_comb /* some inline comment */ begin\n"
        "      for(int i = 0; i < N; i++) begin /*\n"
        "         $display(\"    \");      // Should ignore quoted text..\n"
        "         $display(\"         \");*/\n"
        "         $display(\"\");         // Comment\n"
        "      end\n"
        "      if(a) begin\n"
        "         a++;\n"
        "      end\n"
        "   end\n"
        "\n"
        "   initial $display(\"	\");    /*	*/ // some comment\n"
        "   initial $display(\"\");   //			Some comment\n"
        "endmodule\n"},

      // Tabs
      { "/*\n"
        "*  Tabs: AcceptTest\n"
        "	*\t\t  */\n"
        "\n"
        "// Comment\n"
        "// Another comment   with multiple spaces         \n"
        "// Another comment\t\twith tabs	in it\t\t\n"
        "\t\t\t// Comment only with leading tabs\n"
        "\n"
        "module test();\n"
        "	int a;\n"
        "\n"
        "	some_module #(\n"
        "			.DATA_W			(),\n"
        "			.ADDR_W			())\n"
        "		rom (		// Some comment\n"
        "			.addr_in		(),\n"
        "			.data_out		());\n"
        "\n"
        "	always_comb /* some inline comment */ begin\n"
        "		for(int i = 0; i < N; i++) begin /*\n"
        "			$display(\"		\");		// Should ignore quoted text..\n"
        "			$display(\"        \");*/\n"
        "			$display(\"\");			// Comment\n"
        "		end\n"
        "		if(a) begin\n"
        "			a++;\n"
        "		end\n"
        "	end\n"
        "\n"
        "	initial $display(\"	\");		/*	*/ // some comment\n"
        "	initial $display(\"\");	//			Some comment\n"
        "endmodule\n"},
  };
  RunLintTestCases<VerilogAnalyzer, MixedIndentationRule>(kTestCases);
}

TEST(MixedIndentationRuleTest, RejectTests) {
  constexpr int kToken = TK_SPACE;
  const std::initializer_list<LintTestCase> kTestCases = {
      // 4 Spaces
      { "/*\n"
        "*  4 Spaces: RejectTests\n"
        "   *\t\t  */\n"
        "\n"
        "// Comment\n"
        "// Another comment   with multiple spaces         \n"
        "// Another comment\t\twith tabs	in it\t\t\n"
        "        // Comment only with leading spaces\n"
        "\n"
        "module test();\n"
        "    int a;\n"
        "\n"
        "    some_module #(\n"
        "            .DATA_W", {kToken, "\t\t"}, "(),\n",
        {kToken, "\t        "}, ".ADDR_W         ())\n"
        "        rom (        // Some comment\n"
        "            .addr_in        (),\n"
        "            .data_out       ());\n"
        "\n"
        "    always_comb /* some inline comment */ begin\n",
        {kToken, "       "}, "for(int i = 0; i < N; i++) begin /*\n"
        "            $display(\"     \");        // Should ignore quoted text..\n"
        "        \t$display(\"            \");*/\n",
        {kToken, "        \t"}, "$display(\"\");            // Comment\n",
        {kToken, "    \t  "}, "end\n"
        "        if(a) begin\n"
        "            a++;\n",
        {kToken, "\t       "}, "end\n"
        "    end\n"
        "\n"
        "    initial $display(\"	\");     /*	*/ // some comment\n"
        "    initial $display(\"\");   //			Some comment\n"
        "endmodule\n"},
      
      // 3 Spaces
      { "/*\n"
        "*  3 Spaces: RejectTest\n"
        "   *\t\t  */\n"
        "\n"
        "// Comment\n"
        "// Another comment   with multiple spaces       \n"
        "// Another comment\t\twith tabs	in it\t\t\n"
        "      // Comment only with leading spaces\n"
        "\n"
        "module test();\n"
        "   int a;\n"
        "\n"
        "   some_module #(\n"
        "         .DATA_W       (),\n"
        "         .ADDR_W       ())\n"
        "      rom (      // Some comment\n"
        "         .addr_in      (),\n"
        "         .data_out      ());\n"
        "\n"
        "   always_comb /* some inline comment */ begin\n"
        "      for(int i = 0; i < N; i++) begin /*\n"
        "         $display(\"    \");      // Should ignore quoted text..\n"
        "         $display(\"         \");*/\n"
        "         $display(\"\");         // Comment\n"
        "      end\n"
        "      if(a) begin\n"
        "         a++;\n"
        "      end\n"
        "   end\n"
        "\n"
        "   initial $display(\"	\");    /*	*/ // some comment\n"
        "   initial $display(\"\");   //			Some comment\n"
        "endmodule\n"},

      // Tabs
      { "/*\n"
        "*  Tabs: RejectTest\n"
        "	*\t\t  */\n"
        "\n"
        "// Comment\n"
        "// Another comment   with multiple spaces         \n"
        "// Another comment\t\twith tabs	in it\t\t\n"
        "\t\t\t// Comment only with leading tabs\n"
        "\n"
        "module test();\n"
        "	int a;\n"
        "\n"
        "	some_module #(\n"
        "			.DATA_W			(),\n"
        "			.ADDR_W", {kToken, " 			"}, "())\n"
        "		rom (", {kToken, " 			"}, "// Some comment\n"
        "			.addr_in		(),\n"
        "			.data_out		());\n"
        "\n"
        "	always_comb /* some inline comment */ begin\n",
        {kToken, "        "},"for(int i = 0; i < N; i++) begin /*\n"
        "			$display(\"      \");		// Should ignore quoted text..\n"
        "			$display(\"			\");*/\n"
        "			$display(\"       \");", {kToken, " 			"}, "// Comment\n",
        {kToken, "		 "}, "end\n",
        {kToken, "	 	"}, "if(a) begin\n",
        {kToken, " 			"}, "a++;\n"
        "		end\n"
        "	end\n"
        "\n"
        "	initial $display(\"	\");		/*	*/ // some comment\n"
        "	initial $display(\"\");	//     Some comment\n"
        "endmodule\n"},
  };
  RunLintTestCases<VerilogAnalyzer, MixedIndentationRule>(kTestCases);
}

}  // namespace
}  // namespace analysis
}  // namespace verilog
