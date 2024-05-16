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

#ifndef VERIBLE_VERILOG_ANALYSIS_CHECKERS_PORT_NAME_SUFFIX_RULE_H_
#define VERIBLE_VERILOG_ANALYSIS_CHECKERS_PORT_NAME_SUFFIX_RULE_H_

#include <set>
#include <map>

#include "absl/strings/string_view.h"
#include "common/analysis/lint_rule_status.h"
#include "common/analysis/syntax_tree_lint_rule.h"
#include "common/text/symbol.h"
#include "common/text/syntax_tree_context.h"
#include "common/text/token_info.h"
#include "verilog/analysis/descriptions.h"

namespace verilog {
namespace analysis {

// PortNameSuffixRule check if each port name follows the correct naming
// convention.
// The port names should end with _i for inputs, _o for outputs and _io
// for inouts.
class PortNameSuffixRule : public verible::SyntaxTreeLintRule {
 public:
  using rule_type = verible::SyntaxTreeLintRule;

  PortNameSuffixRule();

  static const LintRuleDescriptor &GetDescriptor();

  absl::Status Configure(absl::string_view configuration) final;

  void HandleSymbol(const verible::Symbol &symbol,
                    const verible::SyntaxTreeContext &context) final;

  verible::LintRuleStatus Report() const final;

 private:
  
  std::set<absl::string_view> input_suffixes;
  std::set<absl::string_view> output_suffixes;
  std::set<absl::string_view> inout_suffixes;
  std::string interface_suffix_style;
  std::map<absl::string_view, std::set<absl::string_view>> suffixes;
  std::string kMessageIn;
  std::string kMessageOut;
  std::string kMessageInOut;


  // Helper functions
  bool IsSuffixOk(const absl::string_view &name, const std::set<absl::string_view> &suffix_list);
  void Violation(absl::string_view direction, const verible::TokenInfo &token,
                 const verible::SyntaxTreeContext &context);
  void ParseViloationString(const absl::string_view direction, 
    std::set<absl::string_view> suffix_list, std::string *msg) const;

  // Violations
  std::set<verible::LintViolation> violations_;
};

}  // namespace analysis
}  // namespace verilog

#endif  // VERIBLE_VERILOG_ANALYSIS_CHECKERS_PORT_NAME_SUFFIX_RULE_H_
