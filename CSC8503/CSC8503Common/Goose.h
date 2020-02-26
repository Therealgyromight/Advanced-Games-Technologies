#pragma once
#include "GameObject.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "../GameTech/GameTechRenderer.h"
#include "../GameTech/TutorialGame.h"

#include <vector>

namespace NCL {
	namespace CSC8503 {
		class Goose : public GameObject
		{
		public:
			Goose(string name);
			~Goose();

			virtual void OnCollisionBegin(GameObject* otherObject);
			virtual void OnCollisionEnd(GameObject* otherObject);
			int appleCount = 0;
			int appleSaved = 0;
			int appleTotal = 5;
			int score = 0;
			bool canCollect = true;
			int sphereCollected = 0;
			int cubeCollected = 0;
			bool won = false;
			virtual void SetNeedSpawn(bool spawn) {
				needSpawn = spawn;
			}

			virtual bool IsInWater() const {
				return inWater;
			}
			virtual bool SpawnNeed() const {
				return needSpawn;
			}

			virtual int GetApple() const {
				return appleCount;
			}

			virtual int GetScore() {
				return score;
			}
		protected:
			bool inWater = false;
			bool needSpawn = false;
			
		};
	}
}

