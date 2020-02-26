#pragma once

#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include <iostream>
#include <fstream>
#include <string>

namespace NCL {
	namespace CSC8503 {
		class Goose;
		class Keeper;
		class Apple;
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();
			static bool fileExists(const string& file);
			virtual void UpdateGame(float dt);
			void SetCameraPos();
		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void BridgeConstraintTest();
			void SimpleGJKTest();

			void ShowHighScore();
			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();
			void LockedCameraMovement();
			
			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			//IT'S HAPPENING
			Goose* AddGooseToWorld(const Vector3& position);
			Keeper* AddParkKeeperToWorld(const Vector3& position);
			GameObject* AddCharacterToWorld(const Vector3& position);
			Apple* AddAppleToWorld(const Vector3& position);


			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			bool menu = true;
			bool useGravity;
			bool inSelectionMode;
			float time = 180.0f;
			float		forceMagnitude;
			int score = 0;
			int score1 = 0;
			int score2 = 0;
			int score3 = 0;

			GameObject* selectionObject = nullptr;

			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;
			OGLTexture* startTex = nullptr;
			OGLTexture* exitTex = nullptr;
			OGLTexture* highTex = nullptr;

			//Coursework Meshes
			OGLMesh*	gooseMesh	= nullptr;
			OGLMesh*	keeperMesh	= nullptr;
			OGLMesh*	appleMesh	= nullptr;
			OGLMesh*	charA		= nullptr;
			OGLMesh*	charB		= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}
			Apple* apple;
			GameObject* apple2;
			GameObject* apple3;
			GameObject* apple4;
			GameObject* apple5;
			Goose* duck;
			Keeper* guard1;
		};
	}
}

