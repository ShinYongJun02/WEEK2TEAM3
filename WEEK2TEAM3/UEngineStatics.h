#pragma once

#include "Core.h"
#include "Types.h"
#include "FUUID.h"

class UEngineStatics
{
public:
	static FUUID GetUUID()
	{
		return FUUID::NewUUID();
	}

private:
	inline static uint32 NextUUID = 0;
};