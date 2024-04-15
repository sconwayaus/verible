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

#ifndef VERIBLE_VERILOG_ANALYSIS_CHECKERS_UNPACKED_DIMENSIONS_RULE_H_
#define VERIBLE_VERILOG_ANALYSIS_CHECKERS_UNPACKED_DIMENSIONS_RULE_H_

#include <set>

#include "common/analysis/lint_rule_status.h"
#include "common/analysis/syntax_tree_lint_rule.h"
#include "common/text/symbol.h"
#include "common/text/syntax_tree_context.h"
#include "verilog/analysis/descriptions.h"

namespace verilog {
namespace analysis {

// Unpacked DimensionsRule Checks that unpacked dimension ranges are declared in
// big-endian order, and when an unpacked dimension range is zero-based
// ([0:N-1]), the size is declared as [N] instead.
class UnpackedDimensionsRule : public verible::SyntaxTreeLintRule {
 public:
  using rule_type = verible::SyntaxTreeLintRule;

  static const LintRuleDescriptor &GetDescriptor();

  void HandleSymbol(const verible::Symbol &symbol,
                    const verible::SyntaxTreeContext &context) final;
  verible::LintRuleStatus Report() const final;
  absl::Status Configure(const absl::string_view configuration);

 private:
  std::set<verible::LintViolation> violations_;
  bool is_range_order_big_endian = true;
  bool allow_zero_based_range = false;
};

}  // namespace analysis
}  // namespace verilog

#endif  // VERIBLE_VERILOG_ANALYSIS_CHECKERS_UNPACKED_DIMENSIONS_RULE_H_
