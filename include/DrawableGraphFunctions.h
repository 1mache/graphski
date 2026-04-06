#pragma once

#include "GraphFunctions.h"

namespace graphski
{
    nlohmann::json serializeDrawableNode(const INode& node, NodeId nodeId);
} // namespace graphski