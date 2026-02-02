// Copyright 2017-2021 The Verible Authors.
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

#include "verible/verilog/analysis/checkers/literal-digit-seperator-rule.h"

#include <initializer_list>

#include "absl/status/status.h"
#include "gtest/gtest.h"
#include "verible/common/analysis/linter_test_utils.h"
#include "verible/common/analysis/token-stream-linter-test-utils.h"
#include "verible/verilog/analysis/verilog-analyzer.h"
#include "verible/verilog/parser/verilog-token-enum.h"

namespace verilog {
namespace analysis {
namespace {

using verible::LintTestCase;
// using verible::RunApplyFixCases;
using verible::RunLintTestCases;
// using verible::RunConfiguredLintTestCases;

TEST(LiteralDigitSeperatorTest, DigitSeperatorPass) {
  const std::initializer_list<LintTestCase> kTestCases = {
      {""},
      // bin
      {"localparam int x = 'b0;"},
      {"localparam int x = 'b1;"},
      {"localparam int x = 'b11;"},
      {"localparam int x = 'b110;"},
      {"localparam int x = 'b1101;"},
      {"localparam int x = 'b1_1010;"},
      {"localparam int x = 'b11_0101;"},
      {"localparam int x = 'b110_1010;"},
      {"localparam int x = 'b1101_0101;"},
      {"localparam int x = 'b1_1010_1010;"},
      {"localparam int x = 'b11_0101_0101;"},
      {"localparam int x = 'b110_1010_1010;"},
      {"localparam int x = 'b1101_0101_0101;"},
      {"localparam int x = 'b1_1010_1010_1010;"},
      {"localparam int x = 'b11_0101_0101_0101;"},
      {"localparam int x = 'b110_1010_1010_1010;"},
      {"localparam int x = 'b1101_0101_0101_0101;"},
      {"localparam int x = 'b1_1010_1010_1010_1011;"},
      {"localparam int x = 'b11_0101_0101_0101_0111;"},
      {"localparam int x = 'b110_1010_1010_1010_1111;"},
      {"localparam int x = 'b1101_0101_0101_0101_1110;"},
      {"localparam int x = 'b1_1010_1010_1010_1011_1101;"},
      {"localparam int x = 'b11_0101_0101_0101_0111_1010;"},
      {"localparam int x = 'b110_1010_1010_1010_1111_0100;"},
      // oct
      {"localparam int x = 'o0;"},
      {"localparam int x = 'o65;"},
      {"localparam int x = 'o701;"},
      {"localparam int x = 'o7_012;"},
      {"localparam int x = 'o70_123;"},
      {"localparam int x = 'o701_234;"},
      {"localparam int x = 'o7_012_345;"},
      {"localparam int x = 'o70_123_456;"},
      {"localparam int x = 'o701_234_567;"},
      {"localparam int x = 'o7_012_345_676;"},
      {"localparam int x = 'o70_123_456_765;"},
      {"localparam int x = 'o701_234_567_654;"},
      {"localparam int x = 'o7_012_345_676_543;"},
      {"localparam int x = 'o70_123_456_765_432;"},
      {"localparam int x = 'o701_234_567_654_321;"},
      {"localparam int x = 'o7_012_345_676_543_210;"},
      // dec
      {"localparam int x = 'd0;"},
      {"localparam int x = 'd91;"},
      {"localparam int x = 'd912;"},
      {"localparam int x = 'd9123;"},
      {"localparam int x = 'd91_234;"},
      {"localparam int x = 'd912_345;"},
      {"localparam int x = 'd9_123_456;"},
      {"localparam int x = 'd91_234_567;"},
      {"localparam int x = 'd912_345_678;"},
      {"localparam int x = 'd9_123_456_789;"},
      {"localparam int x = 'd91_234_567_890;"},
      {"localparam int x = 'd912_345_678_901;"},
      {"localparam int x = 'd9_123_456_789_012;"},
      {"localparam int x = 'd91_234_567_890_123;"},
      {"localparam int x = 'd912_345_678_901_234;"},
      {"localparam int x = 0;"},
      {"localparam int x = 91;"},
      {"localparam int x = 912;"},
      {"localparam int x = 9123;"},
      {"localparam int x = 91_234;"},
      {"localparam int x = 912_345;"},
      {"localparam int x = 9_123_456;"},
      {"localparam int x = 91_234_567;"},
      {"localparam int x = 912_345_678;"},
      {"localparam int x = 9_123_456_789;"},
      {"localparam int x = 91_234_567_890;"},
      {"localparam int x = 912_345_678_901;"},
      {"localparam int x = 9_123_456_789_012;"},
      {"localparam int x = 91_234_567_890_123;"},
      {"localparam int x = 912_345_678_901_234;"},
      // hex
      {"localparam int x = 'h0;"},
      {"localparam int x = 'hf;"},
      {"localparam int x = 'hfe;"},
      {"localparam int x = 'hfed;"},
      {"localparam int x = 'hfedc;"},
      {"localparam int x = 'hf_edcb;"},
      {"localparam int x = 'hfe_dcba;"},
      {"localparam int x = 'hfed_cba9;"},
      {"localparam int x = 'hfedc_ba98;"},
      {"localparam int x = 'hf_edcb_a987;"},
      {"localparam int x = 'hfe_dcba_9876;"},
      {"localparam int x = 'hfed_cba9_8765;"},
      {"localparam int x = 'hfedc_ba98_7654;"},
      {"localparam int x = 'hf_edcb_a987_6543;"},
      {"localparam int x = 'hfe_dcba_9876_5432;"},
      {"localparam int x = 'hfed_cba9_8765_4321;"},
      {"localparam int x = 'hfedc_ba98_7654_3210;"},
  };

  RunLintTestCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(kTestCases);
}

TEST(LiteralDigitSeperatorTest, NoDigitSeperatorFail) {
  const std::initializer_list<LintTestCase> kTestCases = {
      {"localparam x = 32'h", {TK_HexDigits, "12345678"}, ";"},
      {"localparam x = 32'o", {TK_OctDigits, "01234567"}, ";"},
      {"localparam x = 8'd", {TK_DecDigits, "10000"}, ";"},
      {"localparam x = 8'b", {TK_BinDigits, "10101010"}, ";"},
      // bin
      // {"localparam int x = 'b", {TK_BinDigits, "0"}, ";"},
      // {"localparam int x = 'b", {TK_BinDigits, "1"}, ";"},
      // {"localparam int x = 'b", {TK_BinDigits, "11"}, ";"},
      // {"localparam int x = 'b", {TK_BinDigits, "110"}, ";"},
      // {"localparam int x = 'b", {TK_BinDigits, "1101"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "11010"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "110101"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "1101010"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "11010101"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "110101010"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "1101010101"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "11010101010"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "110101010101"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "1101010101010"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "11010101010101"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "110101010101010"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "1101010101010101"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "11010101010101011"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "110101010101010111"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "1101010101010101111"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "11010101010101011110"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "110101010101010111101"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "1101010101010101111010"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "11010101010101011110100"}, ";"},
      // oct
      // {"localparam int x = 'o", {TK_OctDigits, "0"}, ";"},
      // {"localparam int x = 'o", {TK_OctDigits, "701"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "7012"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "70123"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "701234"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "7012345"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "70123456"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "701234567"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "7012345676"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "70123456765"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "701234567654"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "7012345676543"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "70123456765432"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "701234567654321"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "7012345676543210"}, ";"},
      // dec
      // {"localparam int x = 'd", {TK_DecDigits, "0"}, ";"},
      // {"localparam int x = 'd", {TK_DecDigits, "91"}, ";"},
      // {"localparam int x = 'd", {TK_DecDigits, "912"}, ";"},
      // {"localparam int x = 'd", {TK_DecDigits, "9123"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "91234"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "912345"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "9123456"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "91234567"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "912345678"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "9123456789"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "91234567890"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "912345678901"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "9123456789012"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "91234567890123"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "912345678901234"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "91234"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "912345"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "9123456"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "91234567"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "912345678"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "9123456789"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "91234567890"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "912345678901"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "9123456789012"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "91234567890123"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "912345678901234"}, ";"},
      // hex
      // {"localparam int x = 'h", {TK_HexDigits, "0"}, ";"},
      // {"localparam int x = 'h", {TK_HexDigits, "f"}, ";"},
      // {"localparam int x = 'h", {TK_HexDigits, "fe"}, ";"},
      // {"localparam int x = 'h", {TK_HexDigits, "fed"}, ";"},
      // {"localparam int x = 'h", {TK_HexDigits, "fedc"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "fedcb"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "fedcba"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "fedcba9"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "fedcba98"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "fedcba987"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "fedcba9876"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "fedcba98765"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "fedcba987654"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "fedcba9876543"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "fedcba98765432"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "fedcba987654321"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "fedcba9876543210"}, ";"},
      
  };

  RunLintTestCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(kTestCases);
}

TEST(LiteralDigitSeperatorTest, IncorrectDigitSeperatorFail) {
  const std::initializer_list<LintTestCase> kTestCases = {
      {"localparam x = 32'h", {TK_HexDigits, "123_456_78"}, ";"},
      {"localparam x = 32'o", {TK_OctDigits, "012_3456_7"}, ";"},
      {"localparam x = 8'd", {TK_DecDigits, "1_2_8"}, ";"},
      {"localparam x = 8'd", {TK_DecDigits, "100_00"}, ";"},
      {"localparam x = 8'b", {TK_BinDigits, "101_01010"}, ";"},

      // bin
      {"localparam int x = 'b", {TK_BinDigits, "_0"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "0_"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "_1"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "1_1"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "_110"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "11_01"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "1101_0"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "1101_01"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "1101_010"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "110_1_0101"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "1101_0_1010"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "1_1010_10101"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "1_1010_101010"}, ";"},
      {"localparam int x = 'b", {TK_BinDigits, "1_1010_1010101"}, ";"},
      // oct
      {"localparam int x = 'o", {TK_OctDigits, "_0"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "0_"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "_71"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "_701"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "7012"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "7_012_3"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "7_012_34"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "7012_345"}, ";"},
      {"localparam int x = 'o", {TK_OctDigits, "7_012_3456"}, ";"},
      // dec
      {"localparam int x = 'd", {TK_DecDigits, "0_"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "9_1"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "9_12"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "9_1234"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "91_234_5"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "91_234_56"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "91_23_45_67"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "9_1_2_3_4_5_6_7_8"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "912_345_678_9"}, ";"},
      {"localparam int x = 'd", {TK_DecDigits, "912_345_678_90"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "0_"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "9_1"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "9_12"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "9_1234"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "91_234_5"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "91_234_56"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "91_23_45_67"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "9_1_2_3_4_5_6_7_8"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "912_345_678_9"}, ";"},
      {"localparam int x = ", {TK_DecNumber, "912_345_678_90"}, ";"},
      // hex
      {"localparam int x = 'h", {TK_HexDigits, "_0"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "f_"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "f_e"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "f_ed"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "f_edc"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "f_ed_cb"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "f_edcb_a"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "f_edcb_a9"}, ";"},
      {"localparam int x = 'h", {TK_HexDigits, "f_edcb_a98"}, ";"},
  };

  RunLintTestCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(kTestCases);
}

TEST(LiteralDigitSeperatorTest, DecNumberDigitSeperatorPass) {
  const std::initializer_list<LintTestCase> kTestCases = {
      {""},
      // bin
      {"localparam int x = '8b0;"},
      {"localparam int x = '16b0;"},
      {"localparam int x = '32b0;"},
      {"localparam int x = '64b0;"},
      {"localparam int x = '128b0;"},
      {"localparam int x = '999b0;"},
      {"localparam int x = '1000b1;"},
      {"localparam int x = '9999b11;"},
      {"localparam int x = '10_000b110;"},
      {"localparam int x = '100_000b1101;"},
      {"localparam int x = '1_000_000b1_1010;"},
      // oct
      {"localparam int x = '8o0;"},
      {"localparam int x = '16o65;"},
      {"localparam int x = '32o701;"},
      {"localparam int x = '64o7_012;"},
      {"localparam int x = '128o70_123;"},
      {"localparam int x = '999o701_234;"},
      {"localparam int x = '1000o7_012_345;"},
      {"localparam int x = '9999o70_123_456;"},
      {"localparam int x = '10_000o701_234_567;"},
      {"localparam int x = '100_000o7_012_345_676;"},
      {"localparam int x = '1_000_000o70_123_456_765;"},
      // dec
      {"localparam int x = '8d0;"},
      {"localparam int x = '16d91;"},
      {"localparam int x = '32d912;"},
      {"localparam int x = '64d9123;"},
      {"localparam int x = '128d91_234;"},
      {"localparam int x = '999d912_345;"},
      {"localparam int x = '1000d9_123_456;"},
      {"localparam int x = '9999d91_234_567;"},
      {"localparam int x = '10_000d912_345_678;"},
      {"localparam int x = '100_000d9_123_456_789;"},
      {"localparam int x = '1_000_000d91_234_567_890;"},
      // hex
      {"localparam int x = '8h0;"},
      {"localparam int x = '16hf;"},
      {"localparam int x = '32hfe;"},
      {"localparam int x = '64hfed;"},
      {"localparam int x = '128hfedc;"},
      {"localparam int x = '999hf_edcb;"},
      {"localparam int x = '1000hfe_dcba;"},
      {"localparam int x = '9999hfed_cba9;"},
      {"localparam int x = '10_000hfedc_ba98;"},
      {"localparam int x = '100_000hf_edcb_a987;"},
      {"localparam int x = '1_000_000hfe_dcba_9876;"},
  };

  RunLintTestCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(kTestCases);
}

TEST(LiteralDigitSeperatorTest, DecNumberNoDigitSeperatorFail) {
  const std::initializer_list<LintTestCase> kTestCases = {
      {"localparam x = ", {TK_DecNumber, "10000"}, "'h1234_5678;"},
      {"localparam x = ", {TK_DecNumber, "100000"}, "'o01_234_567;"},
      {"localparam x = ", {TK_DecNumber, "1000000"}, "'d10_000;"},
      {"localparam x = ", {TK_DecNumber, "10000000"}, "'b1010_1010;"},
      // bin
      {"localparam int x = ", {TK_DecNumber, "10000"}, "'b1_1010;"},
      {"localparam int x = ", {TK_DecNumber, "100000"}, "'b11_0101;"},
      {"localparam int x = ", {TK_DecNumber, "1000000"}, "'b110_1010;"},
      {"localparam int x = ", {TK_DecNumber, "10000000"}, "'b1101_0101;"},
      // oct
      {"localparam int x = ", {TK_DecNumber, "10000"}, "'o7_012;"},
      {"localparam int x = ", {TK_DecNumber, "100000"}, "'o70_123;"},
      {"localparam int x = ", {TK_DecNumber, "1000000"}, "'o701_234;"},
      {"localparam int x = ", {TK_DecNumber, "10000000"}, "'o7_012_345;"},
      // dec
      {"localparam int x = ", {TK_DecNumber, "10000"}, "'d91_234;"},
      {"localparam int x = ", {TK_DecNumber, "100000"}, "'d912_345;"},
      {"localparam int x = ", {TK_DecNumber, "1000000"}, "'d9_123_456;"},
      {"localparam int x = ", {TK_DecNumber, "10000000"}, "'d91_234_567;"},
      // hex
      {"localparam int x = ", {TK_DecNumber, "10000"}, "'hf_edcb;"},
      {"localparam int x = ", {TK_DecNumber, "100000"}, "'hfe_dcba;"},
      {"localparam int x = ", {TK_DecNumber, "1000000"}, "'hfed_cba9;"},
      {"localparam int x = ", {TK_DecNumber, "10000000"}, "'hfedc_ba98;"},
  };

  RunLintTestCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(kTestCases);
}

TEST(LiteralDigitSeperatorTest, DecNumberIncorrectDigitSeperatorFail) {
  const std::initializer_list<LintTestCase> kTestCases = {
      {"localparam x = ", {TK_DecNumber, "100_00"}, "'h1234_5678;"},
      {"localparam x = ", {TK_DecNumber, "1_000_00"}, "'o01_234_567;"},
      {"localparam x = ", {TK_DecNumber, "100_0000"}, "'d10_000;"},
      {"localparam x = ", {TK_DecNumber, "1_0000_000"}, "'b1010_1010;"},
      // bin
      {"localparam int x = ", {TK_DecNumber, "100_00"}, "'b1_1010;"},
      {"localparam int x = ", {TK_DecNumber, "1_000_00"}, "'b11_0101;"},
      {"localparam int x = ", {TK_DecNumber, "100_0000"}, "'b110_1010;"},
      {"localparam int x = ", {TK_DecNumber, "1_0000_000"}, "'b1101_0101;"},
      // oct
      {"localparam int x = ", {TK_DecNumber, "100_00"}, "'o7_012;"},
      {"localparam int x = ", {TK_DecNumber, "1_000_00"}, "'o70_123;"},
      {"localparam int x = ", {TK_DecNumber, "100_0000"}, "'o701_234;"},
      {"localparam int x = ", {TK_DecNumber, "1_0000_000"}, "'o7_012_345;"},
      // dec
      {"localparam int x = ", {TK_DecNumber, "100_00"}, "'d91_234;"},
      {"localparam int x = ", {TK_DecNumber, "1_000_00"}, "'d912_345;"},
      {"localparam int x = ", {TK_DecNumber, "100_0000"}, "'d9_123_456;"},
      {"localparam int x = ", {TK_DecNumber, "1_0000_000"}, "'d91_234_567;"},
      // hex
      {"localparam int x = ", {TK_DecNumber, "100_00"}, "'hf_edcb;"},
      {"localparam int x = ", {TK_DecNumber, "1_000_00"}, "'hfe_dcba;"},
      {"localparam int x = ", {TK_DecNumber, "100_0000"}, "'hfed_cba9;"},
      {"localparam int x = ", {TK_DecNumber, "1_0000_000"}, "'hfedc_ba98;"},
  };

  RunLintTestCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(kTestCases);
}


// TEST(LiteralDigitSeperatorTest, ConfigurationPass) {
//   LiteralDigitSeperatorRule rule;
//   absl::Status status;
//   EXPECT_TRUE((status = rule.Configure("")).ok()) << status.message();
//   EXPECT_TRUE((status = rule.Configure("bin:true;oct:true;hex:true")).ok())
//       << status.message();
// }

// TEST(LiteralDigitSeperatorTest, TooShortBinaryNumbers) {
//   constexpr int kToken = TK_BinDigits;
//   const std::initializer_list<LintTestCase> kTestCases = {
//       {""},
//       {"localparam x = 0;"},
//       {"localparam x = 1;"},
//       {"localparam x = '0;"},
//       {"localparam x = '1;"},
//       {"localparam x = 2'b0;"},                     // exception granted for 0
//       {"localparam x = 3'b", {kToken, "00"}, ";"},  // only 2 0s for 3 bits
//       {"localparam x = 32'b0;"},                    // exception granted for 0
//       {"localparam x = 2'b", {kToken, "1"}, ";"},
//       {"localparam x = 2'b?;"},  // exception granted for ?
//       {"localparam x = 2'b", {kToken, "x"}, ";"},
//       {"localparam x = 2'b", {kToken, "z"}, ";"},
//       {"localparam x = 2'b ", {kToken, "1"}, ";"},    // with space after base
//       {"localparam x = 2'b ", {kToken, "_1_"}, ";"},  // with underscores
//       {"localparam x = 1'b0;"},
//       {"localparam x = 1'b1;"},
//       {"localparam x = 32'd20;"},  // decimal numbers not treated
//       {"localparam x = 8'b 0001_1000;"},
//       {"localparam x = 8'b ", {kToken, "001_1000"}, ";"},
//       {"localparam x = 8'b ", {kToken, "0001_100"}, ";"},
//       {"localparam x = 8'b ", {kToken, "????_xx1"}, ";"},
//       {"localparam x = 8'b ", {kToken, "1??_xz10"}, ";"},
//       {"localparam x = 0 + 2'b", {kToken, "1"}, ";"},
//       {"localparam x = 3'b", {kToken, "10"}, " + 3'b", {kToken, "1"}, ";"},
//       {"localparam x = 2'b ", {kToken, "x"}, " & 2'b ", {kToken, "1"}, ";"},
//       {"localparam x = 5'b?????;"},
//       {"localparam x = 5'b?;"},                     // exception granted for ?
//       {"localparam x = 5'b", {kToken, "??"}, ";"},  // only 2 ?s for 5 bits
//   };

//   RunConfiguredLintTestCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(
//       kTestCases, "bin:true");
// }

// TEST(LiteralDigitSeperatorTest, BinaryNumbersConfiguredDontCare) {
//   const std::initializer_list<LintTestCase> kTestCases = {
//       {"localparam x = 0;"},
//       {"localparam x = 1;"},
//       {"localparam x = 3'b00;"},
//       {"localparam x = 32'b000;"},
//   };

//   RunConfiguredLintTestCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(
//       kTestCases, "bin:false");
// }

// TEST(LiteralDigitSeperatorTest, TooShortHexNumbers) {
//   constexpr int kToken = TK_HexDigits;
//   const std::initializer_list<LintTestCase> kTestCases = {
//       {"localparam x = 16'h0;"},  // Exception granted for single 0 and ?
//       {"localparam x = 16'h?;"},

//       {"localparam x = 16'h", {kToken, "00"}, ";"},
//       {"localparam x = 16'h", {kToken, "??"}, ";"},

//       {"localparam x = 1'h1;"},
//       {"localparam x = 4'hf;"},
//       {"localparam x = 5'h", {kToken, "f"}, ";"},
//       {"localparam x = 5'h1f;"},
//       {"localparam x = 16'h0001;"},
//       {"localparam x = 16'h00_01;"},
//       {"localparam x = 16'h", {kToken, "001"}, ";"},
//       {"localparam x = 16'h", {kToken, "0_01"}, ";"},
//       {"localparam x = 2'habcd;"},  // Note: truncated values are ok for this
//                                     // rule
//   };

//   RunConfiguredLintTestCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(
//       kTestCases, "hex:true");
// }

// TEST(LiteralDigitSeperatorTest, HexNumbersConfiguredDontCare) {
//   const std::initializer_list<LintTestCase> kTestCases = {
//       {"localparam x = 16'h0;"},
//       {"localparam x = 16'h?;"},
//       {"localparam x = 5'hf;"},
//       {"localparam x = 16'h001;"},
//   };

//   RunConfiguredLintTestCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(
//       kTestCases, "hex:false");
// }

// TEST(LiteralDigitSeperatorTest, TooShortOctalNumbers) {
//   constexpr int kToken = TK_OctDigits;
//   const std::initializer_list<LintTestCase> kTestCases = {
//       {"localparam x = 12'o0;"},  // Exception granted for 0 and ?
//       {"localparam x = 12'o?;"},

//       {"localparam x = 12'o", {kToken, "00"}, ";"},
//       {"localparam x = 12'o", {kToken, "??"}, ";"},

//       {"localparam x = 1'o1;"},
//       {"localparam x = 3'o7;"},
//       {"localparam x = 8'o777;"},  // Note: truncated values are ok for this
//                                    // rule
//       {"localparam x = 9'o777;"},
//       {"localparam x = 9'o7_7_7;"},
//       {"localparam x = 9'o", {kToken, "77"}, ";"},
//       {"localparam x = 9'o", {kToken, "7_7"}, ";"},
//       {"localparam x = 4'o17;"},
//   };

//   RunConfiguredLintTestCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(
//       kTestCases, "oct:true");
// }

// TEST(LiteralDigitSeperatorTest, OctalNumbersConfiguredDontCare) {
//   const std::initializer_list<LintTestCase> kTestCases = {
//       {"localparam x = 9'o77;"},
//       {"localparam x = 12'o77;"},
//   };

//   RunConfiguredLintTestCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(
//       kTestCases, "oct:false");
// }

// TEST(LiteralDigitSeperatorTest, DecimalNumbersNeverCare) {
//   const std::initializer_list<LintTestCase> kTestCases = {
//       {"localparam x = 32'd42;"},
//       {"localparam x = 32'd123456789;"},
//   };

//   RunConfiguredLintTestCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(
//       kTestCases, "");
// }

// TEST(LiteralDigitSeperatorTest, ApplyAutoFix) {
//   const std::initializer_list<verible::AutoFixInOut> kTestCases = {
//       {"localparam x = 32'hAB;", "localparam x = 32'h000000AB;"},
//       {"localparam x = 16'hAB;", "localparam x = 16'h00AB;"},
//       {"localparam x = 9'hAB;", "localparam x = 9'h0AB;"},
//       {"localparam x = 8'b101;", "localparam x = 8'b00000101;"},
//       {"localparam x = 9'o7;", "localparam x = 9'o007;"},
//       {"localparam x = 8'o7;", "localparam x = 8'o007;"},
//   };
//   RunApplyFixCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(
//       kTestCases, "bin:true;hex:true;oct:true;autofix:true");
// }

// TEST(LiteralDigitSeperatorRule, AutoFixDigitZeroProvideUnsizedAlternative) {
//   // In the case lint_zero is configured, we provide alternatives to write
//   // just a simple '0

//   // Alternatives the auto fix offers
//   constexpr int kFirstFix = 0;
//   constexpr int kSecondFix = 1;
//   constexpr int kThirdFix = 2;

//   const std::initializer_list<verible::AutoFixInOut> kTestCases = {
//       // First suggested alternative: replace just with simple '0
//       {"localparam x = 32'h0;", "localparam x = '0;", kFirstFix},
//       // We only apply this for unsigned values
//       {"localparam x = 32'sh0;", "localparam x = 32'sh00000000;", kFirstFix},

//       // Next alternative is the standard expansion
//       {"localparam x = 32'h0;", "localparam x = 32'h00000000;", kSecondFix},

//       // Third alternative would be what we anyway would do with single digit
//       // suggestions: convert to decimal.
//       {"localparam x = 32'h0;", "localparam x = 32'd0;", kThirdFix},
//   };
//   RunApplyFixCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(
//       kTestCases,
//       "bin:true;hex:true;oct:true;lint_zero:true;"
//       "autofix:true");
// }

// TEST(LiteralDigitSeperatorRule, AutoFixSingleDigitProvideDecimalAlternative) {
//   // Alternatives the auto fix offers
//   constexpr int kFirstFix = 0;
//   constexpr int kSecondFix = 1;
//   const std::initializer_list<verible::AutoFixInOut> kTestCases = {
//       // First choice: zero expand
//       {"localparam x = 32'h1;", "localparam x = 32'h00000001;", kFirstFix},
//       {"localparam x = 32'sh1;", "localparam x = 32'sh00000001;", kFirstFix},
//       {"localparam x = 32'h9;", "localparam x = 32'h00000009;", kFirstFix},
//       {"localparam x = 32'sh9;", "localparam x = 32'sh00000009;", kFirstFix},

//       // Second choice: convert to decimal
//       {"localparam x = 32'h1;", "localparam x = 32'd1;", kSecondFix},
//       {"localparam x = 32'sh1;", "localparam x = 32'sd1;", kSecondFix},
//       {"localparam x = 32'h9;", "localparam x = 32'd9;", kSecondFix},
//       {"localparam x = 32'sh9;", "localparam x = 32'sd9;", kSecondFix},
//   };
//   RunApplyFixCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(
//       kTestCases,
//       "bin:true;hex:true;oct:true;"
//       "autofix:true");
// }

// TEST(LiteralDigitSeperatorRule, AutoFixProvideInferredSize) {
//   // Alternatives the auto fix offers
//   constexpr int kFirstFix = 0;
//   constexpr int kSecondFix = 1;
//   const std::initializer_list<verible::AutoFixInOut> kTestCases = {
//       // First choice: zero expand
//       {"localparam x = 32'h10;", "localparam x = 32'h00000010;", kFirstFix},
//       {"localparam x = 3'b01;", "localparam x = 3'b001;", kFirstFix},
//       {"localparam x = 8'o77;", "localparam x = 8'o077;", kFirstFix},

//       // Second choice: Adjust size to inferred size
//       {"localparam x = 32'h10;", "localparam x = 8'h10;", kSecondFix},
//       {"localparam x = 3'b01;", "localparam x = 2'b01;", kSecondFix},
//       {"localparam x = 8'o77;", "localparam x = 6'o77;", kSecondFix},
//   };
//   RunApplyFixCases<VerilogAnalyzer, LiteralDigitSeperatorRule>(
//       kTestCases,
//       "bin:true;hex:true;oct:true;"
//       "autofix:true");
// }

}  // namespace
}  // namespace analysis
}  // namespace verilog
