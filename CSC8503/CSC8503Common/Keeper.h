#pragma once
#include "GameObject.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "../GameTech/GameTechRenderer.h"
#include "../GameTech/TutorialGame.h"
#include "NavigationGrid.h"
#include "NavigationPath.h"
#include "NavigationMap.h"
#include "NavigationMesh.h"
#include "stdlib.h"
#include <vector>

namespace NCL {
	namespace CSC8503 {
		class Keeper : public GameObject
		{
		public:
			Keeper(string name);
			~Keeper();

			void Move(Vector3 target);
			virtual void OnCollisionBegin(GameObject* otherObject);
			virtual void OnCollisionEnd(GameObject* otherObject);

			bool chasing = false;
			string name1;
			
		};
	}
}

