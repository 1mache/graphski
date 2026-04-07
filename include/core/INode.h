#pragma once

#include "core/GraphTypes.h"

namespace graphski
{
class INode
{
public:
    virtual ~INode() = default;
    

    virtual Degree getDegIn() const = 0;
    virtual Degree getDegOut() const = 0;

    virtual void setDegIn(Degree val) = 0;
    virtual void setDegOut(Degree val) = 0;

    virtual const std::string& getName() const = 0;
    virtual void setName(std::string_view name) = 0;
};
}