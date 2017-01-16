#include "PropertyAnimation.hpp"
#include "../BlockReader.hpp"
#include "KeyFrames/KeyFrame.hpp"
#include "KeyFrames/KeyFramePosX.hpp"
#include "KeyFrames/KeyFramePosY.hpp"
#include "KeyFrames/KeyFrameScaleX.hpp"
#include "KeyFrames/KeyFrameScaleY.hpp"
#include "KeyFrames/KeyFrameRotation.hpp"
#include "KeyFrames/KeyFrameOpacity.hpp"
#include "KeyFrames/KeyFrameDrawOrder.hpp"
#include "KeyFrames/KeyFrameLength.hpp"
#include "KeyFrames/KeyFrameVertexDeform.hpp"
#include "KeyFrames/KeyFrameIKStrength.hpp"
#include <cassert> 

using namespace nima;

PropertyAnimation::PropertyAnimation() :
	m_Type(PropertyType::Unknown),
	m_KeyFrames(nullptr),
	m_KeyFramesCount(0)
{

}

PropertyAnimation::~PropertyAnimation()
{
	for(int i = 0; i < m_KeyFramesCount; i++)
	{
		delete m_KeyFrames[i];
	}
	delete [] m_KeyFrames;
}

void PropertyAnimation::read(BlockReader* reader, ActorNode* node)
{
	BlockReader* block = reader->readNextBlock();
	if(block == nullptr)
	{
		return;
	}
	m_Type = block->blockType<PropertyType>();
	if(m_Type >= PropertyType::Max)
	{
		return;
	}
	m_KeyFramesCount = (int)block->readUnsignedShort();
	m_KeyFrames = new KeyFrame*[m_KeyFramesCount];
	KeyFrame* lastKeyFrame = nullptr;
	int currentIndex = 0;
	for(int i = 0; i < m_KeyFramesCount; i++)
	{
		KeyFrame* frame = nullptr;
		switch(m_Type)
		{
			case PropertyType::PosX:
				frame = new KeyFramePosX();
				break;
			case PropertyType::PosY:
				frame = new KeyFramePosY();
				break;
			case PropertyType::ScaleX:
				frame = new KeyFrameScaleX();
				break;
			case PropertyType::ScaleY:
				frame = new KeyFrameScaleY();
				break;
			case PropertyType::Rotation:
				frame = new KeyFrameRotation();
				break;
			case PropertyType::Opacity:
				frame = new KeyFrameOpacity();
				break;
			case PropertyType::DrawOrder:
				frame = new KeyFrameDrawOrder();
				break;
			case PropertyType::Length:
				frame = new KeyFrameLength();
				break;
			case PropertyType::VertexDeform:
				frame = new KeyFrameVertexDeform();
				break;
			case PropertyType::IKStrength:
				frame = new KeyFrameIKStrength();
				break;
			default:
				// This will only happen if the code isn't handling a property type it should handle.
				// Check the PropertyType enum and make sure Max is in the right place (and that you're not missing a case).
				assert(false);
				break;
		}

		if(frame->read(block, node))
		{
			m_KeyFrames[currentIndex++] = frame;
			if(lastKeyFrame != nullptr)
			{
				lastKeyFrame->setNext(frame);
			}
			lastKeyFrame = frame;
		}
		else
		{
			// If the frame fails to load, we just remove it from the list, not we change our list length (no we don't realloc the list length).
			delete frame;
			m_KeyFramesCount--;
		}
	}
}

void PropertyAnimation::apply(float time, ActorNode* node, float mix)
{
	if(m_KeyFramesCount == 0)
	{
		return;
	}

	int idx = 0;
	// Binary find the keyframe index.
	{
		int mid = 0;
		float element = 0.0f;
		int start = 0;
		int end = m_KeyFramesCount-1;

		while (start <= end) 
		{
	    	mid = ((start + end) >> 1);
			element = m_KeyFrames[mid]->time();
			if (element < time) 
			{
				start = mid + 1;
			} 
			else if (element > time) 
			{
				end = mid - 1;
			} 
			else 
			{
				start = mid;
				break;
			}
		}

		idx = start;
	}

	if(idx == 0)
	{
		m_KeyFrames[0]->apply(node, mix);
	}
	else
	{
		if(idx < m_KeyFramesCount)
		{
			KeyFrame* fromFrame = m_KeyFrames[idx-1];
			KeyFrame* toFrame = m_KeyFrames[idx];
			if(time == toFrame->time())
			{
				toFrame->apply(node, mix);
			}
			else
			{
				fromFrame->applyInterpolation(node, time, toFrame, mix);
			}
		}
		else
		{
			m_KeyFrames[idx-1]->apply(node, mix);
		}
	}
}