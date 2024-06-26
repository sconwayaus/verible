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

#ifndef VERIBLE_VERILOG_ANALYSIS_CHECKERS_PACKAGE_FILENAME_RULE_H_
#define VERIBLE_VERILOG_ANALYSIS_CHECKERS_PACKAGE_FILENAME_RULE_H_

#include <memory>
#include <set>
#include <string>

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "common/analysis/lint_rule_status.h"
#include "common/analysis/text_structure_lint_rule.h"
#include "common/text/text_structure.h"
#include "re2/re2.h"
#include "verilog/analysis/descriptions.h"

namespace verilog {
namespace analysis {

// PackageFilenameRule detects when the package name does not match filename
// (modulo optional _pkg suffix, which is removed before name comparison).
// InterfaceNameStyleRule checks that all interface names follow
// a naming convention matching a regex pattern.
class PackageFilenameRule : public verible::TextStructureLintRule {
 public:
  using rule_type = verible::TextStructureLintRule;

  PackageFilenameRule();

  static const LintRuleDescriptor &GetDescriptor();

  void Lint(const verible::TextStructureView &, absl::string_view) final;

  verible::LintRuleStatus Report() const final;

  absl::Status Configure(absl::string_view configuration) final;

 private:
  // Ok to treat dashes as underscores.
  bool allow_dash_for_underscore_ = false;

  // Collection of found violations.
  std::set<verible::LintViolation> violations_;

  // A regex to check the style against
  std::unique_ptr<re2::RE2> style_regex_;

  // An optional filename suffix
  std::string optional_filename_suffix;

  std::string kMessageFilename;
  std::string kMessagePackageName;
};

}  // namespace analysis
}  // namespace verilog

#endif  // VERIBLE_VERILOG_ANALYSIS_CHECKERS_PACKAGE_FILENAME_RULE_H_
