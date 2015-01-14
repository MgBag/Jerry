#pragma once
#define _USE_MATH_DEFINES 

#include <cmath>
#include <allegro5/color.h>
#include "Coordinates.h"
#include "Constants.h"
#include "VelocityVector.h"

class Entity
{
private:
	Coordinates m_ACoordinates;
	Coordinates m_BCoordinates;
	Coordinates m_Offset;
	double m_Width;
	double m_Height;
	ALLEGRO_COLOR m_Color;
	ItemType m_Type;
	bool m_Hit;
	int m_Age;
	bool m_IsAirBorn;
	double m_InitialImpactSpeed;
	bool m_IsCrouching;
	ItemType m_LastImpactType;
	double m_PreviousImpactHeight;

public:
	Entity(double x, double y, double width, double height, double xOffset, double yOffset, ALLEGRO_COLOR color, ItemType type);
	~Entity();

	Coordinates* GetACoordinates();
	Coordinates* GetBCoordinates();
	void SetCoordinates(double x, double y);
	void MoveToOffset(double x, double y);
	void MoveToOffset();

	Coordinates* GetOffset();
	void SetOffset(double xOffset, double yOffset);

	ALLEGRO_COLOR GetColor();
	void SetColor(ALLEGRO_COLOR color);

	double GetWidth();
	double GetHeight();

	void SetWidth(double width);
	void SetHeight(double height);

	bool GetHit();
	void SetHit(bool hit);

	bool GetIsAirBorn();
	void SetIsAirBorn(bool hit);

	int GetAge();
	void SetAge(int age);
	void IncAge();

	ItemType getType();

	double GetInitialImpactSpeed();
	void SetInitialImpactSpeed(double speed);

	bool GetIsCrouching();
	void SetIsCrouching(bool hit);

	ItemType GetLastImpactType();
	void SetLastImpactType(ItemType impactType);

	double GetPreviousImpactHeight();
	void SetPreviousImpactHeight(double preImpactHeight);
};