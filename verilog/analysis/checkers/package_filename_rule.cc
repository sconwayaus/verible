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

#include "verilog/analysis/checkers/package_filename_rule.h"

#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "common/analysis/lint_rule_status.h"
#include "common/analysis/syntax_tree_search.h"
#include "common/text/config_utils.h"
#include "common/text/text_structure.h"
#include "common/text/token_info.h"
#include "common/util/file_util.h"
#include "verilog/CST/package.h"
#include "verilog/analysis/descriptions.h"
#include "verilog/analysis/lint_rule_registry.h"

namespace verilog {
namespace analysis {

VERILOG_REGISTER_LINT_RULE(PackageFilenameRule);

using verible::LintRuleStatus;
using verible::TextStructureView;

#define STYLE_DEFAULT_REGEX "[a-z_0-9]+"
static constexpr absl::string_view style_default_regex = STYLE_DEFAULT_REGEX;

PackageFilenameRule::PackageFilenameRule() {
  style_regex_ =
      std::make_unique<re2::RE2>(style_default_regex, re2::RE2::Quiet);

  optional_filename_suffix = "_pkg";

  kMessageFilename = absl::StrCat(
      "Package declaration name must match the file name "
      "(ignoring optional \"",
      optional_filename_suffix, "\" file name suffix).  ");

  kMessagePackageName =
      absl::StrCat("Package name does not match the naming convention ",
                   "defined by regex pattern: ", style_regex_->pattern());
}

const LintRuleDescriptor &PackageFilenameRule::GetDescriptor() {
  static const LintRuleDescriptor d{
      .name = "package-filename",
      .topic = "file-names",
      .desc =
          "Checks that the package name matches the filename. Depending on "
          "configuration, it is also allowed to replace underscore with dashes "
          "in filenames. The package name style is defined by a RE2 regular "
          "expression.\n"
          "Example common regex patterns:\n"
          "  lower_snake_case: \"[a-z_0-9]+\"\n"
          "  UPPER_SNAKE_CASE: \"[A-Z_0-9]+\"\n"
          "  Title_Snake_Case: \"[A-Z]+[a-z0-9]*(_[A-Z0-9]+[a-z0-9]*)*\"\n"
          "  Sentence_snake_case: \"([A-Z0-9]+[a-z0-9]*_?)([a-z0-9]*_*)*\"\n"
          "  camelCase: \"([a-z0-9]+[A-Z0-9]*)+\"\n"
          "  PascalCaseRegexPattern: \"([A-Z0-9]+[a-z0-9]*)+\"\n"
          "RE2 regular expression syntax documentation can be found at "
          "https://github.com/google/re2/wiki/syntax\n",
      .param =
          {
              {"package_name_style_regex", STYLE_DEFAULT_REGEX,
               "A regex used to check interface name style."},
              {"optional_filename_suffix", "_pkg",
               "An optional filename suffix"},
              {"allow-dash-for-underscore", "false",
               "Allow dashes in the filename corresponding to the "
               "underscores in the package"},
          },
  };
  return d;
}

void PackageFilenameRule::Lint(const TextStructureView &text_structure,
                               absl::string_view filename) {
  if (verible::file::IsStdin(filename)) {
    return;
  }

  const auto &tree = text_structure.SyntaxTree();
  if (tree == nullptr) return;

  // Find all package declarations.
  auto package_matches = FindAllPackageDeclarations(*tree);

  // See if names match the stem of the filename.
  //
  // Note:  package name | filename   | allowed ?
  //        -------------+------------+-----------
  //        foo          | foo.sv     | yes
  //        foo_bar      | foo_bar.sv | yes
  //        foo_bar      | foo-bar.sv | yes, if allow-dash-for-underscore
  //        foo          | foo_pkg.sv | yes
  //        foo          | foo-pkg.sv | yes, iff allow-dash-for-underscore
  //        foo_pkg      | foo_pkg.sv | yes
  //        foo_pkg      | foo.sv     | NO.
  const absl::string_view basename =
      verible::file::Basename(verible::file::Stem(filename));
  std::vector<absl::string_view> basename_components =
      absl::StrSplit(basename, '.');
  std::string unitname(basename_components[0].begin(),
                       basename_components[0].end());
  if (unitname.empty()) return;

  if (allow_dash_for_underscore_) {
    // If we allow for dashes, let's first convert them back to underscore.
    std::replace(unitname.begin(), unitname.end(), '-', '_');
  }

  // Report a violation on every package declaration, potentially.
  for (const auto &package_match : package_matches) {
    const verible::TokenInfo *package_name_token =
        GetPackageNameToken(*package_match.match);
    if (!package_name_token) continue;
    absl::string_view package_id = package_name_token->text();
    auto package_id_plus_suffix =
        absl::StrCat(package_id, optional_filename_suffix);
    if ((package_id != unitname) && (package_id_plus_suffix != unitname)) {
      violations_.insert(verible::LintViolation(
          *package_name_token,
          absl::StrCat(kMessageFilename, "declaration: \"", package_id,
                       "\" vs. basename(file): \"", unitname, "\"")));
    }

    // Check that the package name follows the naming style
    if (!RE2::FullMatch(unitname, *style_regex_)) {
      violations_.insert(
          verible::LintViolation(*package_name_token, kMessagePackageName));
    }
  }
}

absl::Status PackageFilenameRule::Configure(absl::string_view configuration) {
  using verible::config::SetBool;
  using verible::config::SetRegex;
  using verible::config::SetString;
  absl::Status s = verible::ParseNameValues(
      configuration,
      {{"package_name_style_regex", SetRegex(&style_regex_)},
       {"optional_filename_suffix", SetString(&optional_filename_suffix)},
       {"allow-dash-for-underscore", SetBool(&allow_dash_for_underscore_)}});
  if (!s.ok()) return s;

  kMessageFilename = absl::StrCat(
      "Package declaration name must match the file name "
      "(ignoring optional \"",
      optional_filename_suffix, "\" file name suffix).  ");

  kMessagePackageName =
      absl::StrCat("Package name does not match the naming convention ",
                   "defined by regex pattern: ", style_regex_->pattern());

  return absl::OkStatus();
}

LintRuleStatus PackageFilenameRule::Report() const {
  return LintRuleStatus(violations_, GetDescriptor());
}

}  // namespace analysis
}  // namespace verilog
