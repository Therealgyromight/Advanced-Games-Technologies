#include "Apple.h"


using namespace NCL;
using namespace CSC8503;

NCL::CSC8503::Apple::Apple(string name) {
	Apple::SetName(name);
}

NCL::CSC8503::Apple::~Apple()
{
}

void NCL::CSC8503::Apple::OnCollisionBegin(GameObject* otherObject)  {
	
	
}

void NCL::CSC8503::Apple::OnCollisionEnd(GameObject* otherObject) {

}