#pragma once

#include <vector>

namespace serialization {
	enum VariableType {
		CHAR,	
		INT,
		UINT,
		FLOAT,
		DOUBLE,

		CHAR_PTR,
		INT_PTR,
		UINT_PTR,
		FLOAT_PTR,
		DOURBLE_PTR
	};

	struct VariableElement {
		const char* m_name;
		VariableType m_type;
		size_t m_size;
		union {
			char* ptr;
			char data[8];
		} m_value;
	};

	struct ClassElement {
		std::vector<VariableElement> variables;
	};
}