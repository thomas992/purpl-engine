// game.ini parsing

#include "gameinfo.h"

gameinfo_t *gameinfo_parse(const char *path)
{
	gameinfo_t *info;
	char *path2;

	if (!path || !strlen(path))
		return NULL;

	path2 = util_absolute_path(path);

}
