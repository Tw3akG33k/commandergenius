
#ifndef	_CRAY_H_
#define _CRAY_H_

#define RAY_SPEED   		108 // problem here: shot speeds for K3 guns/K1 tank should be 124 while 108 is right for K2 enemies
#define RAY_AUTO_SPEED		120
#define RAY_ZAPZOT_TIME    	10

#include "../../../common/CObject.h"
#include "../../../common/CPlayer.h"
#include "../../../common/objenums.h"
#include <vector>

// reference to ../misc.cpp
unsigned int rnd(void);

class CRay : public CObject
{
public:
	CRay(CMap *p_map, Uint32 x, Uint32 y,
		direction_t dir, object_t byType=OBJ_RAY, size_t byID=0);

	virtual void process();
	void moveinAir();
	void setOwner(object_t type, unsigned int index);
	void getTouchedBy(CObject &theObject);
	bool isFlying();
	void setZapped();
	void gotZapped();

	direction_t m_Direction;
	int m_speed;

	enum {
		RAY_STATE_FLY,
		RAY_STATE_SETZAPZOT,
		RAY_STATE_ZAPZOT
	} state;

protected:
	bool m_automatic_raygun;
	char m_pShotSpeed;

	char zapzottimer;

	unsigned char dontHitEnable;
	unsigned int dontHit;         // index of an object type ray will not harm

	struct{
		object_t obj_type;
		unsigned int ID;
	}owner;

	// for earth chunks
	int baseframe;
};

#endif //_CRAY_H_
