cmake_minimum_required( VERSION 3.0 )
project( AIBot C CXX )

#add_definitions(-DDEBUG_FRAMEWORK)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

# Filters
include( Filters.cmake )


SET( AIBOT_BUILDFILES 

	ChangeLog.txt
	CheckInstall.bat
	CleanAll.bat
	CMakeLists.txt
	CreateJob.bat
	Filters.cmake
	GenAIBootCampViewerFiles.bat
	GenAIBootCampViewerFiles.py
	INSTALL.txt
	PlayStandaloneMatch.bat
	rebuild_vis_sln_x64.bat
	TeamInfo.team
)

SET( AIBOT_HEADERS 

	#Framework
	Bot.h	
	BotLogicIF.h
	Globals.h
	LevelInfo.h
	Logger.h
	NPCInfo.h
	ObjectInfo.h
	TileInfo.h
	TurnInfo.h

	#MY_BOT
	MyBotLogic/MyBotLogic.h
	MyBotLogic/Map.h
	MyBotLogic/Node.h
	MyBotLogic/Singleton.h
	MyBotLogic/MiCoMa.h
	MyBotLogic/Npc.h
	MyBotLogic/LoggerPath.h
	MyBotLogic/Objective.h

	#BehaviourTree
	MyBotLogic/BehaviourTree/BaseBloc.h
	MyBotLogic/BehaviourTree/BlackBoard.h
	MyBotLogic/BehaviourTree/BlocAction
	MyBotLogic/BehaviourTree/BlocComposite.h
	MyBotLogic/BehaviourTree/BlocDecorator.h
	MyBotLogic/BehaviourTree/BlocFail.h
	MyBotLogic/BehaviourTree/BlocFindBGBG.h
	MyBotLogic/BehaviourTree/BlocFindBGBNpc.h
	MyBotLogic/BehaviourTree/BlocHasGoal.h
	MyBotLogic/BehaviourTree/BlocInvert.h
	MyBotLogic/BehaviourTree/BlocLoop.h
	MyBotLogic/BehaviourTree/BlocRoot.h
	MyBotLogic/BehaviourTree/BlocSelect.h
	MyBotLogic/BehaviourTree/BlocSequence.h
	MyBotLogic/BehaviourTree/BlocSuccess.h
	MyBotLogic/BehaviourTree/BlocUpdateActionList.h
	MyBotLogic/BehaviourTree/GeneralAction.h
)

SET( AIBOT_SRC 

	#Framework
	Bot.cpp
	LevelInfo.cpp
	Logger.cpp
	TurnInfo.cpp

	#MY_BOT
	MyBotLogic/MyBotLogic.cpp
	MyBotLogic/BehaviourTree/BlackBoard.cpp
	MyBotLogic/BehaviourTree/BehaviourTree.cpp
	MyBotLogic/Map.cpp
	MyBotLogic/Node.cpp
	MyBotLogic/MiCoMa.cpp
	MyBotLogic/Npc.cpp	

	#Main
	Main.cpp
)

SET(jobfiles "${AIBOT_HEADERS};${AIBOT_SRC};${AIBOT_BUILDFILES}")
file(WRITE jobfiles.txt "${jobfiles}")

add_executable(
	aibot

	${AIBOT_BUILDFILES}
	${AIBOT_HEADERS}
	${AIBOT_SRC}
)

CREATE_VISUAL_STUDIO_FILTERS(AIBOT)


