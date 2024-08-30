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

#include "verilog/analysis/checkers/default_nettype_rule.h"

#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "common/analysis/lint_rule_status.h"
#include "common/analysis/token_stream_lint_rule.h"
#include "common/text/token_info.h"
#include "verilog/analysis/descriptions.h"
#include "verilog/analysis/lint_rule_registry.h"
#include "verilog/parser/verilog_token_enum.h"

namespace verilog {
namespace analysis {

using verible::AutoFix;
using verible::LintRuleStatus;
using verible::LintViolation;
using verible::TokenInfo;

// Register the lint rule
VERILOG_REGISTER_LINT_RULE(DefaultNetTypeRule);

static constexpr absl::string_view kMessage =
    "`default_nettype none should be present before the first module, class or "
    "interface.";

static constexpr absl::string_view kAutofixMessage =
    "Add `default_nettype none";

const LintRuleDescriptor &DefaultNetTypeRule::GetDescriptor() {
  static const LintRuleDescriptor d{
      .name = "default-nettype",
      .topic = "default-nettype",
      .desc =
          "Checks that '`default_nettype none' directive is present before the "
          "first module, class or interface.",
  };
  return d;
}

void DefaultNetTypeRule::HandleToken(const TokenInfo &token) {
  // Ignore all white space and comments
  switch (token.token_enum()) {
    case TK_NEWLINE:
    case TK_SPACE:
    case TK_COMMENT_BLOCK:
    case TK_EOL_COMMENT:
      return;
    default:
      break;
  }

  // Responds to a token by updating the state of the analysis.
  switch (state_) {
    case State::kComplete: {
      // Do nothing
      return;
    }
    case State::kStart: {
      // Record the first token to be used to insert an autofix
      first_token_ = &token;

      state_ = State::kSearch;
      // fall-through to default
    }
    case State::kSearch: {
      switch (token.token_enum()) {
        case DR_default_nettype: {
          state_ = State::kNetType;
          break;
        }
        case TK_module:
        case TK_class:
        case TK_interface: {
          if (!found_default_nettype_none_) {
            std::vector<AutoFix> autofixes{
                AutoFix(kAutofixMessage,
                        {*first_token_, absl::StrCat("`default_nettype none\n",
                                                     first_token_->text())})};

            violations_.insert(LintViolation(token, kMessage, autofixes));
          }
          state_ = State::kComplete;
          break;
        }
        default: {
          break;
        }
      }
      break;
    }
    case State::kNetType: {
      if (token.token_enum() == SymbolIdentifier) {
        if (token.text() == "none") {
          found_default_nettype_none_ = true;
        }
        state_ = State::kSearch;
      } else {
        state_ = State::kSearch;
      }
      break;
    }
    default: {
    }
  }  // switch (state_)
}

LintRuleStatus DefaultNetTypeRule::Report() const {
  return LintRuleStatus(violations_, GetDescriptor());
}

}  // namespace analysis
}  // namespace verilog
