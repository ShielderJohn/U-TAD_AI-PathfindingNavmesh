#ifndef __STEERING_H__
#define __STEERING_H__

class Character;

class Steering
{
public:
	virtual USVec2D GetSteering(Character* character, const USVec2D& target) = 0;
	virtual float GetAngularSteering(Character* character, float targetRotation) = 0;
	virtual void DrawDebug(Character* character) = 0;

private:

};

#endif