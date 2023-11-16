#pragma once

namespace gfx {
	enum PipelineStage {
		VERTEX = 1,
		GEOMETRY = 2,
		PIXEL = 4,
		COMPUTE = 8,
		ALL = 0xFFFFFFFF
	};
}
