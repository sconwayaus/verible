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

#include "verilog/CST/port.h"

#include <vector>

#include "common/analysis/syntax_tree_search.h"
#include "common/text/concrete_syntax_leaf.h"
#include "common/text/concrete_syntax_tree.h"
#include "common/text/symbol.h"
#include "common/text/tree_utils.h"
#include "common/util/logging.h"
#include "verilog/CST/declaration.h"
#include "verilog/CST/identifier.h"
#include "verilog/CST/verilog_matchers.h"  // pragma IWYU: keep
#include "verilog/CST/verilog_nonterminals.h"

namespace verilog {

using verible::Symbol;
using verible::SyntaxTreeLeaf;

std::vector<verible::TreeSearchMatch> FindAllPortDeclarations(
    const Symbol &root) {
  return SearchSyntaxTree(root, NodekPortDeclaration());
}

std::vector<verible::TreeSearchMatch> FindAllActualNamedPort(
    const Symbol &root) {
  return SearchSyntaxTree(root, NodekActualNamedPort());
}

std::vector<verible::TreeSearchMatch> FindAllPortReferences(
    const verible::Symbol &root) {
  return SearchSyntaxTree(root, NodekPort());
}

std::vector<verible::TreeSearchMatch> FindAllTaskFunctionPortDeclarations(
    const Symbol &root) {
  return SearchSyntaxTree(root, NodekPortItem());
}

const SyntaxTreeLeaf *GetIdentifierFromPortDeclaration(const Symbol &symbol) {
  const auto *identifier_symbol =
      verible::GetSubtreeAsSymbol(symbol, NodeEnum::kPortDeclaration, 3);
  if (!identifier_symbol) return nullptr;
  return AutoUnwrapIdentifier(*identifier_symbol);
}

const SyntaxTreeLeaf *GetDirectionFromPortDeclaration(const Symbol &symbol) {
  if (const auto *dir_symbol =
          GetSubtreeAsSymbol(symbol, NodeEnum::kPortDeclaration, 0)) {
    return &SymbolCastToLeaf(*dir_symbol);
  }
  return nullptr;
}

const verible::SyntaxTreeNode *GetDataTypeNodeFromPortDeclaration(
    const Symbol &symbol) {
  if (const auto *dir_symbol =
          GetSubtreeAsNode(symbol, NodeEnum::kPortDeclaration, 2)) {
    return &SymbolCastToNode(*dir_symbol);
  }
  return nullptr;
}

const verible::SyntaxTreeNode *GetDataTypePrimitiveNodeFromDataType(
    const Symbol &symbol) {
  if (const auto *data_type_primitive_node =
          GetSubtreeAsNode(symbol, NodeEnum::kDataType, 1)) {
    return &SymbolCastToNode(*data_type_primitive_node);
  }
  return nullptr;
}

const SyntaxTreeLeaf *GetSignalTypeFromPortDeclaration(
  const Symbol &symbol) {
  if (const auto *data_type_primitive_node =
          GetSubtreeAsLeaf(symbol, NodeEnum::kPortDeclaration, 1)) {
    return &SymbolCastToLeaf(*data_type_primitive_node);
  }

  return nullptr;
}

const SyntaxTreeLeaf *GetDataTypePrimitiveFromPortDeclaration(
    const Symbol &symbol) {

  if (const verible::SyntaxTreeNode *data_type_node =
          GetDataTypeNodeFromPortDeclaration(symbol)) {
    if (const verible::SyntaxTreeNode *data_type_primitive_node =
            GetDataTypePrimitiveNodeFromDataType(*data_type_node)) {

      if(data_type_primitive_node->MatchesTag(NodeEnum::kLocalRoot)) {
        return nullptr;
      }
      
      if (const auto *dir_symbol = GetSubtreeAsSymbol(
              *data_type_primitive_node, NodeEnum::kDataTypePrimitive, 0)) {
        return &SymbolCastToLeaf(*dir_symbol);
      }
    }
  }
  return nullptr;
}

const Symbol *GetInterfaceHeaderNodeFromPortDeclaration(const Symbol &symbol) {
  if (const auto *data_type_node = GetSubtreeAsNode(symbol, NodeEnum::kPortDeclaration, 2, NodeEnum::kDataType)) {
    if(const auto *interface_port_header_node = GetSubtreeAsNode(*data_type_node, NodeEnum::kDataType, 1)) {
      if(NodeEnum(interface_port_header_node->Tag().tag) == NodeEnum::kInterfacePortHeader) {
        return &SymbolCastToNode(*interface_port_header_node);
      }
    }
  }
  return nullptr;
}

const SyntaxTreeLeaf *GetInterfaceModPortFromInterfaceHeaderNode(const Symbol &symbol) {
  if (const auto *modport_symbol =
          GetSubtreeAsSymbol(symbol, NodeEnum::kInterfacePortHeader, 2)) {
    return &SymbolCastToLeaf(*modport_symbol);
  }
  return nullptr;
}

std::vector<verible::TreeSearchMatch> FindAllModulePortDeclarations(
    const verible::Symbol &root) {
  return SearchSyntaxTree(root, NodekModulePortDeclaration());
}

const verible::SyntaxTreeLeaf *GetIdentifierFromModulePortDeclaration(
    const verible::Symbol &symbol) {
  static const char *const TOO_MANY_IDS_ERROR =
      "Expected one identifier node in module port declaration, but got ";
  auto &node = SymbolCastToNode(symbol);
  if (!MatchNodeEnumOrNull(node, NodeEnum::kModulePortDeclaration)) {
    return nullptr;
  }
  auto id_unpacked_dims = FindAllIdentifierUnpackedDimensions(symbol);
  if (id_unpacked_dims.empty()) {
    auto port_ids = verible::SearchSyntaxTree(symbol, NodekPortIdentifier());
    if (port_ids.size() > 1) {
      LOG(ERROR) << TOO_MANY_IDS_ERROR << port_ids.size();
    }
    if (port_ids.empty()) return nullptr;
    return GetIdentifier(*port_ids[0].match);
  }
  if (id_unpacked_dims.size() > 1) {
    LOG(ERROR) << TOO_MANY_IDS_ERROR << id_unpacked_dims.size();
  }
  return GetSymbolIdentifierFromIdentifierUnpackedDimensions(
      *id_unpacked_dims.front().match);
}

const verible::SyntaxTreeLeaf *GetDirectionFromModulePortDeclaration(
    const verible::Symbol &symbol) {
  if (const auto *dir_symbol =
          GetSubtreeAsSymbol(symbol, NodeEnum::kModulePortDeclaration, 0)) {
    return &SymbolCastToLeaf(*dir_symbol);
  }
  return nullptr;
}

const verible::SyntaxTreeLeaf *GetIdentifierFromPortReference(
    const verible::Symbol &port_reference) {
  const auto *identifier_symbol =
      verible::GetSubtreeAsSymbol(port_reference, NodeEnum::kPortReference, 0);
  if (!identifier_symbol) return nullptr;
  return AutoUnwrapIdentifier(*identifier_symbol);
}

const verible::SyntaxTreeNode *GetPortReferenceFromPort(
    const verible::Symbol &port) {
  return verible::GetSubtreeAsNode(port, NodeEnum::kPort, 0,
                                   NodeEnum::kPortReference);
}

static const verible::SyntaxTreeNode *
GetTypeIdDimensionsFromTaskFunctionPortItem(const Symbol &symbol) {
  return verible::GetSubtreeAsNode(
      symbol, NodeEnum::kPortItem, 1,
      NodeEnum::kDataTypeImplicitBasicIdDimensions);
}

const verible::SyntaxTreeNode *GetUnpackedDimensionsFromTaskFunctionPortItem(
    const verible::Symbol &port_item) {
  const auto &type_id_dimensions =
      GetTypeIdDimensionsFromTaskFunctionPortItem(port_item);
  if (!type_id_dimensions) return nullptr;
  return verible::GetSubtreeAsNode(*type_id_dimensions,
                                   NodeEnum::kDataTypeImplicitBasicIdDimensions,
                                   2, NodeEnum::kUnpackedDimensions);
}

const Symbol *GetTypeOfTaskFunctionPortItem(const verible::Symbol &symbol) {
  const auto &type_id_dimensions =
      GetTypeIdDimensionsFromTaskFunctionPortItem(symbol);
  if (!type_id_dimensions) return nullptr;
  return verible::GetSubtreeAsNode(*type_id_dimensions,
                                   NodeEnum::kDataTypeImplicitBasicIdDimensions,
                                   0, NodeEnum::kDataType);
}

const SyntaxTreeLeaf *GetIdentifierFromTaskFunctionPortItem(
    const verible::Symbol &symbol) {
  const auto *type_id_dimensions =
      GetTypeIdDimensionsFromTaskFunctionPortItem(symbol);
  if (!type_id_dimensions) return nullptr;
  if (type_id_dimensions->size() <= 1) return nullptr;
  const auto *port_item = (*type_id_dimensions)[1].get();
  return port_item ? AutoUnwrapIdentifier(*port_item) : nullptr;
}

const verible::SyntaxTreeLeaf *GetActualNamedPortName(
    const verible::Symbol &actual_named_port) {
  return verible::GetSubtreeAsLeaf(actual_named_port,
                                   NodeEnum::kActualNamedPort, 1);
}

const verible::Symbol *GetActualNamedPortParenGroup(
    const verible::Symbol &actual_named_port) {
  return verible::GetSubtreeAsSymbol(actual_named_port,
                                     NodeEnum::kActualNamedPort, 2);
}

}  // namespace verilog
