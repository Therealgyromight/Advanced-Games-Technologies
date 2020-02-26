#include "Goose.h"


using namespace NCL;
using namespace CSC8503;

NCL::CSC8503::Goose::Goose(string name) {
	Goose::SetName(name);
}

NCL::CSC8503::Goose::~Goose()
{
}

void NCL::CSC8503::Goose::OnCollisionBegin(GameObject* otherObject) {
	if (otherObject->GetName() == "apple" && appleCount == 0 && canCollect == true) {
		appleCount++;
		canCollect = false;
		otherObject->GetTransform().SetWorldPosition(Vector3(1000, 0, 0));
	}
	if (otherObject->GetName() == "nest") {
		if (appleCount > 0) {
			appleSaved++;
			appleCount--;
			score++;
			canCollect = true;
			sphereCollected = 0;
			cubeCollected = 0;
		}
		
		
		if (appleSaved == appleTotal) {
			std::cout << "You win" << std::endl;
			won = true;
		}

		if (sphereCollected == 1) {
			sphereCollected = 0;
			canCollect = true;
			score += 10;
		}
		if (cubeCollected == 1) {
			cubeCollected = 0;
			canCollect = true;
			score += 5;
		}
	}


	if (otherObject->GetName() == "pond") {
		inWater = true;
	}
	if (otherObject->GetName() == "floor") {
		inWater = false;
	}
	if (otherObject->GetName() == "keeper") {
		score -= 2;
		if (appleCount > 0) {
			appleCount--;
			
			needSpawn = true;
			canCollect = true;
			GetPhysicsObject()->AddForce(Vector3(5000, 0, 5000));
		}
		else {
			GetPhysicsObject()->AddForce(Vector3(5000, 0, 5000));
			if(sphereCollected==1)
				sphereCollected = 2;
			
			if(cubeCollected == 1 )
				cubeCollected = 2;
			canCollect = true;
		}
		
	}

	if (otherObject->GetName() == "sphere" && canCollect == true) {
		sphereCollected = 1;
		canCollect = false;
		otherObject->GetTransform().SetWorldPosition(Vector3(1000, 0, 0));
	}
	if (otherObject->GetName() == "bonus" && canCollect == true) {
		cubeCollected = 1;
		canCollect = false;
		otherObject->GetTransform().SetWorldPosition(Vector3(1000, 0, 0));
	}

}

void NCL::CSC8503::Goose::OnCollisionEnd(GameObject* otherObject) {
	
}