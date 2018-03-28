#include "Game.h"
#include <sstream>
#include <SFML\Audio.hpp>
#include <iostream>
#include <cstdio>
#include <ctime>

using namespace tle;
using namespace std;

bool level1 = true;
bool level2 = true;
bool level3 = false;

//sf::SoundBuffer guardDeathBuffer;
//sf::Sound guardDeathSound;

bool thiefRotation = true;
float guardSightSpeed = 0.033;
bool guardCollision = false;

const EKeyCode ThiefForward = Key_W;
const EKeyCode ThiefBackward = Key_S;
const EKeyCode TurnLeft = Key_A;
const EKeyCode TurnRight = Key_D;

const EKeyCode KillGuard = Key_Q;
const EKeyCode hideInCupboard = Key_H;

const EKeyCode ReturnToMenu = Key_Escape;
const EKeyCode ControlsPage = Key_I;
const EKeyCode PauseGame = Key_P;
const EKeyCode QuitGame = Key_Q;
const EKeyCode Return = Key_B;

Game::Game()
{

}

Game::~Game()
{
	myEngine->Delete();
}

void Game::createEngine()
{
	myEngine = tle::New3DEngine(tle::kTLX);
	myEngine->StartWindowed();
	myEngine->AddMediaFolder("D:\\My Game Projects\\Media"); 
}

void Game::initializeStats()
{
	frame = 1.0f;
	fps = 60.0f;

	guardXMatrix = 8;
	guardYMatrix = 9;
	guardZMatrix = 10;

	playerSpotted = 1;

	rotatedWall = 0;
	notRotatedWall = 2;
	
	noYAxis = 0.0f;

	guardVision = 30.0f;

	lvl1XSpawn = 0.0f;
	lvl1YSpawn = 0.0f;
	lvl1ZSpawn = 30.0f;

	lvl2XSpawn = 0.0f;
	lvl2YSpawn = 0.0f;
	lvl2ZSpawn = -545.0f;
	
	lvl3XSpawn = 500.0f;
	lvl3YSpawn = 0.0f;
	lvl3ZSpawn = 20.0f;

	wallXCollision = 5.0f;
	wallMinZCollision = 3.0f;
	wallMaxZCollision = 10.0f;

	rotWallXCollision = 10.0f;
	rotWallZCollision = 5.0f;
	thiefMove = 0.1f;
	
	thiefForwardSpeed = 0.1f;
	thiefBackwardSpeed = -0.1f;
	thiefRotateSpeed = 1.0f;
	thiefStill = 0.0f;
	thiefHalfSpeed = 1.0f;

	guardSpeed = 0.02f;
	fastGuardSpeed = 0.0f;
	guardDeadSpeed = 0.0f;
	guard2TurnTimer = 100.0f;
	guard3TurnTimer = 100.0f;
	guardTurn = 180.0f;
	
	guardDeathX = 0;
	guardDeathY = 1000;
	guardDeathZ = 0;

	cupboardEnter = 25.0f;
	
	waypointDist = 10.0f;

	guard2Turn = false;
	lvl3guardTurn = false;
	
	guard2Killed = false;
	
	playerHidden = false;

	waypoint1Reached = false;
	waypoint2Reached = false;
	waypoint3Reached = false;

	wallCollision = false;
}

void Game::initializeAssets()
{
	// Load meshes
	floorMesh = myEngine->LoadMesh("floor.x");
	blackFloorMesh = myEngine->LoadMesh("floor.x");
	winboxMesh = myEngine->LoadMesh("box.x");
	
	thiefMesh = myEngine->LoadMesh("sierra.x");

	cupboardMesh = myEngine->LoadMesh("box.x");
	
	//Load models
	blackFloor = blackFloorMesh->CreateModel(0, -1000, 1000);
	blackFloor->SetSkin("black.png");
	
	floor = floorMesh->CreateModel(0, -10, 0);
	floor->SetSkin("green.png");
	floor->Scale(2);

	winbox = winboxMesh->CreateModel(495, 0, -250);
	winbox->SetSkin("purple.png");
	
	//Load Array meshes
	wallMesh = myEngine->LoadMesh("box.x");
	map1Walls[MAP1_NUM_WALLS];

	//Load Array Meshes////////////////////////////////
	waypointMesh = myEngine->LoadMesh("box.x");
	eventBoxMesh = myEngine->LoadMesh("box.x");
	guardSightMesh = myEngine->LoadMesh("box.x");
	
	//EVENT TRIGGER BOXES///////////////////////////////
	eventBox[0] = eventBoxMesh->CreateModel(-300, 20, -300);
	eventBox[0]->SetSkin("blue.png");

	//CUPBOARDS ///////////////////////////////
	cupboards[0] = cupboardMesh->CreateModel(-250, 0, -315);
	cupboards[0]->SetSkin("brown.png");
	cupboards[0]->ScaleY(4);

	//GUARD PATROLS///////////////////////////////
	waypoints[0] = waypointMesh->CreateModel(-140, 20, -420);
	waypoints[1] = waypointMesh->CreateModel(-40, 20, -420);
	
	waypoints[2] = waypointMesh->CreateModel(-175, 20, -390);
	waypoints[3] = waypointMesh->CreateModel(-175, 20, -300);

	waypoints[4] = waypointMesh->CreateModel(-420, 20, -310);
	waypoints[5] = waypointMesh->CreateModel(-200, 20, -310);

	//Level 1 Guards/////////////////////////////////

	lvl1GuardMesh = myEngine->LoadMesh("casual_A.x");
	lvl1Guards[NUM_GUARDS];

	lvl1Guards[0] = lvl1GuardMesh->CreateModel(-115, 0, 100);
	lvl1Guards[0]->Scale(10);
	
	lvl1Guards[1] = lvl1GuardMesh->CreateModel(-185, 0, 190);
	lvl1Guards[1]->Scale(10);
	lvl1Guards[1]->RotateLocalY(-90);

	//Level 2 Guards/////////////////////////////////
	lvl2GuardMesh = myEngine->LoadMesh("casual_A.x");
	
	lvl2Guards[NUM_GUARDS2];

	lvl2Guards[0] = lvl2GuardMesh->CreateModel(-100, 0, -420);
	lvl2Guards[0]->Scale(10);
	lvl2Guards[0]->RotateLocalY(-90);

	lvl2Guards[1] = lvl2GuardMesh->CreateModel(-175, 0, -340);
	lvl2Guards[1]->Scale(10);

	lvl2Guards[2] = lvl2GuardMesh->CreateModel(-420, 0, -270);
	lvl2Guards[2]->Scale(10);
	lvl2Guards[2]->RotateLocalY(180);

	//Level 3 Guards/////////////////////////////////
	lvl3GuardMesh = myEngine->LoadMesh("casual_A.x");

	lvl3Guards[NUM_GUARDS2];

	lvl3Guards[0] = lvl2GuardMesh->CreateModel(660, 0, -50);
	lvl3Guards[0]->Scale(10);

	lvl3Guards[1] = lvl2GuardMesh->CreateModel(760, 0, -50);
	lvl3Guards[1]->Scale(10);
	lvl3Guards[1]->RotateLocalY(180);

	lvl3Guards[2] = lvl2GuardMesh->CreateModel(495, 0, -150);
	lvl3Guards[2]->Scale(10);
	lvl3Guards[2]->RotateLocalY(180);
	

	///////////////////////////////////////////////////////////////////////
	
	thief = thiefMesh->CreateModel(lvl1XSpawn, lvl1YSpawn, lvl1ZSpawn);  //SECOND LEVEL COORDINATES
	thief->Scale(14);
	
	myCamera = myEngine->CreateCamera(tle::kManual, 0.0f, 0.0f, 1000.0f);	////LEVEL 1 FIRST CAMERA ANGLE
	myCamera->RotateLocalX(80);
}

void Game::map1()
{
/////////////////////////////////////////////////////////////////////////////////////////////////
//FIRST LEVEL
/////////////////////////////////////////////////////////////////////////////////////////////////

	map1Walls[0] = wallMesh->CreateModel(0, 0, -25);
	map1Walls[0]->ScaleZ(6);
	map1Walls[0]->ScaleY(5);
	map1Walls[0]->RotateLocalY(90);
	map1Walls[0]->SetSkin("red.png");

	map1Walls[1] = wallMesh->CreateModel(-25, 0, 10);
	map1Walls[1]->ScaleZ(6);
	map1Walls[1]->ScaleY(5);
	map1Walls[1]->SetSkin("red.png");

	map1Walls[2] = wallMesh->CreateModel(25, 0, 10);
	map1Walls[2]->ScaleZ(6);
	map1Walls[2]->ScaleY(5);
	map1Walls[2]->SetSkin("red.png");

	map1Walls[3] = wallMesh->CreateModel(-50, 0, 45);
	map1Walls[3]->ScaleZ(6);
	map1Walls[3]->ScaleY(5);
	map1Walls[3]->RotateLocalY(90);
	map1Walls[3]->SetSkin("red.png");

	map1Walls[4] = wallMesh->CreateModel(-110, 0, 45);
	map1Walls[4]->ScaleZ(6);
	map1Walls[4]->ScaleY(5);
	map1Walls[4]->RotateLocalY(90);
	map1Walls[4]->SetSkin("red.png");

	map1Walls[5] = wallMesh->CreateModel(25, 0, 70);
	map1Walls[5]->ScaleZ(6);
	map1Walls[5]->ScaleY(5);
	map1Walls[5]->SetSkin("red.png");

	map1Walls[6] = wallMesh->CreateModel(-10, 0, 95);
	map1Walls[6]->ScaleZ(6);
	map1Walls[6]->ScaleY(5);
	map1Walls[6]->RotateLocalY(90);
	map1Walls[6]->SetSkin("red.png");

	map1Walls[7] = wallMesh->CreateModel(-70, 0, 95);
	map1Walls[7]->ScaleZ(6);
	map1Walls[7]->ScaleY(5);
	map1Walls[7]->RotateLocalY(90);
	map1Walls[7]->SetSkin("red.png");

	map1Walls[8] = wallMesh->CreateModel(-95, 0, 130);
	map1Walls[8]->ScaleZ(6);
	map1Walls[8]->ScaleY(5);
	map1Walls[8]->SetSkin("red.png");

	map1Walls[9] = wallMesh->CreateModel(-145, 0, 70);
	map1Walls[9]->ScaleZ(6);
	map1Walls[9]->ScaleY(5);
	map1Walls[9]->SetSkin("red.png");

	map1Walls[10] = wallMesh->CreateModel(-145, 0, 130);
	map1Walls[10]->ScaleZ(6);
	map1Walls[10]->ScaleY(5);
	map1Walls[10]->SetSkin("red.png");

	map1Walls[11] = wallMesh->CreateModel(-170, 0, 165);
	map1Walls[11]->ScaleZ(6);
	map1Walls[11]->ScaleY(5);
	map1Walls[11]->RotateLocalY(90);
	map1Walls[11]->SetSkin("red.png");

	map1Walls[12] = wallMesh->CreateModel(-230, 0, 165);
	map1Walls[12]->ScaleZ(6);
	map1Walls[12]->ScaleY(5);
	map1Walls[12]->RotateLocalY(90);
	map1Walls[12]->SetSkin("red.png");

	map1Walls[13] = wallMesh->CreateModel(-95, 0, 190);
	map1Walls[13]->ScaleZ(6);
	map1Walls[13]->ScaleY(5);
	map1Walls[13]->SetSkin("red.png");

	map1Walls[14] = wallMesh->CreateModel(-130, 0, 215);
	map1Walls[14]->ScaleZ(6);
	map1Walls[14]->ScaleY(5);
	map1Walls[14]->RotateLocalY(90);
	map1Walls[14]->SetSkin("red.png");

	map1Walls[15] = wallMesh->CreateModel(-190, 0, 215);
	map1Walls[15]->ScaleZ(6);
	map1Walls[15]->ScaleY(5);
	map1Walls[15]->RotateLocalY(90);
	map1Walls[15]->SetSkin("red.png");

	map1Walls[16] = wallMesh->CreateModel(-250, 0, 215);
	map1Walls[16]->ScaleZ(6);
	map1Walls[16]->ScaleY(5);
	map1Walls[16]->RotateLocalY(90);
	map1Walls[16]->SetSkin("red.png");

	map1Walls[17] = wallMesh->CreateModel(-270, 0, 165);
	map1Walls[17]->ScaleZ(2);
	map1Walls[17]->ScaleY(5);
	map1Walls[17]->RotateLocalY(90);
	map1Walls[17]->SetSkin("red.png");

	map1Walls[18] = wallMesh->CreateModel(-285, 0, 190);
	map1Walls[18]->ScaleZ(6);
	map1Walls[18]->ScaleY(5);
	map1Walls[18]->SetSkin("blue.png");
}

void Game::map2()
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//SECOND LEVEL

	map2Walls[0] = wallMesh->CreateModel(-25, 0, -540);
	map2Walls[0]->ScaleZ(6);
	map2Walls[0]->ScaleY(5);
	map2Walls[0]->SetSkin("red.png");

	map2Walls[1] = wallMesh->CreateModel(25, 0, -540);
	map2Walls[1]->ScaleZ(6);
	map2Walls[1]->ScaleY(5);
	map2Walls[1]->SetSkin("red.png");

	map2Walls[2] = wallMesh->CreateModel(0, 0, -575);
	map2Walls[2]->ScaleZ(6);
	map2Walls[2]->ScaleY(5);
	map2Walls[2]->RotateLocalY(90);
	map2Walls[2]->SetSkin("red.png");

	map2Walls[3] = wallMesh->CreateModel(25, 0, -480);
	map2Walls[3]->ScaleZ(6);
	map2Walls[3]->ScaleY(5);
	map2Walls[3]->SetSkin("red.png");

	map2Walls[4] = wallMesh->CreateModel(-25, 0, -480);
	map2Walls[4]->ScaleZ(6);
	map2Walls[4]->ScaleY(5);
	map2Walls[4]->SetSkin("red.png");

	map2Walls[5] = wallMesh->CreateModel(-50, 0, -445);
	map2Walls[5]->ScaleZ(6);
	map2Walls[5]->ScaleY(5);
	map2Walls[5]->RotateLocalY(90);
	map2Walls[5]->SetSkin("red.png");

	map2Walls[6] = wallMesh->CreateModel(25, 0, -420);
	map2Walls[6]->ScaleZ(6);
	map2Walls[6]->ScaleY(5);
	map2Walls[6]->SetSkin("red.png");

	map2Walls[7] = wallMesh->CreateModel(-10, 0, -395);
	map2Walls[7]->ScaleZ(6);
	map2Walls[7]->ScaleY(5);
	map2Walls[7]->RotateLocalY(90);
	map2Walls[7]->SetSkin("red.png");

	map2Walls[8] = wallMesh->CreateModel(-70, 0, -395);
	map2Walls[8]->ScaleZ(6);
	map2Walls[8]->ScaleY(5);
	map2Walls[8]->RotateLocalY(90);
	map2Walls[8]->SetSkin("red.png");

	map2Walls[9] = wallMesh->CreateModel(-110, 0, -445);
	map2Walls[9]->ScaleZ(6);
	map2Walls[9]->ScaleY(5);
	map2Walls[9]->RotateLocalY(90);
	map2Walls[9]->SetSkin("red.png");

	map2Walls[10] = wallMesh->CreateModel(-130, 0, -395);
	map2Walls[10]->ScaleZ(6);
	map2Walls[10]->ScaleY(5);
	map2Walls[10]->RotateLocalY(90);
	map2Walls[10]->SetSkin("red.png");

	map2Walls[11] = wallMesh->CreateModel(-170, 0, -445);
	map2Walls[11]->ScaleZ(6);
	map2Walls[11]->ScaleY(5);
	map2Walls[11]->RotateLocalY(90);
	map2Walls[11]->SetSkin("red.png");

	map2Walls[12] = wallMesh->CreateModel(-155, 0, -360);
	map2Walls[12]->ScaleZ(6);
	map2Walls[12]->ScaleY(5);
	map2Walls[12]->SetSkin("red.png");

	map2Walls[13] = wallMesh->CreateModel(-205, 0, -420);
	map2Walls[13]->ScaleZ(6);
	map2Walls[13]->ScaleY(5);
	map2Walls[13]->SetSkin("red.png");

	map2Walls[14] = wallMesh->CreateModel(-205, 0, -360);
	map2Walls[14]->ScaleZ(6);
	map2Walls[14]->ScaleY(5);
	map2Walls[14]->SetSkin("red.png");

	map2Walls[15] = wallMesh->CreateModel(-155, 0, -300);
	map2Walls[15]->ScaleZ(6);
	map2Walls[15]->ScaleY(5);
	map2Walls[15]->SetSkin("red.png");

	map2Walls[16] = wallMesh->CreateModel(-190, 0, -275);
	map2Walls[16]->ScaleZ(6);
	map2Walls[16]->ScaleY(5);
	map2Walls[16]->RotateLocalY(90);
	map2Walls[16]->SetSkin("red.png");

	map2Walls[17] = wallMesh->CreateModel(-230, 0, -325);
	map2Walls[17]->ScaleZ(6);
	map2Walls[17]->ScaleY(5);
	map2Walls[17]->RotateLocalY(90);
	map2Walls[17]->SetSkin("red.png");

	map2Walls[18] = wallMesh->CreateModel(-290, 0, -325);
	map2Walls[18]->ScaleZ(6);
	map2Walls[18]->ScaleY(5);
	map2Walls[18]->RotateLocalY(90);
	map2Walls[18]->SetSkin("red.png");

	map2Walls[19] = wallMesh->CreateModel(-250, 0, -275);
	map2Walls[19]->ScaleZ(6);
	map2Walls[19]->ScaleY(5);
	map2Walls[19]->RotateLocalY(90);
	map2Walls[19]->SetSkin("red.png");

	map2Walls[20] = wallMesh->CreateModel(-310, 0, -275);
	map2Walls[20]->ScaleZ(6);
	map2Walls[20]->ScaleY(5);
	map2Walls[20]->RotateLocalY(90);
	map2Walls[20]->SetSkin("red.png");

	map2Walls[21] = wallMesh->CreateModel(-350, 0, -325);
	map2Walls[21]->ScaleZ(6);
	map2Walls[21]->ScaleY(5);
	map2Walls[21]->RotateLocalY(90);
	map2Walls[21]->SetSkin("red.png");

	map2Walls[22] = wallMesh->CreateModel(-410, 0, -325);
	map2Walls[22]->ScaleZ(6);
	map2Walls[22]->ScaleY(5);
	map2Walls[22]->RotateLocalY(90);
	map2Walls[22]->SetSkin("red.png");

	map2Walls[23] = wallMesh->CreateModel(-370, 0, -275);
	map2Walls[23]->ScaleZ(6);
	map2Walls[23]->ScaleY(5);
	map2Walls[23]->RotateLocalY(90);
	map2Walls[23]->SetSkin("red.png");

	map2Walls[24] = wallMesh->CreateModel(-395, 0, -240);
	map2Walls[24]->ScaleZ(6);
	map2Walls[24]->ScaleY(5);
	map2Walls[24]->SetSkin("red.png");

	map2Walls[25] = wallMesh->CreateModel(-445, 0, -300);
	map2Walls[25]->ScaleZ(6);
	map2Walls[25]->ScaleY(5);
	map2Walls[25]->SetSkin("red.png");

	map2Walls[26] = wallMesh->CreateModel(-470, 0, -265);
	map2Walls[26]->ScaleZ(6);
	map2Walls[26]->ScaleY(5);
	map2Walls[26]->RotateLocalY(90);
	map2Walls[26]->SetSkin("red.png");

	map2Walls[27] = wallMesh->CreateModel(-430, 0, -215);
	map2Walls[27]->ScaleZ(6);
	map2Walls[27]->ScaleY(5);
	map2Walls[27]->RotateLocalY(90);
	map2Walls[27]->SetSkin("red.png");

	map2Walls[28] = wallMesh->CreateModel(-490, 0, -215);
	map2Walls[28]->ScaleZ(6);
	map2Walls[28]->ScaleY(5);
	map2Walls[28]->RotateLocalY(90);
	map2Walls[28]->SetSkin("red.png");

	map2Walls[29] = wallMesh->CreateModel(-530, 0, -265);
	map2Walls[29]->ScaleZ(6);
	map2Walls[29]->ScaleY(5);
	map2Walls[29]->RotateLocalY(90);
	map2Walls[29]->SetSkin("red.png");

	map2Walls[30] = wallMesh->CreateModel(-515, 0, -180);
	map2Walls[30]->ScaleZ(6);
	map2Walls[30]->ScaleY(5);
	map2Walls[30]->SetSkin("red.png");

	map2Walls[31] = wallMesh->CreateModel(-565, 0, -240);
	map2Walls[31]->ScaleZ(6);
	map2Walls[31]->ScaleY(5);
	map2Walls[31]->SetSkin("red.png");

	map2Walls[32] = wallMesh->CreateModel(-515, 0, -120);
	map2Walls[32]->ScaleZ(6);
	map2Walls[32]->ScaleY(5);
	map2Walls[32]->SetSkin("red.png");

	map2Walls[33] = wallMesh->CreateModel(-565, 0, -180);
	map2Walls[33]->ScaleZ(6);
	map2Walls[33]->ScaleY(5);
	map2Walls[33]->SetSkin("red.png");

	map2Walls[34] = wallMesh->CreateModel(-565, 0, -120);
	map2Walls[34]->ScaleZ(6);
	map2Walls[34]->ScaleY(5);
	map2Walls[34]->SetSkin("red.png");

	map2Walls[35] = wallMesh->CreateModel(-565, 0, -120);
	map2Walls[35]->ScaleZ(6);
	map2Walls[35]->ScaleY(5);
	map2Walls[35]->SetSkin("red.png");

	map2Walls[36] = wallMesh->CreateModel(-540, 0, -85);
	map2Walls[36]->ScaleZ(6);
	map2Walls[36]->ScaleY(5);
	map2Walls[36]->RotateLocalY(90);
	map2Walls[36]->SetSkin("blue.png");
}

void Game::map3()
{
	map3Walls[0] = wallMesh->CreateModel(500, 0, 0);
	map3Walls[0]->ScaleZ(6);
	map3Walls[0]->ScaleY(5);
	map3Walls[0]->RotateLocalY(90);
	map3Walls[0]->SetSkin("red.png");

	map3Walls[1] = wallMesh->CreateModel(475, 0, 35);
	map3Walls[1]->ScaleZ(6);
	map3Walls[1]->ScaleY(5);
	map3Walls[1]->SetSkin("red.png");

	map3Walls[2] = wallMesh->CreateModel(525, 0, 35);
	map3Walls[2]->ScaleZ(6);
	map3Walls[2]->ScaleY(5);
	map3Walls[2]->SetSkin("red.png");

	map3Walls[3] = wallMesh->CreateModel(475, 0, 95);
	map3Walls[3]->ScaleZ(6);
	map3Walls[3]->ScaleY(5);
	map3Walls[3]->SetSkin("red.png");

	map3Walls[4] = wallMesh->CreateModel(550, 0, 70);
	map3Walls[4]->ScaleZ(6);
	map3Walls[4]->ScaleY(5);
	map3Walls[4]->RotateLocalY(90);
	map3Walls[4]->SetSkin("red.png");

	map3Walls[5] = wallMesh->CreateModel(510, 0, 120);
	map3Walls[5]->ScaleZ(6);
	map3Walls[5]->ScaleY(5);
	map3Walls[5]->RotateLocalY(90);
	map3Walls[5]->SetSkin("red.png");

	map3Walls[6] = wallMesh->CreateModel(570, 0, 120);
	map3Walls[6]->ScaleZ(6);
	map3Walls[6]->ScaleY(5);
	map3Walls[6]->RotateLocalY(90);
	map3Walls[6]->SetSkin("red.png");

	map3Walls[7] = wallMesh->CreateModel(630, 0, 120);
	map3Walls[7]->ScaleZ(6);
	map3Walls[7]->ScaleY(5);
	map3Walls[7]->RotateLocalY(90);
	map3Walls[7]->SetSkin("red.png");

	map3Walls[8] = wallMesh->CreateModel(610, 0, 70);
	map3Walls[8]->ScaleZ(6);
	map3Walls[8]->ScaleY(5);
	map3Walls[8]->RotateLocalY(90);
	map3Walls[8]->SetSkin("red.png");

	map3Walls[9] = wallMesh->CreateModel(635, 0, 35);
	map3Walls[9]->ScaleZ(6);
	map3Walls[9]->ScaleY(5);
	map3Walls[9]->SetSkin("red.png");

	map3Walls[10] = wallMesh->CreateModel(635, 0, -25);
	map3Walls[10]->ScaleZ(6);
	map3Walls[10]->ScaleY(5);
	map3Walls[10]->SetSkin("red.png");

	map3Walls[11] = wallMesh->CreateModel(685, 0, -25);
	map3Walls[11]->ScaleZ(6);
	map3Walls[11]->ScaleY(5);
	map3Walls[11]->SetSkin("red.png");

	map3Walls[12] = wallMesh->CreateModel(685, 0, 35);
	map3Walls[12]->ScaleZ(6);
	map3Walls[12]->ScaleY(5);
	map3Walls[12]->SetSkin("red.png");

	map3Walls[13] = wallMesh->CreateModel(710, 0, 70);
	map3Walls[13]->ScaleZ(6);
	map3Walls[13]->ScaleY(5);
	map3Walls[13]->RotateLocalY(90);
	map3Walls[13]->SetSkin("red.png");

	map3Walls[14] = wallMesh->CreateModel(735, 0, 35);
	map3Walls[14]->ScaleZ(6);
	map3Walls[14]->ScaleY(5);
	map3Walls[14]->SetSkin("red.png");

	map3Walls[15] = wallMesh->CreateModel(735, 0, -25);
	map3Walls[15]->ScaleZ(6);
	map3Walls[15]->ScaleY(5);
	map3Walls[15]->SetSkin("red.png");

	map3Walls[16] = wallMesh->CreateModel(690, 0, 120);
	map3Walls[16]->ScaleZ(6);
	map3Walls[16]->ScaleY(5);
	map3Walls[16]->RotateLocalY(90);
	map3Walls[16]->SetSkin("red.png");

	map3Walls[17] = wallMesh->CreateModel(750, 0, 120);
	map3Walls[17]->ScaleZ(6);
	map3Walls[17]->ScaleY(5);
	map3Walls[17]->RotateLocalY(90);
	map3Walls[17]->SetSkin("red.png");

	map3Walls[18] = wallMesh->CreateModel(785, 0, 95);
	map3Walls[18]->ScaleZ(6);
	map3Walls[18]->ScaleY(5);
	map3Walls[18]->SetSkin("red.png");

	map3Walls[19] = wallMesh->CreateModel(785, 0, 35);
	map3Walls[19]->ScaleZ(6);
	map3Walls[19]->ScaleY(5);
	map3Walls[19]->SetSkin("red.png");

	map3Walls[20] = wallMesh->CreateModel(785, 0, -25);
	map3Walls[20]->ScaleZ(6);
	map3Walls[20]->ScaleY(5);
	map3Walls[20]->SetSkin("red.png");

	map3Walls[21] = wallMesh->CreateModel(785, 0, -85);
	map3Walls[21]->ScaleZ(6);
	map3Walls[21]->ScaleY(5);
	map3Walls[21]->SetSkin("red.png");

	map3Walls[22] = wallMesh->CreateModel(710, 0, -60);
	map3Walls[22]->ScaleZ(6);
	map3Walls[22]->ScaleY(5);
	map3Walls[22]->RotateLocalY(90);
	map3Walls[22]->SetSkin("red.png");

	map3Walls[23] = wallMesh->CreateModel(750, 0, -110);
	map3Walls[23]->ScaleZ(6);
	map3Walls[23]->ScaleY(5);
	map3Walls[23]->RotateLocalY(90);
	map3Walls[23]->SetSkin("red.png");

	map3Walls[24] = wallMesh->CreateModel(690, 0, -110);
	map3Walls[24]->ScaleZ(6);
	map3Walls[24]->ScaleY(5);
	map3Walls[24]->RotateLocalY(90);
	map3Walls[24]->SetSkin("red.png");

	map3Walls[25] = wallMesh->CreateModel(610, 0, -60);
	map3Walls[25]->ScaleZ(6);
	map3Walls[25]->ScaleY(5);
	map3Walls[25]->RotateLocalY(90);
	map3Walls[25]->SetSkin("red.png");

	map3Walls[26] = wallMesh->CreateModel(550, 0, -60);
	map3Walls[26]->ScaleZ(6);
	map3Walls[26]->ScaleY(5);
	map3Walls[26]->RotateLocalY(90);
	map3Walls[26]->SetSkin("red.png");

	map3Walls[27] = wallMesh->CreateModel(630, 0, -110);
	map3Walls[27]->ScaleZ(6);
	map3Walls[27]->ScaleY(5);
	map3Walls[27]->RotateLocalY(90);
	map3Walls[27]->SetSkin("red.png");

	map3Walls[28] = wallMesh->CreateModel(570, 0, -110);
	map3Walls[28]->ScaleZ(6);
	map3Walls[28]->ScaleY(5);
	map3Walls[28]->RotateLocalY(90);
	map3Walls[28]->SetSkin("red.png");

	map3Walls[29] = wallMesh->CreateModel(490, 0, -60);
	map3Walls[29]->ScaleZ(6);
	map3Walls[29]->ScaleY(5);
	map3Walls[29]->RotateLocalY(90);
	map3Walls[29]->SetSkin("red.png");

	map3Walls[30] = wallMesh->CreateModel(465, 0, -95);
	map3Walls[30]->ScaleZ(6);
	map3Walls[30]->ScaleY(5);
	map3Walls[30]->SetSkin("red.png");

	map3Walls[31] = wallMesh->CreateModel(550, 0, -110);
	map3Walls[31]->ScaleZ(6);
	map3Walls[31]->ScaleY(5);
	map3Walls[31]->RotateLocalY(90);
	map3Walls[31]->SetSkin("red.png");

	map3Walls[32] = wallMesh->CreateModel(525, 0, -145);
	map3Walls[32]->ScaleZ(6);
	map3Walls[32]->ScaleY(5);
	map3Walls[32]->SetSkin("red.png");

	map3Walls[33] = wallMesh->CreateModel(525, 0, -205);
	map3Walls[33]->ScaleZ(6);
	map3Walls[33]->ScaleY(5);
	map3Walls[33]->SetSkin("red.png");

	map3Walls[34] = wallMesh->CreateModel(560, 0, -230);
	map3Walls[34]->ScaleZ(6);
	map3Walls[34]->ScaleY(5);
	map3Walls[34]->RotateLocalY(90);
	map3Walls[34]->SetSkin("red.png");

	map3Walls[35] = wallMesh->CreateModel(465, 0, -155);
	map3Walls[35]->ScaleZ(6);
	map3Walls[35]->ScaleY(5);
	map3Walls[35]->SetSkin("red.png");

	map3Walls[36] = wallMesh->CreateModel(465, 0, -210);
	map3Walls[36]->ScaleZ(5);
	map3Walls[36]->ScaleY(5);
	map3Walls[36]->SetSkin("red.png");

	map3Walls[37] = wallMesh->CreateModel(430, 0, -230);
	map3Walls[37]->ScaleZ(6);
	map3Walls[37]->ScaleY(5);
	map3Walls[37]->RotateLocalY(90);
	map3Walls[37]->SetSkin("red.png");

	map3Walls[38] = wallMesh->CreateModel(405, 0, -260);
	map3Walls[38]->ScaleZ(5);
	map3Walls[38]->ScaleY(5);
	map3Walls[38]->SetSkin("red.png");

	map3Walls[39] = wallMesh->CreateModel(585, 0, -260);
	map3Walls[39]->ScaleZ(5);
	map3Walls[39]->ScaleY(5);
	map3Walls[39]->SetSkin("red.png");

	map3Walls[40] = wallMesh->CreateModel(430, 0, -290);
	map3Walls[40]->ScaleZ(6);
	map3Walls[40]->ScaleY(5);
	map3Walls[40]->RotateLocalY(90);
	map3Walls[40]->SetSkin("red.png");

	map3Walls[41] = wallMesh->CreateModel(490, 0, -290);
	map3Walls[41]->ScaleZ(6);
	map3Walls[41]->ScaleY(5);
	map3Walls[41]->RotateLocalY(90);
	map3Walls[41]->SetSkin("red.png");

	map3Walls[42] = wallMesh->CreateModel(555, 0, -290);
	map3Walls[42]->ScaleZ(7);
	map3Walls[42]->ScaleY(5);
	map3Walls[42]->RotateLocalY(90);
	map3Walls[42]->SetSkin("red.png");
}
//
//bool Game::guardDeath()
//{
//	/*if (!guardDeathBuffer.loadFromFile("guarddeathsound.ogg"))
//	{
//		return false;
//	}
//
//	guardDeathSound.setBuffer(guardDeathBuffer);
//	guardDeathSound.play();
//	guardDeathSound.setLoop(false);*/
//}

void Game::mainLoop()
{
	enum ECState { MENU, INGAME,CONTROLS, QUIT, GAMEOVER, WIN}; // enum states to determine what mode the player is currently in
	ECState gameState = MENU;

	IFont* myFont = myEngine->LoadFont("Charlemagne std", 36);
	IFont* backFont = myEngine->LoadFont("Charlemagne std", 38);
	IFont* controlsFont = myEngine->LoadFont("Charlemagne std", 20);

	while (myEngine->IsRunning())
	{
		if (gameState == MENU)
		{
			myCamera->SetPosition(0, -20, 1000);
			
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//GAME MENU

			backFont->Draw("PROJECT: RECONNAISSANCE", 650, 240, kWhite, kCentre, kVCentre);
			myFont->Draw("Press P to play", 650, 350, kBlue, kCentre, kVCentre);
			myFont->Draw("Press Q to quit", 650, 410, kBlue, kCentre, kVCentre);
			myFont->Draw("Press I for Controls", 650, 470, kBlue, kCentre, kVCentre);

			if (myEngine->KeyHit(PauseGame))
			{
				myCamera->SetPosition(0, 30, 0);
				myCamera->AttachToParent(thief);
				gameState = INGAME;
			}
			
			if (myEngine->KeyHit(ControlsPage))
			{
				gameState = CONTROLS;
			}

			if (myEngine->KeyHit(QuitGame))
			{
				myEngine->Stop();
			}
		}

		if (gameState == WIN)
		{
			myCamera->SetPosition(0, -20, 1000);
			myFont->Draw("Congratulations! You have successfully disarmed the bio weapon", 670, 240, kWhite, kCentre, kVCentre);
			controlsFont->Draw("The world is now a safer place", 690, 350, kWhite, kCentre, kVCentre);
			controlsFont->Draw("Press Q to Exit", 665, 500, kWhite, kCentre, kVCentre);

			if (myEngine->KeyHit(Key_Q))
			{
				myEngine->Stop();
			}
		}

		if (gameState == CONTROLS)
		{
			myFont->Draw("Controls", 650, 240, kWhite, kCentre, kVCentre);

			controlsFont->Draw("Q = Kill Guard", 650, 350, kBlue, kCentre, kVCentre);
			controlsFont->Draw("Up Arrow Key = Move Forwards", 650, 410, kBlue, kCentre, kVCentre);
			controlsFont->Draw("Down Arrow Key = Move Backwards", 650, 470, kBlue, kCentre, kVCentre);
			controlsFont->Draw("Left Arrow Key = Turn Left", 650, 530, kBlue, kCentre, kVCentre);
			controlsFont->Draw("Right Arrow Key = Turn Right", 650, 590, kBlue, kCentre, kVCentre);
			controlsFont->Draw("H = Hide in cupboard", 650, 650, kBlue, kCentre, kVCentre);

			controlsFont->Draw("Press B to Return to Menu", 300, 590, kBlue, kCentre, kVCentre);

			if (myEngine->KeyHit(Return))
			{
				gameState = MENU;
			}
		}

		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/

		if (gameState == INGAME)
		{
			float updateTime = frame / fps; // UPDATIME TIME FOR GAME, USED FOR TIMING STATES ETC
			float frameTime = frame / updateTime;
			frameTime += updateTime;

			std::clock_t start;
			double duration;

			start = std::clock();
			int time = CLOCKS_PER_SEC;
			time = (std::clock() + start) / (int)CLOCKS_PER_SEC;

			
			/*std::cout << "printf: " << duration << '\n';*/

			
			stringstream outText;
			outText << "White Position: " << time;
			myFont->Draw(outText.str(), 20, 20);
			outText.str(""); // Clear myStream

				
			if (myEngine->KeyHit(ReturnToMenu))
			{
				myCamera->DetachFromParent();
				gameState = MENU;
			}

			for (int i = 0; i < NUM_GUARDS && !guardCollision && level1 == true; i++) // Array that applies collisions to level 1 guards 
			{
				const int guardsMatrix = 16;
				
				float matrix[guardsMatrix];
				
				float x;
				float y;
				float z;

				lvl1Guards[i]->GetMatrix(matrix);

				x = matrix[guardXMatrix];
				y = matrix[guardYMatrix];
				z = matrix[guardZMatrix];

				float xx, yy, zz;

				xx = thief->GetX() - lvl1Guards[i]->GetX();
				yy = thief->GetY() - lvl1Guards[i]->GetY();
				zz = thief->GetZ() - lvl1Guards[i]->GetZ();

				float collisionDist = sqrt(xx*xx + yy*yy + zz*zz);
				float dotProduct = x * xx + y * yy + z * zz;
				float normalise;

				float guardXdistance = lvl1Guards[i]->GetX() - thief->GetX();
				float guardZdistance = lvl1Guards[i]->GetZ() - thief->GetZ();

				float guardDistance = sqrtf(guardXdistance * guardXdistance + guardZdistance * guardZdistance);

				if (dotProduct > playerSpotted && guardDistance < guardVision)
				{
					thief->SetLocalPosition(lvl1XSpawn, lvl1YSpawn, lvl1ZSpawn);
				}
				if (dotProduct < playerSpotted && guardDistance < guardVision && myEngine->KeyHit(KillGuard))
				{
					lvl1Guards[i]->SetLocalPosition(guardDeathX, guardDeathY, guardDeathZ);
				}

			}

			for (int i = 0; i < NUM_GUARDS2 && !guardCollision && level2 == true; i++) // Array that applies collisions to level 2 guards 
			{
				const int guardsMatrix = 16;

				float matrix[guardsMatrix];

				float x;
				float y;
				float z;

				lvl2Guards[i]->GetMatrix(matrix);

				x = matrix[guardXMatrix];
				y = matrix[guardYMatrix];
				z = matrix[guardZMatrix];

				float xx, yy, zz;

				xx = thief->GetX() - lvl2Guards[i]->GetX();
				yy = thief->GetY() - lvl2Guards[i]->GetY();
				zz = thief->GetZ() - lvl2Guards[i]->GetZ();

				float collisionDist = sqrt(xx*xx + yy*yy + zz*zz);
				float dotProduct = x * xx + y * yy + z * zz;
				float normalise;

				float guardXdistance = lvl2Guards[i]->GetX() - thief->GetX();
				float guardZdistance = lvl2Guards[i]->GetZ() - thief->GetZ();

				float guardDistance = sqrtf(guardXdistance * guardXdistance + guardZdistance * guardZdistance);

				if (dotProduct > playerSpotted && guardDistance < guardVision && !playerHidden)
				{
					thief->SetLocalPosition(lvl2XSpawn, lvl2YSpawn, lvl2ZSpawn);
					thief->ResetOrientation();
				}

				if (dotProduct < playerSpotted && guardDistance < guardVision && myEngine->KeyHit(KillGuard))
				{
					lvl2Guards[i]->SetLocalPosition(guardDeathX, guardDeathY, guardDeathZ);
				}
			}


			for (int i = 0; i < NUM_GUARDS3 && !guardCollision && level3 == true; i++) // Array that applies collisions to level 2 guards 
			{
				const int guardsMatrix = 16;

				float matrix[guardsMatrix];

				float x;
				float y;
				float z;

				lvl3Guards[i]->GetMatrix(matrix);

				x = matrix[guardXMatrix];
				y = matrix[guardYMatrix];
				z = matrix[guardZMatrix];

				float xx, yy, zz;

				xx = thief->GetX() - lvl3Guards[i]->GetX();
				yy = thief->GetY() - lvl3Guards[i]->GetY();
				zz = thief->GetZ() - lvl3Guards[i]->GetZ();

				float collisionDist = sqrt(xx*xx + yy*yy + zz*zz);
				float dotProduct = x * xx + y * yy + z * zz;
				float normalise;

				float guardXdistance = lvl3Guards[i]->GetX() - thief->GetX();
				float guardZdistance = lvl3Guards[i]->GetZ() - thief->GetZ();

				float guardDistance = sqrtf(guardXdistance * guardXdistance + guardZdistance * guardZdistance);

				if (dotProduct > playerSpotted && guardDistance < guardVision)
				{
					thief->SetLocalPosition(lvl3XSpawn, lvl3YSpawn, lvl3ZSpawn);
					thief->ResetOrientation();
				}

				if (dotProduct < playerSpotted && guardDistance < guardVision && myEngine->KeyHit(KillGuard))
				{
					lvl3Guards[i]->SetLocalPosition(guardDeathX, guardDeathY, guardDeathZ);
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//TURN GUARDS

			if (!guard2Killed)
			{
				if (guard2Turn == false)
				{
					guard2TurnTimer -= updateTime;
				}
				if (guard2Turn == true)
				{
					guard2TurnTimer += updateTime;
				}
				if (guard2TurnTimer < 0 && guard2Turn == false)
				{
					guard2Turn = true;
					lvl1Guards[1]->RotateY(180);
				}
				if (guard2TurnTimer > 100 && guard2Turn == true)
				{
					guard2Turn = false;
					lvl1Guards[1]->RotateY(180);
				}
			}

			if (!guard2Killed)
			{
				if (lvl3guardTurn == false)
				{
					guard3TurnTimer -= updateTime;
				}
				if (lvl3guardTurn == true)
				{
					guard3TurnTimer += updateTime;
				}
				if (guard3TurnTimer < 0 && lvl3guardTurn == false)
				{
					lvl3guardTurn = true;
					lvl3Guards[0]->RotateY(180);
				}
				if (guard3TurnTimer > 70 && lvl3guardTurn == true)
				{
					lvl3guardTurn = false;
					lvl3Guards[0]->RotateY(180);
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//INITIALISE MOVING GUARDS

			lvl2Guards[0]->MoveLocalZ(guardSpeed  * GAME_SPEED); 
			lvl2Guards[1]->MoveLocalZ(guardSpeed  * GAME_SPEED);
			lvl2Guards[2]->MoveLocalZ(fastGuardSpeed  * GAME_SPEED);
			
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//GUARD WAYPOINTS

			int currentWaypoint = 0;
			int nextWaypoint = 1;
			int nextGuardWaypnts = 2;
			int maxWaypoints = 4;

			for (int i = 0; i < NUM_WAYPOINTS && !guardCollision && currentWaypoint <= maxWaypoints; i++)
			{
				float waypointX = waypoints[currentWaypoint]->GetX() - lvl2Guards[i]->GetX();
				float waypointZ = waypoints[currentWaypoint]->GetZ() - lvl2Guards[i]->GetZ();
				
				float distance = sqrtf(waypointX * waypointX + waypointZ * waypointZ);

				float waypoint1X = waypoints[nextWaypoint]->GetX() - lvl2Guards[i]->GetX();
				float waypoint1Z = waypoints[nextWaypoint]->GetZ() - lvl2Guards[i]->GetZ();
				float distance1Y = noYAxis;

				float distance1 = sqrtf(waypoint1X * waypoint1X + waypoint1Z * waypoint1Z);

				float eventBoxX = eventBox[0]->GetX() - thief->GetX();
				float eventBoxZ = eventBox[0]->GetZ() - thief->GetZ();

				float eventBoxDist = sqrtf(eventBoxX * eventBoxX + eventBoxZ * eventBoxZ);

				if (eventBoxDist < 10)
				{
					fastGuardSpeed = 0.04f;
				}

				if (distance1 < waypointDist)
				{
					lvl2Guards[i]->RotateLocalY(guardTurn);
				}
				
				if (distance < waypointDist)
				{
					if (currentWaypoint == maxWaypoints)
					{
						lvl2Guards[i]->ResetOrientation();
						lvl2Guards[i]->RotateLocalY(-90);
					}
					
					lvl2Guards[i]->RotateLocalY(guardTurn);
				}

				currentWaypoint += nextGuardWaypnts;
				nextWaypoint += nextGuardWaypnts;
			}

			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//CUPBOARD COLLISIONS

			bool exitCupboard = false;
			for (int i = 0; i < NUM_CUPBOARDS; i++)
			{
				float cupboardDistanceX = cupboards[i]->GetX() - thief->GetX();
				float cupboardDistanceZ = cupboards[i]->GetZ() - thief->GetZ();
				
				float cupboardDistance = sqrt(cupboardDistanceX* cupboardDistanceX + cupboardDistanceZ * cupboardDistanceZ);

				if (cupboardDistance < cupboardEnter && myEngine->KeyHit(hideInCupboard))
				{
					if (playerHidden == false && exitCupboard == false)
					{
						playerHidden = true;
						thief->SetLocalPosition(-250, 0, -325);
						thief->ResetOrientation();
					}

					else if (playerHidden == true)
					{
						exitCupboard = true;
						playerHidden = false;
						thief->SetLocalPosition(-250, 0, -300);
					}
				}
			}

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// MOVE TO LEVEL 2
			
			float lvl2DoorX = map1Walls[18]->GetX() - thief->GetX();
			float lvl2DoorZ = map1Walls[18]->GetZ() - thief->GetZ();
			
			float DoorDist = 18.0f;

			float lvl2DoorDist = sqrt(lvl2DoorX* lvl2DoorX + lvl2DoorZ * lvl2DoorZ);

			if (lvl2DoorDist < DoorDist)
			{
				level1 = false;
				level2 = true;
				thief->ResetOrientation();
				thief->SetPosition(0, 0, -550);
			}

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// MOVE TO LEVEL 3
			float lvl3DoorX = map2Walls[36]->GetX() - thief->GetX();
			float lvl3DoorZ = map2Walls[36]->GetZ() - thief->GetZ();

			float DoorDist3 = 18.0f;

			float lvl3DoorDist = sqrt(lvl3DoorX* lvl3DoorX + lvl3DoorZ * lvl3DoorZ);

			if (lvl3DoorDist < DoorDist3)
			{
				level2 = false;
				level3 = true;
				thief->ResetOrientation();
				thief->SetPosition(lvl3XSpawn, lvl3YSpawn, lvl3ZSpawn);
			}
			
			handleInputs();

			//WIN GAME
			float winBoxRadius = 20;
			float winGameX = winbox->GetX() - thief->GetX();
			float winGameZ = winbox->GetZ() - thief->GetZ();

			float winGameDist = sqrtf(winGameX * winGameX + winGameZ * winGameZ);

			if (winGameDist < winBoxRadius)
			{
				gameState = WIN;
			}

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// WALL COLLISIONS

			const float wallLength = 50;
			const float wallHalfLength = wallLength / 2;
			const float wallWidth = 4.0f;

			//Wall Values
			float wallMinX;
			float wallMaxX;
			float wallMinZ;
			float wallMaxZ;

			//Thief Values
			float thiefMinX;
			float thiefMaxX;
			float thiefMinZ;
			float thiefMaxZ;

			const float thiefRadius = 3.0f;

			thiefMinX = (thief->GetLocalX() - thiefRadius - wallWidth);
			thiefMaxX = (thief->GetLocalX() + thiefRadius + wallWidth);
			thiefMinZ = (thief->GetLocalZ() - thiefRadius - wallWidth);
			thiefMaxZ = (thief->GetLocalZ() + thiefRadius + wallWidth);

			bool mWallCollision = false;

			if (level1 == true) 
			{
				for (int i = 0; i < MAP1_NUM_WALLS && !mWallCollision; i++)
				{
					const int wallMatrixSize = 16;
					float wallMatrix[wallMatrixSize];
					
					map1Walls[i]->GetMatrix(wallMatrix);
					bool wallNotRotated = abs(wallMatrix[rotatedWall]) > abs(wallMatrix[notRotatedWall]);

					if (wallNotRotated)
					{
						wallMinX = (map1Walls[i]->GetLocalX() - wallXCollision - wallWidth - thiefRadius);
						wallMaxX = (map1Walls[i]->GetLocalX() + wallXCollision + wallWidth + thiefRadius);
						wallMinZ = (map1Walls[i]->GetLocalZ() - wallMinZCollision - wallHalfLength - thiefRadius);
						wallMaxZ = (map1Walls[i]->GetLocalZ() + wallMaxZCollision + wallHalfLength + thiefRadius);
					}
					else
					{
						wallMinX = (map1Walls[i]->GetLocalX() - rotWallXCollision - wallHalfLength - thiefRadius); // ROUGH COLLISION FOR LARGE SIZED WALL
						wallMaxX = (map1Walls[i]->GetLocalX() + rotWallXCollision + wallHalfLength + thiefRadius);
						wallMinZ = (map1Walls[i]->GetLocalZ() - wallWidth - thiefRadius);
						wallMaxZ = (map1Walls[i]->GetLocalZ() + rotWallZCollision + wallWidth + thiefRadius);
					}

					if (thiefMinX + thiefRadius > wallMinX && thief->GetLocalX()
						+ thiefRadius < wallMaxX && thief->GetLocalZ() + thiefRadius > wallMinZ
						&& thief->GetLocalZ() + thiefRadius < wallMaxZ && myEngine->KeyHeld(ThiefForward))
					{
						thiefForwardSpeed = thiefStill;
						thiefBackwardSpeed = -thiefMove;
						mWallCollision = true;
					}
					else
					{
						thiefForwardSpeed = thiefMove;
					}

					if (thiefMinX + thiefRadius > wallMinX && thief->GetLocalX()
						+ thiefRadius < wallMaxX && thief->GetLocalZ() + thiefRadius > wallMinZ
						&& thief->GetLocalZ() + thiefRadius < wallMaxZ && myEngine->KeyHeld(ThiefBackward))
					{
						thiefForwardSpeed = thiefMove;
						thiefBackwardSpeed = thiefStill;
						mWallCollision = true;
					}
					else
					{
						thiefBackwardSpeed = -thiefMove;
					}
				}
			}

			if (level3 == true)
			{
				for (int i = 0; i < MAP3_NUM_WALLS && !mWallCollision; i++)
				{
					const int wallMatrixSize = 16;
					float wallMatrix[wallMatrixSize];

					map3Walls[i]->GetMatrix(wallMatrix);
					bool wallNotRotated = abs(wallMatrix[rotatedWall]) > abs(wallMatrix[notRotatedWall]);

					if (wallNotRotated)
					{
						wallMinX = (map3Walls[i]->GetLocalX() - wallXCollision - wallWidth - thiefRadius);
						wallMaxX = (map3Walls[i]->GetLocalX() + wallXCollision + wallWidth + thiefRadius);
						wallMinZ = (map3Walls[i]->GetLocalZ() - wallMinZCollision - wallHalfLength - thiefRadius);
						wallMaxZ = (map3Walls[i]->GetLocalZ() + wallMaxZCollision + wallHalfLength + thiefRadius);
					}
					else
					{
						wallMinX = (map3Walls[i]->GetLocalX() - rotWallXCollision - wallHalfLength - thiefRadius); // ROUGH COLLISION FOR LARGE SIZED WALL
						wallMaxX = (map3Walls[i]->GetLocalX() + rotWallXCollision + wallHalfLength + thiefRadius);
						wallMinZ = (map3Walls[i]->GetLocalZ() - wallWidth - thiefRadius);
						wallMaxZ = (map3Walls[i]->GetLocalZ() + rotWallZCollision + wallWidth + thiefRadius);
					}

					if (thiefMinX + thiefRadius > wallMinX && thief->GetLocalX()
						+ thiefRadius < wallMaxX && thief->GetLocalZ() + thiefRadius > wallMinZ
						&& thief->GetLocalZ() + thiefRadius < wallMaxZ && myEngine->KeyHeld(ThiefForward))
					{
						thiefForwardSpeed = thiefStill;
						thiefBackwardSpeed = -thiefMove;
						mWallCollision = true;
					}
					else
					{
						thiefForwardSpeed = thiefMove;
					}

					if (thiefMinX + thiefRadius > wallMinX && thief->GetLocalX()
						+ thiefRadius < wallMaxX && thief->GetLocalZ() + thiefRadius > wallMinZ
						&& thief->GetLocalZ() + thiefRadius < wallMaxZ && myEngine->KeyHeld(ThiefBackward))
					{
						thiefForwardSpeed = thiefMove;
						thiefBackwardSpeed = thiefStill;
						mWallCollision = true;
					}
					else
					{
						thiefBackwardSpeed = -thiefMove;
					}
				}
			}

			if (level2 == true)
			{
				for (int i = 0; i < MAP2_NUM_WALLS && !mWallCollision; i++)
				{
					const int matrix = 16;
					float wallMatrix[matrix];

					map2Walls[i]->GetMatrix(wallMatrix);
					bool wallNotRotated = abs(wallMatrix[0]) > abs(wallMatrix[2]);

					if (wallNotRotated)
					{
						wallMinX = (map2Walls[i]->GetLocalX() - wallXCollision - wallWidth - thiefRadius);
						wallMaxX = (map2Walls[i]->GetLocalX() + wallXCollision + wallWidth + thiefRadius);
						wallMinZ = (map2Walls[i]->GetLocalZ() - wallMinZCollision - wallHalfLength - thiefRadius);
						wallMaxZ = (map2Walls[i]->GetLocalZ() + wallMaxZCollision + wallHalfLength + thiefRadius);
					}
					else
					{
						wallMinX = (map2Walls[i]->GetLocalX() - rotWallXCollision - wallHalfLength - thiefRadius); // ROUGH COLLISION FOR LARGE SIZED WALL
						wallMaxX = (map2Walls[i]->GetLocalX() + rotWallXCollision + wallHalfLength + thiefRadius);
						wallMinZ = (map2Walls[i]->GetLocalZ() - wallWidth - thiefRadius);
						wallMaxZ = (map2Walls[i]->GetLocalZ() + rotWallZCollision + wallWidth + thiefRadius);
					}

					if (thiefMinX + thiefRadius > wallMinX && thief->GetLocalX()
						+ thiefRadius < wallMaxX && thief->GetLocalZ() + thiefRadius > wallMinZ
						&& thief->GetLocalZ() + thiefRadius < wallMaxZ && myEngine->KeyHeld(ThiefForward))
					{
						thiefForwardSpeed = thiefStill;
						thiefBackwardSpeed = -thiefMove;
						mWallCollision = true;
					}
					else
					{
						thiefForwardSpeed = thiefMove;
					}

					if (thiefMinX + thiefRadius > wallMinX && thief->GetLocalX()
						+ thiefRadius < wallMaxX && thief->GetLocalZ() + thiefRadius > wallMinZ
						&& thief->GetLocalZ() + thiefRadius < wallMaxZ && myEngine->KeyHeld(ThiefBackward))
					{
						thiefForwardSpeed = thiefMove;
						thiefBackwardSpeed = thiefStill;
						mWallCollision = true;
					}
					else
					{
						thiefBackwardSpeed = -thiefMove;
					}


				}
			}
		}
	}
}
	
void Game::handleInputs()
{
	if (!playerHidden)
	{
		if (myEngine->KeyHeld(TurnLeft))
		{
			thief->RotateY(-thiefRotateSpeed * thiefMove);
		}

		if (myEngine->KeyHeld(TurnRight))
		{
			thief->RotateY(thiefRotateSpeed * thiefMove);
		}

		if (myEngine->KeyHeld(ThiefForward))
		{
			thief->MoveLocalZ(thiefForwardSpeed * GAME_SPEED);
		}

		if (myEngine->KeyHeld(ThiefBackward))
		{
			thief->MoveLocalZ(thiefBackwardSpeed * GAME_SPEED);
		}
	}
}

void Game::run()
{
	createEngine();
	initializeStats();
	initializeAssets();
	map1();
	map2();
	map3();
	mainLoop();
}
