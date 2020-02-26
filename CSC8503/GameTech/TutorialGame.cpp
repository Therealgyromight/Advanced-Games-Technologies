#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

#include "../CSC8503Common/PositionConstraint.h"
#include "../CSC8503Common/Goose.h"
#include "../CSC8503Common/Apple.h"
#include "../CSC8503Common/Keeper.h"

#include <iostream>
#include <fstream>
#include <string>


using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= true;
	inSelectionMode = false;
	physics->UseGravity(useGravity);
	Debug::SetRenderer(renderer);

	InitialiseAssets();
}


bool TutorialGame::fileExists(const string& file) {
	struct stat buf;
	return (stat(file.c_str(), &buf) == 0);
}
/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh"		 , &cubeMesh);
	loadFunc("sphere.msh"	 , &sphereMesh);
	loadFunc("goose.msh"	 , &gooseMesh);
	loadFunc("CharacterA.msh", &keeperMesh);
	loadFunc("CharacterM.msh", &charA);
	loadFunc("CharacterF.msh", &charB);
	loadFunc("Apple.msh"	 , &appleMesh);

	basicTex	= (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	startTex = (OGLTexture*)TextureLoader::LoadAPITexture("start.png");
	exitTex = (OGLTexture*)TextureLoader::LoadAPITexture("exit.png");
	highTex = (OGLTexture*)TextureLoader::LoadAPITexture("highscore.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete gooseMesh;
	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void TutorialGame::UpdateGame(float dt) {
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}
	if (lockedObject != nullptr) {
		LockedCameraMovement();
	}

	UpdateKeys();
	
	

	if (useGravity) {
		Debug::Print("(G)ravity on", Vector2(10, 40));
	}
	else {
		Debug::Print("(G)ravity off", Vector2(10, 40));
	}
	if (menu == false && duck->won == false) {
		time -= dt;
		renderer->DrawString(std::to_string(time), Vector2(1168, 600));
	}
	
	if (time == 0 && duck->won == false) {
		ShowHighScore();
		renderer->DrawString("YOU LOSE, PLEASE TRY AGAIN", Vector2(808, 600));
	}
	SelectObject();
	MoveSelectedObject();

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);
	if (menu == false) {
		SetCameraPos();
	}
	if (duck->canCollect == false || duck->cubeCollected == 1 || duck->sphereCollected == 1) {
		guard1->chasing = true;
	}

	if (duck->cubeCollected == 2) {
		duck->cubeCollected = 0;
		GameObject* newcube = AddCubeToWorld(Vector3(guard1->GetTransform().GetLocalPosition()), Vector3(1,1,1));
		newcube->SetName("bonus");
		
	}
	if (duck->sphereCollected == 2) {
		duck->sphereCollected = 0;
		AddSphereToWorld(Vector3(guard1->GetTransform().GetLocalPosition()),1);
		
	}
	if (duck->canCollect)
		guard1->chasing = false;

	guard1->Move(duck->GetTransform().GetLocalPosition());
	Debug::FlushRenderables();
	renderer->Render();
	std::string s = std::to_string(duck->GetScore());

	renderer->DrawString("Score = " + s, Vector2(10, 100));

	if (duck->SpawnNeed() == true) {
		AddAppleToWorld(Vector3(guard1->GetTransform().GetLocalPosition().x , guard1->GetTransform().GetLocalPosition().y, guard1->GetTransform().GetLocalPosition().z ));
		duck->SetNeedSpawn(false);
	}
	if (duck->won == true) {
		renderer->DrawString("You Won, Congratulations!!!", Vector2(350, 600));
		duck->score += (time / 2);
		time = 0;
		score = duck->score;
		
		if (score > score1) {
			score3 = score2;
			score2 = score1;
			score1 = score;
			std::ofstream file1("score1.txt");
			file1 << score1;
			std::ofstream file2("score2.txt");
			file2 << score2;
			std::ofstream file3("score3.txt");
			file3 << score3;
			file1.close();
			file2.close();
			file3.close();
		}
		if (score < score1 && score > score2) {
			score3 = score2;
			score2 = score;
			std::ofstream file1("score1.txt");
			file1 << score1;
			std::ofstream file2("score2.txt");
			file2 << score2;
			std::ofstream file3("score3.txt");
			file3 << score3;
			file1.close();
			file2.close();
			file3.close();
		}
		if (score < score2 && score > score3) {
			score3 = score;
			std::ofstream file1("score1.txt");
			file1 << score1;
			std::ofstream file2("score2.txt");
			file2 << score2;
			std::ofstream file3("score3.txt");
			file3 << score3;
			file1.close();
			file2.close();
			file3.close();
		}
		ShowHighScore();
	}
}

void TutorialGame::ShowHighScore() {
	renderer->DrawString("HIGHSCORES", Vector2(700, 400));
	renderer->DrawString("Number 1: Rich " + std::to_string(score1), Vector2(700, 360));
	renderer->DrawString("Number 2: Michael " + std::to_string(score2), Vector2(700, 320));
	renderer->DrawString("Number 3: Alex " + std::to_string(score3), Vector2(700, 280));
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = duck;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		duck->GetPhysicsObject()->AddForce(-rightAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		duck->GetPhysicsObject()->AddForce(rightAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		duck->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		duck->GetPhysicsObject()->AddForce(-fwdAxis);
	}
}

void  TutorialGame::LockedCameraMovement() {
	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetWorldPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}
}


void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	Vector3 rotation = duck->GetTransform().GetLocalOrientation().ToEuler();
	int speed = 30;
	if (duck->IsInWater() == true) {
		speed = 15;
	}
	else { speed = 30; }

	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();
	Vector3 rightAxis = Vector3(camWorld.GetColumn(0));
	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
			duck->GetPhysicsObject()->AddTorque(Vector3(0, 5, 0));
			
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
			duck->GetPhysicsObject()->AddTorque(Vector3(0, -5, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			duck->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			duck->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
			duck->GetPhysicsObject()->AddForce(fwdAxis * speed);
			
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
			duck->GetPhysicsObject()->AddForce(-fwdAxis * speed);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			duck->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	
}

void TutorialGame::SetCameraPos() {
	Quaternion angle = (duck->GetTransform().GetWorldOrientation());
	Vector3 faxis = Vector3(0, 0, 1);
	faxis = angle * faxis;



	world->GetMainCamera()->SetPosition(duck->GetTransform().GetWorldPosition() - (faxis * 20));
	world->GetMainCamera()->SetPosition(
		Vector3(
			world->GetMainCamera()->GetPosition().x,
			world->GetMainCamera()->GetPosition().y + 5,
			world->GetMainCamera()->GetPosition().z
		)
	);



	Vector3 newCameraPos = (Matrix4::Rotation(0, Vector3(0, 1, 0)) * (world->GetMainCamera()->GetPosition() - duck->GetTransform().GetWorldPosition()) + duck->GetTransform().GetWorldPosition());



	Matrix4 temp = Matrix4::BuildViewMatrix(newCameraPos, duck->GetTransform().GetWorldPosition(), Vector3(0.0f, 1.0f, 0.5f));
	Matrix4 modelMat = temp.Inverse();



	Quaternion q(modelMat);
	Vector3 angles = q.ToEuler(); //nearly there now!



	world->GetMainCamera()->SetPitch(angles.x);
	world->GetMainCamera()->SetYaw(angles.y);
}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around.

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		renderer->DrawString("Press Q to change to camera mode!", Vector2(10, 0));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				if (selectionObject->GetName() != "floor" && selectionObject->GetName() != "" && selectionObject->GetName() != "start" && selectionObject->GetName() != "highscore" && selectionObject->GetName() != "quit") {
					std::string a = std::to_string(selectionObject->GetTransform().GetLocalPosition().x);
					std::string x = std::to_string(selectionObject->GetTransform().GetLocalPosition().y);
					std::string p = std::to_string(selectionObject->GetTransform().GetLocalPosition().z);
					renderer->DrawString(selectionObject->GetName() + " " + a + x + p, Vector2(0, 600));

				}
				if (selectionObject->GetName() == "quit") {
					exit(0);
				}
				if (selectionObject->GetName() == "highscore") {
					ShowHighScore();
				}
				if (selectionObject->GetName() == "start") {
					menu = false;
					world->GetMainCamera()->menu = false;
					world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
					inSelectionMode = !inSelectionMode;
				}
				
				return true;
				
			}
			else {
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
				}
			}
		}
	}
	else {
		renderer->DrawString("Press Q to change to select mode!", Vector2(10, 0));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	renderer -> DrawString("Click Force:" + std::to_string(forceMagnitude),
		Vector2(10, 20)); // Draw debug text at 10 ,20
	forceMagnitude += Window::GetMouse() -> GetWheelMovement() * 100.0f;
	
		if (!selectionObject) {
		return;// we haven ’t selected anything !
		
	}
	// Push the selected object !
		if (Window::GetMouse() -> ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(
			* world -> GetMainCamera());
		
			RayCollision closestCollision;
		if (world -> Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject -> GetPhysicsObject() -> AddForceAtPosition(
					ray.GetDirection() * forceMagnitude,
					closestCollision.collidedAt);
				
			}
			
		}
		
	}

}

void TutorialGame::InitCamera() {

	//do {
	//} while (!fileExists(fName));

	if (fileExists("score1.txt")) {
		std::ifstream infile("score1.txt");
		string sLine;
		std::getline(infile, sLine);
		int sVal = std::stoi(sLine);
		score1 = sVal;
		infile.close();
	}
	if (fileExists("score2.txt")) {
		std::ifstream infile("score2.txt");
		string sLine;
		std::getline(infile, sLine);
		int sVal = std::stoi(sLine);
		score2 = sVal;
		infile.close();
	}
	if (fileExists("score3.txt")) {
		std::ifstream infile("score3.txt");
		string sLine;
		std::getline(infile, sLine);
		int sVal = std::stoi(sLine);
		score3 = sVal;
		infile.close();
	}

	world->GetMainCamera()->SetNearPlane(0.5f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	if (menu == true) {
		world->GetMainCamera()->SetPosition(Vector3(100, -100, 100));
	}
	else {
		world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	}
	
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	InitMixedGridWorld(10, 10, 3.5f, 3.5f);
	//InitSphereGridWorld(1, 1, 3.5f, 3.5f, 10);
	duck = AddGooseToWorld(Vector3(30, 2, 0));
	apple = AddAppleToWorld(Vector3(60, 1, 40));
	
	apple = AddAppleToWorld(Vector3(-20, 1, -80));
	
	apple = AddAppleToWorld(Vector3(12, 1, 0));
	
	apple = AddAppleToWorld(Vector3(58, 1, -59));
	
	apple = AddAppleToWorld(Vector3(-74, 1, 45));
	

	AddCubeToWorld(Vector3(95,5,0), Vector3(5,5,100), 0);
	AddCubeToWorld(Vector3(-95, 5, 0), Vector3(5, 5, 100), 0);
	AddCubeToWorld(Vector3(0, 5, 95), Vector3(90, 5, 5), 0);
	AddCubeToWorld(Vector3(0, 5, -95), Vector3(90, 5, 5), 0);
	//Tree Preset
	GameObject* cover = AddCubeToWorld(Vector3(20, 10, -75), Vector3(5, 2, 5), 0);
	cover->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	GameObject* coverbase = AddCubeToWorld(Vector3(20,4, -75), Vector3(1, 4, 1), 0);
	coverbase->GetRenderObject()->SetColour(Vector4(0.3, 0.2, 0.1, 1));

	GameObject* cover1 = AddCubeToWorld(Vector3(-47, 10, 75), Vector3(5, 2, 5), 0);
	cover1->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	GameObject* coverbase1 = AddCubeToWorld(Vector3(-47, 4, 75), Vector3(1, 4, 1), 0);
	coverbase1->GetRenderObject()->SetColour(Vector4(0.3, 0.2, 0.1, 1));

	GameObject* cover2 = AddCubeToWorld(Vector3(-40, 10, 0), Vector3(5, 2, 5), 0);
	cover2->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	GameObject* coverbase2 = AddCubeToWorld(Vector3(-40, 4, 0), Vector3(1, 4, 1), 0);
	coverbase2->GetRenderObject()->SetColour(Vector4(0.3, 0.2, 0.1, 1));
	// Rest of Worlds
	GameObject* quit = AddCubeToWorld(Vector3(80, -100, 100), Vector3(5,5,5), 0);
	quit->SetName("quit");
	quit->GetRenderObject()->SetDefaultTexture(exitTex);
	GameObject* start = AddCubeToWorld(Vector3(100, -100, 80), Vector3(5, 5, 5), 0);
	start->SetName("start");
	start->GetRenderObject()->SetDefaultTexture(startTex);
	GameObject* highscore = AddCubeToWorld(Vector3(120, -100, 100), Vector3(5, 5, 5), 0);
	highscore->SetName("highscore");
	highscore->GetRenderObject()->SetDefaultTexture(highTex);

	GameObject* pond = AddCubeToWorld(Vector3(70,0.01,70), Vector3(20, 0.01, 20), 0);
	pond->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
	pond->SetName("pond");

	guard1 = AddParkKeeperToWorld(Vector3(40, 2, 0));
	guard1->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	GameObject* nest = AddCubeToWorld(Vector3(80, 1.7, 80), Vector3(5, 1.5, 5), 0);
	nest->GetRenderObject()->SetColour(Vector4(0.5, 0.2, 0, 1));
	nest->SetName("nest");

	AddFloorToWorld(Vector3(0, -2, 0));
}

//From here on it's functions to add in objects to the world!

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("floor");

	Vector3 floorSize = Vector3(100, 2, 100);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(floorSize);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();
	floor->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	floor->GetRenderObject()->SetDefaultTexture(NULL);
	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject("sphere");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetWorldScale(sphereSize);
	sphere->GetTransform().SetWorldPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();
	sphere->GetRenderObject()->SetDefaultTexture(NULL);
	sphere->GetRenderObject()->SetColour(Vector4(0.9, 0.9, 0.8, 1));
	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform().SetWorldPosition(position);
	cube->GetTransform().SetWorldScale(dimensions);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetRenderObject()->SetDefaultTexture(NULL);
	cube->GetRenderObject()->SetColour(Vector4(0.9, 0.8, 0.4, 1));
	world->AddGameObject(cube);

	return cube;
}

Goose* TutorialGame::AddGooseToWorld(const Vector3& position)
{
	float size			= 1.0f;
	float inverseMass	= 1.0f;

	Goose* goose = new Goose("goose");


	SphereVolume* volume = new SphereVolume(size);
	goose->SetBoundingVolume((CollisionVolume*)volume);

	goose->GetTransform().SetWorldScale(Vector3(size,size,size) );
	goose->GetTransform().SetWorldPosition(Vector3(30, 1, 30));

	goose->SetRenderObject(new RenderObject(&goose->GetTransform(), gooseMesh, nullptr, basicShader));
	goose->SetPhysicsObject(new PhysicsObject(&goose->GetTransform(), goose->GetBoundingVolume()));

	goose->GetPhysicsObject()->SetInverseMass(inverseMass);
	goose->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(goose);

	return goose;
}

Keeper* TutorialGame::AddParkKeeperToWorld(const Vector3& position)
{
	float meshSize = 4.0f;
	float inverseMass = 0.5f;

	Keeper* keeper = new Keeper("keeper");

	AABBVolume* volume = new AABBVolume(Vector3(0.3, 0.9f, 0.3) * meshSize);
	keeper->SetBoundingVolume((CollisionVolume*)volume);

	keeper->GetTransform().SetWorldScale(Vector3(meshSize, meshSize, meshSize));
	keeper->GetTransform().SetWorldPosition(position);

	keeper->SetRenderObject(new RenderObject(&keeper->GetTransform(), keeperMesh, nullptr, basicShader));
	keeper->SetPhysicsObject(new PhysicsObject(&keeper->GetTransform(), keeper->GetBoundingVolume()));

	keeper->GetPhysicsObject()->SetInverseMass(inverseMass);
	keeper->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(keeper);

	return keeper;
}

GameObject* TutorialGame::AddCharacterToWorld(const Vector3& position) {
	float meshSize = 4.0f;
	float inverseMass = 0.5f;

	auto pos = keeperMesh->GetPositionData();

	Vector3 minVal = pos[0];
	Vector3 maxVal = pos[0];

	for (auto& i : pos) {
		maxVal.y = max(maxVal.y, i.y);
		minVal.y = min(minVal.y, i.y);
	}

	GameObject* character = new GameObject();

	float r = rand() / (float)RAND_MAX;


	AABBVolume* volume = new AABBVolume(Vector3(0.3, 0.9f, 0.3) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform().SetWorldScale(Vector3(meshSize, meshSize, meshSize));
	character->GetTransform().SetWorldPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), r > 0.5f ? charA : charB, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(character);

	return character;
}

Apple* TutorialGame::AddAppleToWorld(const Vector3& position) {
	Apple* apple = new Apple("apple");

	SphereVolume* volume = new SphereVolume(0.7f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform().SetWorldScale(Vector3(4, 4, 4));
	apple->GetTransform().SetWorldPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), appleMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();
	apple->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	world->AddGameObject(apple);
	

	return apple;
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);


	GameObject* bonus = AddCubeToWorld(Vector3(20,2,10), cubeDims);
	bonus->SetName("bonus");
	AddSphereToWorld(Vector3(50,2,-40), sphereRadius);

	
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
	
}

void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(8, 8, 8);

	float	invCubeMass = 5;
	int		numLinks	= 25;
	float	maxDistance	= 30;
	float	cubeDistance = 20;

	Vector3 startPos = Vector3(500, 1000, 500);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);

	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}

	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

void TutorialGame::SimpleGJKTest() {
	Vector3 dimensions		= Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(100, 2, 100);

	GameObject* fallingCube = AddCubeToWorld(Vector3(0, 20, 0), dimensions, 10.0f);
	GameObject* newFloor	= AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);

	delete fallingCube->GetBoundingVolume();
	delete newFloor->GetBoundingVolume();

	fallingCube->SetBoundingVolume((CollisionVolume*)new OBBVolume(dimensions));
	newFloor->SetBoundingVolume((CollisionVolume*)new OBBVolume(floorDimensions));

}