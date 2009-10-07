/*
 * CGameControl.cpp 
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 */

#include "CGameControl.h"
#include "fileio/CExeFile.h"
#include "fileio/CPatcher.h"
#include "fileio/CTileLoader.h"
#include "fileio.h"
#include "CLogFile.h"
#include "sdl/sound/CSound.h"

CGameControl::CGameControl() {
	m_mode = GAMELAUNCHER;
	m_Episode = 0;
	m_ChosenGame = 0;

	m_EGAGraphics = NULL;
	m_Messages = NULL;
}


////
// Initialization Routine
////
bool CGameControl::init(char mode)
{
	m_mode = mode;
	if(m_mode == GAMELAUNCHER)
	{
		// Resources for the main menu.
		if(!loadResources(1, "games/EP1"))	return false;

		// Load the graphics for menu and background.
		mp_GameLauncher = new CGameLauncher();
		return mp_GameLauncher->init();
	}
	else if(m_mode == PASSIVE)
	{	
		// Create mp_PassiveMode object used for the screens while Player is not playing
		mp_PassiveMode = new CPassive( m_Episode, m_DataDirectory );
		if( mp_PassiveMode->init() ) return true;
	}
	else if(m_mode == PLAYGAME)
	{
		char episode, numplayers, difficulty;
		std::string gamepath;

		m_mode = PLAYGAME;
		episode = mp_PassiveMode->getEpisode();
		numplayers = mp_PassiveMode->getNumPlayers();
		difficulty = mp_PassiveMode->getDifficulty();
		gamepath = mp_PassiveMode->getGamePath();

		mp_PlayGame = new CPlayGame(episode, WORLD_MAP_LEVEL,
						numplayers, difficulty,
						gamepath);
		return mp_PlayGame->init();
	}
	return false;
}

bool CGameControl::loadResources(unsigned short Episode, const std::string& DataDirectory)
{
CExeFile ExeFile(Episode, DataDirectory);
int version;
unsigned char *p_exedata;
	
	m_Episode = Episode;
	m_DataDirectory = DataDirectory;

	// TODO: not very readable. Check if there is a function for handling that.
	if( m_DataDirectory.size() > 0 && m_DataDirectory[m_DataDirectory.size()-1] != '/' )
		m_DataDirectory += "/";

    	// Get the EXE of the game and decompress it if needed.
    	if(!ExeFile.readData()) return false;

    	version = ExeFile.getEXEVersion();
	p_exedata = ExeFile.getData();

	g_pLogFile->ftextOut("Commander Keen Episode %d (Version %d.%d) was detected.<br>", Episode, version/100, version%100);
	if(version == 134) g_pLogFile->ftextOut("This version of the game is not supported!<br>");

	if(ExeFile.getData() == NULL) {
		g_pLogFile->textOut(RED, "CGameControl::loadResources: Could not load data from the EXE File<br>");
		return false;
	}

	// Patch the EXE-File-Data directly in the memory.
	{
		CPatcher Patcher(Episode, version, p_exedata, DataDirectory);
		Patcher.patchMemory();
	}

	// Load tile attributes.
	{
		CTileLoader TileLoader( Episode, version, p_exedata );
		if(!TileLoader.load()) {
			g_pLogFile->textOut(RED, "CGameControl::loadResources: Could not load data for the tiles<br>");
			return false;
		}
	}

	// Decode the entire graphics for the game (EGALATCH, EGASPRIT, etc.)
	if(m_EGAGraphics) delete m_EGAGraphics; // except for the first start of a game this always happens
	m_EGAGraphics = new CEGAGraphics(Episode, DataDirectory); // Path is relative to the data dir
    	if(!m_EGAGraphics) return false;

    	m_EGAGraphics->loadData();

    	// load the strings. TODO: After that this one will replace loadstrings
    	//m_Messages = new CMessages();
    	//m_Messages->readData(Episode, p_exedata, version, DataDirectory);
	loadstrings();

	// Load the sound data
	g_pSound->loadSoundData(Episode, DataDirectory);
	return true;
}


////
// Process Routine
////
// This function is run every time, the Timer says so, through.
void CGameControl::process()
{
	//// First we must know in which mode we are. There are three:
	// The first menu of the game
	if(m_mode == GAMELAUNCHER)
	{
		// Launch the code of the Startmenu here! The one for choosing the games
		mp_GameLauncher->process();
		m_ChosenGame = mp_GameLauncher->getChosengame();

		if( mp_GameLauncher->waschosen() )
		{
			//// Game has been chosen. Launch it!
			// Get the path were to Launch the game
			m_DataDirectory = "games/" + mp_GameLauncher->getDirectory( m_ChosenGame );

			// We have to check which Episode is used
			m_Episode = mp_GameLauncher->retrievetEpisode( m_ChosenGame );

			if( m_Episode > 0 ) // The game has to have a valid episode!
			{
				// Load the Resources
				if( loadResources( m_Episode, m_DataDirectory ) )
				{
					if(init(PASSIVE)) cleanup(GAMELAUNCHER);
					else
					{
						mp_GameLauncher->letchooseagain();
						delete mp_PassiveMode;
					}
				}
			}
			else
			{
				mp_GameLauncher->letchooseagain();
				g_pLogFile->textOut(RED,"No Suitable game was detected in this path! Please check its contents!\n");
			}
		}
		else if(mp_GameLauncher->getQuit())
		{
			// User chose exit. So quit...
			m_mode = SHUTDOWN;
		}
	}
	// Intro, Title screen, and demo mode are performed by the passive class CPassive
	else if(m_mode == PASSIVE)
	{
		mp_PassiveMode->process();

		// check here what the player chose from the menu over the passive mode.
		// NOTE: Demo is not part of playgame anymore!!
		if(mp_PassiveMode->mustStartGame())
		{
			init( PLAYGAME );
			delete mp_PassiveMode;
		}

		// User wants to exit. Called from the PassiveMode
		if(mp_PassiveMode->getExitEvent())
			m_mode = SHUTDOWN;
	}
	// Here goes the PlayGame Engine
	else if(m_mode == PLAYGAME)
	{
		// The player is playing the game. It also includes scenes like ending
		mp_PlayGame->process();

		if( mp_PlayGame->getEndGame() )
		{
			cleanup();
			init(PASSIVE);
		}
		else if( mp_PlayGame->getExitEvent() )
			m_mode = SHUTDOWN;
	}
	// That should never happen!
	else
	{
		// Something went wrong here! send warning and load startmenu
		m_mode = GAMELAUNCHER;
	}

}

void CGameControl::cleanup(char mode)
{
	if(mode == GAMELAUNCHER)
	{
		// Launch the cleanup-code of the Startmenu here! The one for choosing the games
		mp_GameLauncher->cleanup();
		delete mp_GameLauncher;
	}
	else if(mode == PASSIVE)
	{
		// If in passive mode, cleanup here!
		mp_PassiveMode->cleanup();
		delete mp_PassiveMode;
	}
	else if(mode == PLAYGAME)
	{
		// Tie up when in game play
		mp_PlayGame->cleanup();
		delete mp_PlayGame;
	} 
}

CGameControl::~CGameControl() {
	if(m_EGAGraphics) delete m_EGAGraphics;
	if(m_Messages) delete m_Messages;
}
