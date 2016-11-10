#ifndef _NIMA_ACTORANIMATION_HPP_
#define _NIMA_ACTORANIMATION_HPP_

#include <string>

namespace nima
{
	class BlockReader;
	class ActorNode;
	
	class ActorAnimation
	{
		private:
			std::string m_Name;
			int m_FPS;
			float m_Duration;
			bool m_IsLooping;
			//NodeAnimation[] m_AnimatedNodes;

		public:
			const std::string& name() const;
			float duration() const;

			static ActorAnimation* read(BlockReader* reader, ActorNode** nodes);

	};
}
#endif