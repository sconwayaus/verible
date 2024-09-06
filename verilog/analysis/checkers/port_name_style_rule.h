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

#ifndef VERIBLE_VERILOG_ANALYSIS_CHECKERS_PORT_NAME_STYLE_RULE_H_
#define VERIBLE_VERILOG_ANALYSIS_CHECKERS_PORT_NAME_STYLE_RULE_H_

#include <map>
#include <set>

#include "absl/strings/string_view.h"
#include "common/analysis/lint_rule_status.h"
#include "common/analysis/syntax_tree_lint_rule.h"
#include "common/text/symbol.h"
#include "common/text/syntax_tree_context.h"
#include "common/text/token_info.h"
#include "re2/re2.h"
#include "verilog/analysis/descriptions.h"

namespace verilog {
namespace analysis {

// PortNameStyleRule check if each port name follows the correct naming
// convention matching a regex pattern.
class PortNameStyleRule : public verible::SyntaxTreeLintRule {
 public:
  using rule_type = verible::SyntaxTreeLintRule;

  PortNameStyleRule();

  static const LintRuleDescriptor &GetDescriptor();

  void HandleSymbol(const verible::Symbol &symbol,
                    const verible::SyntaxTreeContext &context) final;

  verible::LintRuleStatus Report() const final;

  absl::Status Configure(absl::string_view configuration) final;

 private:
  std::set<verible::LintViolation> violations_;

  // A regex to check the style against
  std::unique_ptr<re2::RE2> input_style_regex_;
  std::unique_ptr<re2::RE2> output_style_regex_;
  std::unique_ptr<re2::RE2> inout_style_regex_;
  std::unique_ptr<re2::RE2> interface_style_regex_;

  bool enable_interface_modport_suffix;

  // Helper functions
  std::string CreateViolationMessage(absl::string_view direction,
                                     const re2::RE2 &regex);
  bool IsSuffixOk(const absl::string_view &name,
                  const std::set<absl::string_view> &suffix_list);
  void Violation(absl::string_view direction, const verible::TokenInfo &token,
                 const verible::SyntaxTreeContext &context);
};

}  // namespace analysis
}  // namespace verilog

#endif  // VERIBLE_VERILOG_ANALYSIS_CHECKERS_PORT_NAME_STYLE_RULE_H_