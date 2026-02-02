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

#ifndef VERIBLE_VERILOG_ANALYSIS_CHECKERS_UNDERSIZED_BINARY_LITERAL_RULE_H_
#define VERIBLE_VERILOG_ANALYSIS_CHECKERS_UNDERSIZED_BINARY_LITERAL_RULE_H_

#include <set>
#include <string>

#include "absl/status/status.h"
#include "verible/common/analysis/lint-rule-status.h"
#include "verible/common/analysis/token-stream-lint-rule.h"
#include "verible/common/text/symbol.h"
#include "verible/common/text/syntax-tree-context.h"
#include "verible/verilog/analysis/descriptions.h"

namespace verilog {
namespace analysis {

// LiteralDigitSeperatorRule checks literals use an approprate digit seperator
class LiteralDigitSeperatorRule : public verible::TokenStreamLintRule {
 public:
  using rule_type = verible::TokenStreamLintRule;

  static const LintRuleDescriptor& GetDescriptor();

  void HandleToken(const verible::TokenInfo &token) final;

  verible::LintRuleStatus Report() const final;

  absl::Status Configure(std::string_view configuration) final;

 private:

  std::string* GetExpectedLiteral(std::string *literal, 
int digit_spacing, int min_digits_to_seperate, std::string *expected_literal);

  void HandleNumberDigitSeperation(const verible::TokenInfo &token, int digit_spacing, int min_digits_to_seperate, std::string_view base);
  // Generate string representation of why lint error occurred at leaf
  // static std::string FormatReason(std::string_view width,
  //                                 std::string_view base_text, char base,
  //                                 std::string_view literal);

  // bool check_bin_numbers_ = true;
  // bool check_hex_numbers_ = false;
  // bool check_oct_numbers_ = false;
  // bool lint_zero_ = false;
  // bool autofix_ = true;

  std::set<verible::LintViolation> violations_;
};

}  // namespace analysis
}  // namespace verilog

#endif  // VERIBLE_VERILOG_ANALYSIS_CHECKERS_UNDERSIZED_BINARY_LITERAL_RULE_H_
