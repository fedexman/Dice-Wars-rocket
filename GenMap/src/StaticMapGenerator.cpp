#include "interface_lib.h"
#include "MapLoader.h"
#include "DefaultMap.h"

API_EXPORT void InitMap(SInfo* info)
{
	info->SetName("Default map");
	info->AddMember("Fabien Picarougne");
}

API_EXPORT IMap* MapFactory()
{
	return(new MapLoader());
}

API_EXPORT void MapDelete(IMap* obj)
{
	delete(obj);
}
