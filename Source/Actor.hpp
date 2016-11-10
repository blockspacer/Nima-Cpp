#ifndef _NIMA_ACTOR_HPP_
#define _NIMA_ACTOR_HPP_

#include "ActorNode.hpp"
#include "ActorImage.hpp"
#include "BlockReader.hpp"
#include "Solver.hpp"

namespace nima
{
	class Actor
	{
		public:
			Actor();
			~Actor();

		private:
			int m_NodeCount;
			ActorNode** m_Nodes;
			ActorNode* m_Root;
			void readNodesBlock(BlockReader* block);
			void readAnimationsBlock(BlockReader* block);
		
		protected:
			int m_MaxTextureIndex;
			int m_ImageNodeCount;
			int m_SolverNodeCount;

			ActorImage** m_ImageNodes;
			Solver** m_Solvers;

		public:
			static Actor* fromBytes(unsigned char* bytes, unsigned int length);
			static Actor* fromFile(const char* filename);
	};
}
#endif