#include "graphics/DrawableGraphFunctions.h"

#include "graphics/DrawableNode.h"

namespace graphski
{
    nlohmann::json serializeDrawableNode(const INode &node, NodeId nodeId)
    {
        auto iNodeJson = serializeINode(node, nodeId);

        const auto* drawableNode = dynamic_cast<const DrawableNode*>(&node);
        if (!drawableNode)
            throw std::invalid_argument("Expected a DrawableNode for serialization, but got a different INode type.");

        // append generic node json with drawable-specific data
        iNodeJson["position"] = { drawableNode->getPosition().x, drawableNode->getPosition().y };
        return iNodeJson;
    }
}