#pragma once

#include "Core.h"
#include "Types.h"
#include "FUUID.h"

class UEngineStatics
{
public:
	static FUUID GenUUID()
	{
		FUUID ResultUUID = NextUUID;
		NextUUID = FUUID::NewUUID();;
		return ResultUUID;
	}
	
	static FUUID GetNexUUID()
	{
		return NextUUID;
	}

	static void SetNextUUID(FUUID InUUID)
	{
		NextUUID = InUUID;
	}
private:
	inline static FUUID NextUUID;
};