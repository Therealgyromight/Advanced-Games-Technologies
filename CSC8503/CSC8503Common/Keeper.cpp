#include "Keeper.h"

using namespace NCL;
using namespace CSC8503;

NCL::CSC8503::Keeper::Keeper(string name) {
	Keeper::SetName(name);
}

NCL::CSC8503::Keeper::~Keeper()
{
}

void NCL::CSC8503::Keeper::OnCollisionBegin(GameObject* otherObject) {
	if (otherObject->GetName() == "goose") {
		chasing = false;
	}
	if (otherObject->GetName() == "sphere") {
		
	}
}

void NCL::CSC8503::Keeper::OnCollisionEnd(GameObject* otherObject) {

}

vector<Vector3> nodes;
;
void NCL::CSC8503::Keeper::Move(Vector3 target) {
	if(chasing == true){
		NavigationGrid grid("TestGrid1.txt");
		Vector3 direction;
		NavigationPath outPath;
		Vector3 endPos;
		srand(time(NULL));
		Vector3 startPos = GetTransform().GetLocalPosition();
		if ((target.x > 50) && (target.z > 50)) {
			endPos = (Vector3(0, 0, 0));
		}
		else {
			endPos = target;
		}



		bool found = grid.FindPath(startPos, endPos, outPath);
		nodes.clear();
		Vector3 pos;
		while (outPath.PopWaypoint(pos)) {
			nodes.push_back(pos);
		}

		if (nodes.size() < 2) {
			direction = endPos - startPos;
		}
		else {
			direction = nodes[1] - nodes[0];
		}

		direction.Normalise();
		GetPhysicsObject()->AddForce(direction * 80);
	}
	else {
		NavigationGrid grid("TestGrid1.txt");
		Vector3 direction;
		NavigationPath outPath;
		Vector3 endPos;
		Vector3 startPos = GetTransform().GetLocalPosition();
		Vector3 goal = Vector3((rand() % 100) - 100, 0, (rand() % 100) - 100);
		if ((goal.x > 50) && (goal.z > 50)) {
			Move(target);

		}
		else {
			endPos = Vector3(0,0,0);
		}



		bool found = grid.FindPath(startPos, endPos, outPath);
		nodes.clear();
		Vector3 pos;
		while (outPath.PopWaypoint(pos)) {
			nodes.push_back(pos);
		}

		if (nodes.size() < 2) {
			direction = endPos - startPos;
		}
		else {
			direction = nodes[1] - nodes[0];
		}

		direction.Normalise();
		GetPhysicsObject()->AddForce(direction * 80);
	}
}