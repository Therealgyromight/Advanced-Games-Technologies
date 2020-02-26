#include "GameObject.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "../GameTech/GameTechRenderer.h"
#include "../GameTech/TutorialGame.h"

#include <vector>

namespace NCL {
	namespace CSC8503 {
		class Apple : public GameObject
		{
		public:
			Apple(string name);
			~Apple();

			virtual void OnCollisionBegin(GameObject* otherObject);
			virtual void OnCollisionEnd(GameObject* otherObject);

			bool collectable = true;
			string name1;
		};
	}
}

