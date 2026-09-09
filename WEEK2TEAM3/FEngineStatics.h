#pragma once

#include "FUUID.h"

class FEngineStatics
{
public:
	static FUUID GenUUID();
	static FUUID GetNextUUID();
	static void SetNextUUID(FUUID InUUID);

private:
	static FUUID NextUUID;
};
