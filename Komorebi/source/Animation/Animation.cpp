#include "Animation/Animation.h"
#include <d3d11.h>

Animation::Keyframe findKeyframe(const std::vector<Animation::Keyframe>& keyframes, double timeMillis) {
	for (int i = 0; i < keyframes.size(); i++) {
		const Animation::Keyframe& keyframe = keyframes[i];
		if (keyframe.timeStamp >= timeMillis) {
			if (i > 0) {
				const Animation::Keyframe& keyframePrev = keyframes[i - 1];
				float t = (timeMillis - keyframePrev.timeStamp) / (keyframe.timeStamp - keyframePrev.timeStamp);
				return {
					timeMillis,
					DirectX::SimpleMath::Vector3::Lerp(keyframePrev.pos, keyframe.pos, t),
					DirectX::SimpleMath::Quaternion::Lerp(keyframePrev.rot, keyframe.rot, t),
					DirectX::SimpleMath::Vector3::Lerp(keyframePrev.scale, keyframe.scale, t)
				};
			}
			return keyframe;
		}
	}
	return keyframes[keyframes.size()-1];
}

Animation::Animation(unsigned int numKeyframes, double keyframesPerSecond, std::vector<Channel>&& channels) :
	m_numKeyframes(numKeyframes),
	m_keyframesPerSecond(keyframesPerSecond)
{
	m_durationMilliseconds = (double)numKeyframes * 1000 / keyframesPerSecond;
	m_channels = channels;
}

void Animation::Update() {	
	float timeMillis = std::fmod(m_timer.Peek(), m_durationMilliseconds);
	for (Channel& channel : m_channels) {
		const Keyframe& keyframe = findKeyframe(channel.keyframes, timeMillis);
		channel.node->m_localTransform.SetPosition(DirectX::SimpleMath::Vector3(0,0,0));
		channel.node->m_localTransform.SetRotation(DirectX::SimpleMath::Quaternion::Identity);
		channel.node->m_localTransform.SetScale(DirectX::SimpleMath::Vector3(1,1,1));

		channel.node->m_localTransform.RotateQuaternion(keyframe.rot);
		channel.node->m_localTransform.Translate(keyframe.pos);
	}	
}