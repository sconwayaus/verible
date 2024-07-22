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

#ifndef VERIBLE_VERILOG_ANALYSIS_CHECKERS_MIXED_INDENTATION_RULE_H_
#define VERIBLE_VERILOG_ANALYSIS_CHECKERS_MIXED_INDENTATION_RULE_H_

#include <set>

#include "absl/strings/string_view.h"
#include "common/analysis/lint_rule_status.h"
#include "common/analysis/text_structure_lint_rule.h"
#include "common/text/text_structure.h"
#include "verilog/analysis/descriptions.h"

namespace verilog {
namespace analysis {

// MixedIndentationRule detects when mixed indentation is used
class MixedIndentationRule : public verible::TextStructureLintRule {
 public:
  using rule_type = verible::TextStructureLintRule;

  static const LintRuleDescriptor &GetDescriptor();

  MixedIndentationRule() = default;

  // void HandleLineSection(absl::string_view line,
  //                        absl::string_view::size_type start_pos,
  //                        absl::string_view::size_type end_pos);

  void Lint(const verible::TextStructureView &text_structure,
            absl::string_view filename) final;

  bool isIndentPure(absl::string_view whitespace, const int indent_is_spaces,
                    const int num_indent_spaces);

  bool checkLeadingSpacingIndent(absl::string_view whitespace,
                                 const int num_indent_spaces);

  void CheckIndentation(const verible::TextStructureView &text_structure,
                        absl::string_view segment, const int indent_is_spaces,
                        const int num_indent_spaces);

  void ParseIndentation(const verible::TextStructureView &text_structure,
                        const int indent_is_spaces, const int num_spaces);

  verible::LintRuleStatus Report() const final;

 private:
  // Collection of found violations.
  std::set<verible::LintViolation> violations_;
};

}  // namespace analysis
}  // namespace verilog

#endif  // VERIBLE_VERILOG_ANALYSIS_CHECKERS_MIXED_INDENTATION_RULE_H_
