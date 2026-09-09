#include "FEngineStatics.h"

FUUID FEngineStatics::NextUUID;

FUUID FEngineStatics::GenUUID()
{
	FUUID ResultUUID = NextUUID;
	NextUUID = FUUID::NewUUID();
	return ResultUUID;
}

FUUID FEngineStatics::GetNexUUID()
{
	return NextUUID;
}

void FEngineStatics::SetNextUUID(FUUID InUUID)
{
	NextUUID = InUUID;
}
