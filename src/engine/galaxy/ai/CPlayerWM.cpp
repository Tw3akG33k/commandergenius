/*
 * CPlayer.cpp
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 */

#include "CPlayerWM.h"
#include "engine/galaxy/ai/CFlag.h"
#include "common/CBehaviorEngine.h"
#include "sdl/CInput.h"
#include "CVec.h"

const Uint16 WALKBASEFRAME = 130;
const Uint16 SWIMBASEFRAME = 156;

namespace galaxy {

CPlayerWM::CPlayerWM(CMap *pmap, Uint32 x, Uint32 y,
					std::vector<CObject*>& ObjectPtrs):
CObject(pmap, x, y, OBJ_PLAYER),
m_basesprite(130),
m_looking_dir(LEFT),
m_animation(0),
m_animation_time(1),
m_animation_ticker(0),
m_ObjectPtrs(ObjectPtrs)
{
	// TODO Auto-generated constructor stub
	sprite = m_basesprite;

	setupinitialCollisions();
}

/**
 * The main process cycle for the player itself only on the map
 */
void CPlayerWM::process()
{
	// Perform animation cycle
	if(m_animation_ticker >= m_animation_time)
	{
		m_animation++;
		m_animation_ticker = 0;
	}
	else m_animation_ticker++;

	processWalking();
}

/*
 * Processes the walking of the player on map here
 */
void CPlayerWM::processWalking()
{
	size_t movespeed = 50;
	bool walking=false;

	// Normal walking
	if(g_pInput->getHoldedCommand(IC_LEFT))
	{
		moveLeft(movespeed);
		walking = true;
		m_looking_dir = LEFT;
	}
	else if(g_pInput->getHoldedCommand(IC_RIGHT))
	{
		moveRight(movespeed);
		walking = true;
		m_looking_dir = RIGHT;
	}

	if(g_pInput->getHoldedCommand(IC_UP))
	{
		moveUp(movespeed);
		walking = true;

		if(m_looking_dir == LEFT)
			m_looking_dir = LEFTUP;
		else if(m_looking_dir == RIGHT)
			m_looking_dir = RIGHTUP;
		else
			m_looking_dir = UP;
	}
	else if(g_pInput->getHoldedCommand(IC_DOWN))
	{
		moveDown(movespeed);
		walking = true;

		if(m_looking_dir == LEFT)
			m_looking_dir = LEFTDOWN;
		else if(m_looking_dir == RIGHT)
			m_looking_dir = RIGHTDOWN;
		else
			m_looking_dir = DOWN;
	}

	if(g_pInput->getHoldedCommand(IC_STATUS))
		solid = !solid;

	// perform actions depending if the action button was pressed
	if(g_pInput->getPressedCommand(IC_JUMP))
	{
		Uint16 object = mp_Map->getPlaneDataAt(2, getXMidPos(), getYMidPos());
		if(object)
		{
			startLevel(object);
			g_pInput->flushCommands();
		}
	}

	// this means if keen is just walking on the map or swimming in the sea
	if(m_basesprite == WALKBASEFRAME)
		performWalkingAnimation(walking);
	else if(m_basesprite == SWIMBASEFRAME)
		performSwimmingAnimation();

	// This will trigger between swim and walkmode
	checkforSwimming();

}

/**
 * This function will help starting the level for Commander Keen
 */
void CPlayerWM::startLevel(Uint16 object)
{

}

/**
 * This is the function will switch between swim and walk mode
 * Those are the tileproperties to check for
 * 11      Enter water from top	Keen 4
 * 12      Enter water from right	Keen 4
 * 13      Enter water from bottom Keen 4
 * 14      Enter water from left	Keen 4
 */
void CPlayerWM::checkforSwimming()
{
	Uint16 left, right, up, down;
	std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);

	left = Tile[mp_Map->at( getXLeftPos()>>CSF, getYMidPos()>>CSF, 1)].behaviour;
	right = Tile[mp_Map->at( getXRightPos()>>CSF, getYMidPos()>>CSF, 1)].behaviour;
	up = Tile[mp_Map->at( getXMidPos()>>CSF, getYUpPos()>>CSF, 1)].behaviour;
	down = Tile[mp_Map->at( getXMidPos()>>CSF, getYDownPos()>>CSF, 1)].behaviour;

	// from top
	if(up == 11)
		m_basesprite = SWIMBASEFRAME;
	else if(down == 11)
		m_basesprite = WALKBASEFRAME;

	// from right
	if(right == 12)
		m_basesprite = SWIMBASEFRAME;
	else if(left == 12)
		m_basesprite = WALKBASEFRAME;

	// from bottom
	if(down == 13)
		m_basesprite = SWIMBASEFRAME;
	else if(up == 13)
		m_basesprite = WALKBASEFRAME;

	// from left
	if(left == 14)
		m_basesprite = SWIMBASEFRAME;
	else if(right == 14)
		m_basesprite = WALKBASEFRAME;
}

/**
 * This performs the animation when player is walking on the map
 */
void CPlayerWM::performWalkingAnimation(bool walking)
{
	if(m_looking_dir == LEFT)
		sprite = m_basesprite;
	else if(m_looking_dir == RIGHT)
		sprite = m_basesprite + 3;
	else if(m_looking_dir == UP)
		sprite = m_basesprite + 6;
	else if(m_looking_dir == DOWN)
		sprite = m_basesprite + 9;
	else if(m_looking_dir == RIGHTDOWN)
		sprite = m_basesprite + 12;
	else if(m_looking_dir == LEFTDOWN)
		sprite = m_basesprite + 15;
	else if(m_looking_dir == LEFTUP)
		sprite = m_basesprite + 18;
	else if(m_looking_dir == RIGHTUP)
		sprite = m_basesprite + 21;

	if(walking)
	{
		m_animation_time = 5;
		sprite +=  m_animation%3;
	}
	else
		sprite +=  2;
}

void CPlayerWM::performSwimmingAnimation()
{
	if(m_looking_dir == UP)
		sprite = m_basesprite;
	else if(m_looking_dir == RIGHT)
		sprite = m_basesprite + 2;
	else if(m_looking_dir == DOWN)
		sprite = m_basesprite + 4;
	else if(m_looking_dir == LEFT)
		sprite = m_basesprite + 6;
	else if(m_looking_dir == RIGHTUP)
		sprite = m_basesprite + 8;
	else if(m_looking_dir == RIGHTDOWN)
		sprite = m_basesprite + 10;
	else if(m_looking_dir == LEFTDOWN)
		sprite = m_basesprite + 12;
	else if(m_looking_dir == LEFTUP)
		sprite = m_basesprite + 14;

	m_animation_time = 5;
	sprite +=  m_animation%2;
}

CPlayerWM::~CPlayerWM() {
	// TODO Auto-generated destructor stub
}

}
