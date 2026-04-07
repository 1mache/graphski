#pragma once

#include "core/GraphFunctions.h"

namespace graphski::graphics
{
    using ::graphski::core::INode;
    using ::graphski::core::NodeId;

    nlohmann::json serializeDrawableNode(const INode& node, NodeId nodeId);
} // namespace graphski::graphics