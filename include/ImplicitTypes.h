#pragma once
#include <vector>
#include <MetaType.h>


struct AndType : public MetaType
{
	std::vector<MetaType> joined_types;
	IMPL_VISITOR_TYPE
};

struct OrType : public MetaType
{
	std::vector<MetaType> possible_types;
	IMPL_VISITOR_TYPE
};




