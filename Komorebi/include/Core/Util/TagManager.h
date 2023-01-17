#pragma once

#include <vector>

class TagManager {
public:
	static TagManager* GetInstance() {
		if (!m_instance) {
			m_instance = new TagManager();
		}
		return m_instance;
	}

	uint32_t RegisterTag(const char* tag);
	uint32_t TagsToBitmask(std::vector<const char*> tags);
	uint32_t TagToBitmask(const char* tag);
private:
	TagManager() {};
	static TagManager* m_instance;
	std::vector<const char*> m_tags;
};