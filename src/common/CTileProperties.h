/*
 * CTileProperties.h
 *
 *  Created on: 10.06.2010
 *      Author: gerstrong
 *
 *  This new class defines the Properties of one tile
 *  For the Galaxy Engine it will be extended and replaces the
 *  old st_tiles structure
 */

#ifndef CTILEPROPERTIES_H_
#define CTILEPROPERTIES_H_

class CTileProperties
{
public:
	CTileProperties();

	virtual ~CTileProperties();

	int chgtile;         // tile to change to when level completed (for wm)
	// or tile to change to when picked up (in-level)
	unsigned int animOffset;   // starting offset from the base frame

	// Tile Properties start here!
	char animation;
	char behaviour;
	char bup,bright,bdown,bleft;

private:
};

#endif /* CTILEPROPERTIES_H_ */