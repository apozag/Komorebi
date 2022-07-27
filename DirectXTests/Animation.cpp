#include "Animation.h"

const Animation::Keyframe& findKeyframe(const std::vector<Animation::Keyframe>& keyframes, double timeMillis) {
	for (const Animation::Keyframe& keyframe : keyframes) {
		if (keyframe.timeStamp >= timeMillis) {
			return keyframe;
		}
	}
	return keyframes[keyframes.size()-1];
}

Animation::Animation(unsigned int numKeyframes, unsigned int keyframesPerSecond, std::vector<Channel>&& channels) :
	m_numKeyframes(numKeyframes),
	m_keyframesPerSecond(keyframesPerSecond)
{
	m_durationMilliseconds = (double)numKeyframes * 1000 / keyframesPerSecond;
	m_channels = channels;
}

void Animation::Update() {
	float timeMillis = std::fmod(m_timer.Peek(), m_durationMilliseconds)*0.01f;	
	for (Channel channel : m_channels) {
		// TODO: lerp keyframes		
		const Keyframe& keyframe = findKeyframe(channel.keyframes, timeMillis);
		channel.node->localTransform.SetPosition(keyframe.pos);
		channel.node->localTransform.SetRotation(keyframe.rot);
		//channel.node->localTransform.SetScale(keyframe.scale);
	}
}