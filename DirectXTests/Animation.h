#pragma once

#include <vector>
#include "Defines.h"
#include "Node.h"
#include "Timer.h"

class Animation {
public:
	struct Keyframe {
		double timeStamp;
		DirectX::SimpleMath::Vector3 pos;
		DirectX::SimpleMath::Quaternion rot;
		DirectX::SimpleMath::Vector3 scale;
	};
	struct Channel {
		std::vector<Keyframe> keyframes;
		Node* node;
	};
public:
	Animation(unsigned int numKeyframes, unsigned int keyframesPerSecond, std::vector<Channel>&& channels);
	
	void Update();
private:
	float m_durationMilliseconds;
	unsigned int m_keyframesPerSecond;
	unsigned int m_numKeyframes;
	std::vector<Channel> m_channels;
	Timer m_timer;
};