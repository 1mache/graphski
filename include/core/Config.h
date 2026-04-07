#pragma once
#include <filesystem>
#include <cassert>

namespace graphski::core
{
    class Config
    {
    public:
	    // File name for serialization of the graph
	    // TODO: make this dynamic
	    static constexpr const char* SERIALIZED_FILE_NAME = "graph.json";
    };
}