//#include <algorithm>    // std::find

#include "Core/Util/TagManager.h"

TagManager* TagManager::m_instance;

uint32_t TagManager::RegisterTag(const char* tag) {
	int idx = 0;
	for (const char* t : m_tags) {
		if (!strcmp(t, tag)) break;
		idx++;
	}
	if (idx == m_tags.size()) {
		m_tags.push_back(tag);
	}
	return uint64_t(1) << idx;
}
uint32_t TagManager::TagsToBitmask(std::vector<const char*> tags) {
	uint32_t mask = 0;
	for (const char* tag : tags) {
		int idx = 0;
		for (const char* t : m_tags) {
			if (!strcmp(t, tag)) {
				mask |= uint64_t(1) << idx;
				break;
			}
			idx++;
		}
	}
	return mask;
}
uint32_t TagManager::TagToBitmask(const char* tag) {
	int idx = 0;
	for (const char* t : m_tags) {
		if (!strcmp(t, tag)) return uint64_t(1) << idx;
		idx++;
	}
	return 0;	
}