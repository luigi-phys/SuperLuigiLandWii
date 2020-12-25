#include "worldnames.h"

dDvdLoader_c s_worldNamesLoader;
bool s_worldNamesLoaded = false;

dWorldNames_c dWorldNames_c::s_info;


bool LoadWorldNames() {
	if (s_worldNamesLoaded)
		return true;

	void *data = s_worldNamesLoader.load("/NewerRes/WorldNames.bin");
	if (data) {
		dWorldNames_c::s_info.load(data);
		s_worldNamesLoaded = true;
		return true;
	}

	return false;
}


void dWorldNames_c::load(void *buffer) {
	data = (header_s*)buffer;

	// decode all the level names
	for (int sect = 0; sect < sectionCount(); sect++) {
		// parse this section
		section_s *thisSect = getSectionByIndex(sect);

		for (int lev = 0; lev < thisSect->levelCount; lev++) {
			entry_s *level = &thisSect->levels[lev];

			char *name = (char*)getNameForLevel(level);

			for (int i = 0; i < level->nameLength+1; i++) {
				name[i] -= 0xD0;
			}
		}
	}
}

dWorldNames_c::entry_s *dWorldNames_c::searchBySlot(int world, int level) {
	for (int i = 0; i < sectionCount(); i++) {
		section_s *sect = getSectionByIndex(i);

		for (int j = 0; j < sect->levelCount; j++) {
			entry_s *entry = &sect->levels[j];
			if (entry->worldSlot == world && entry->levelSlot == level)
				return entry;
		}
	}

	return 0;
}

dWorldNames_c::entry_s *dWorldNames_c::searchByDisplayNum(int world, int level) {
	for (int i = 0; i < sectionCount(); i++) {
		section_s *sect = getSectionByIndex(i);

		for (int j = 0; j < sect->levelCount; j++) {
			entry_s *entry = &sect->levels[j];
			if (entry->displayWorld == world && entry->displayLevel == level)
				return entry;
		}
	}

	return 0;
}
