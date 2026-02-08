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

#ifndef VERIBLE_VERILOG_ANALYSIS_CHECKERS_DEFAULT_NETTYPE_H_
#define VERIBLE_VERILOG_ANALYSIS_CHECKERS_DEFAULT_NETTYPE_H_

#include <set>

#include "verible/common/analysis/lint-rule-status.h"
#include "verible/common/analysis/token-stream-lint-rule.h"
#include "verible/common/text/token-info.h"
#include "verible/verilog/analysis/descriptions.h"

namespace verilog {
namespace analysis {

// Checks that default_nettype is defined before the first module.
class DefaultNetTypeRule : public verible::TokenStreamLintRule {
 public:
  using rule_type = verible::TokenStreamLintRule;

  static const LintRuleDescriptor &GetDescriptor();

  DefaultNetTypeRule() = default;

  void HandleToken(const verible::TokenInfo &token) final;

  verible::LintRuleStatus Report() const final;

 private:
  // States of the internal token-based analysis.
  enum class State { kStart, kSearch, kSearchDefaultNettypeValue, kComplete };

  // Internal lexical analysis state.
  State state_ = State::kStart;

  const verible::TokenInfo *first_token_ = nullptr;
  const verible::TokenInfo *default_nettype_token_ = nullptr;
  bool found_default_nettype_ = false;

  // Collection of found violations.
  std::set<verible::LintViolation> violations_;
};

}  // namespace analysis
}  // namespace verilog

#endif  // VERIBLE_VERILOG_ANALYSIS_CHECKERS_DEFAULT_NETTYPE_H_
