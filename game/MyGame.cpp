#include "stdafx.h"
#include "MyGame.h"

#pragma warning (disable: 4244)


float Coords[][2] = {
	{ 40, 160 }, //0
	{ 40, 340 }, //1
	{ 40, 530 }, //2
	{ 480, 530 }, //3
	{ 480, 680 }, //4
	{ 480, 340 }, //5
	{ 480, 160 }, //6
	{ 955, 160 }, //7
	{ 955, 340 }, //8
	{ 955, 530 }, //9
	{ 1420, 530 }, //10
	{ 1420, 340 }, //11
	{ 1420, 160 }, //12
	{ 2550, 760 }, //13
	{ 2700, 760 }, //14
	{ 2740, 552 }, //15
	{ 2700, 330 }, //16
	{ 2550, 330 }, //17
	{ 3000, 330 }, //18
	{ 3100, 590 }, //19
	{ 2940, 670 }, //20
	{ 3250, 200 }, //21
	{ 3490, 300 }, //22
	{ 3490, 400 }, //23
	{ 3250, 600 }, //24
	{ 3450, 750 }, //25
	{ 3270, 900 }, //26
	{ 3470, 1090 }, //27
	{ 3270, 1200 }, //28
	{ 3270, 1500 }, //29
	{ 3470, 1600 }, //30
	{ 3440, 1770 }, //31
	{ 3340, 1870 }, //32
	{ 3210, 1730 }, //33
};

int Connections[][2] = {
	{ 0, 1 },
	{ 0, 6 },
	{ 1, 2 },
	{ 1, 5 },
	{ 2, 3 },
	{ 3, 4 },
	{ 3, 5 },
	{ 3, 9 },
	{ 5, 6 },
	{ 5, 8 },
	{ 6, 7 },
	{ 7, 8 },
	{ 7, 12},
	{ 8, 9 },
	{ 8, 11},
	{ 9, 10},
	{10, 11},
	{11, 12},
	{13, 14},
	{14, 15},
	{15, 16},
	{16, 17},
	{18, 19},
	{19, 20},
	{21, 22},
	{21, 24},
	{22, 23},
	{23, 24},
	{24, 25},
	{25, 26},
	{26, 27},
	{27, 28},
	{28, 29},
	{28, 30},
	{29, 30},
	{29, 33},
	{30, 31},
	{31, 32},
	{32, 33},

};



/////////////////////////////////////////////////////
// Dijkstra algorithm implementation

bool PathFind(vector<NODE>& graph, int nStart, int nGoal, vector<int>& path)
{
	list<int> open;

	// mark all nodes in the graph as unvisited
	for (unsigned i = 0; i < graph.size(); i++)
		graph[i].open = false;

	// open the Start node
	graph[nStart].costSoFar = 0;
	graph[nStart].nConnection = -1;
	graph[nStart].open = true;
	open.push_back(nStart);

	while (open.size() > 0)
	{
		// Find the element with the smallest costSoFar
		// iMin is the iterator (pointer) to its position in the opn list
		list<int>::iterator iCurrent = min_element(open.begin(), open.end(), [graph](int i, int j) -> bool {
			return graph[i].costSoFar < graph[j].costSoFar;
			});
		int curNode = *iCurrent;
		float coastSoFar = graph[curNode].costSoFar;

		// If the end node found, then terminate
		if (curNode == nGoal)
			break;

		// Otherwise, visit all the connections
		for (CONNECTION conn : graph[curNode].conlist)
		{
			int endNode = conn.nEnd;
			float newCostSoFar = coastSoFar + conn.cost;

			// for open nodes, ignore if the current route worse then the route already found
			if (graph[endNode].open && graph[endNode].costSoFar <= newCostSoFar)
				continue;

			// Wow, we've found a better route!
			graph[endNode].costSoFar = newCostSoFar;
			graph[endNode].nConnection = curNode;

			// if unvisited yet, add to the open list
			if (!graph[endNode].open)
			{
				graph[endNode].open = true;
				open.push_back(endNode);
			}

			// in Dijkstra, this should never be a closed node
		}

		// We can now close the current graph...
		graph[curNode].closed = true;
		open.erase(iCurrent);
	}

	// Collect the path from the generated graph data
	if (open.size() == 0)
		return false;		// path not found!

	int i = nGoal;
	while (graph[i].nConnection >= 0)
	{
		path.push_back(i);
		i = graph[i].nConnection;
	}
	path.push_back(i);

	reverse(path.begin(), path.end());
	return true;
}

void CMyGame::ZombieControl()
{
	/*CSprite* zomb = new CSprite(40, 530, "Zombie.png", GetTime());
	zomb->SetSpeed(10);
	zomb->SetState(wait);
	zombieList.push_back(zomb);*/



	if (zomb1.GetState() == wait)
	{
		// Random
		int r = rand() % 11;
		zX = Coords[r][0];
		zY = Coords[r][1];

		zomb1.SetState(search);

		//CVector v(zX, zY);	// destination
		CVector v(1420, 530);	// destination


		// find the first node: the closest to the NPC
		vector<NODE>::iterator iFirst =
			min_element(m_graph.begin(), m_graph.end(), [this](NODE& n1, NODE& n2) -> bool {
			return Distance(n1.pos, zomb1.GetPos()) < Distance(n2.pos, zomb1.GetPos());
				});

		// find the last node: the closest to the destination
		vector<NODE>::iterator iLast =
			min_element(m_graph.begin(), m_graph.end(), [v](NODE& n1, NODE& n2) -> bool {
			return Distance(n1.pos, v) < Distance(n2.pos, v);
				});

		int nFirst = iFirst - m_graph.begin();
		int nLast = iLast - m_graph.begin();


		// remove the current way points and reset the NPC
		if (!m_waypoints.empty())
		{
			m_waypoints.clear();
			zomb1.SetState(wait);
		}

		// call the path finding algorithm to complete the waypoints
		vector<int> path;
		if (PathFind(m_graph, nFirst, nLast, path))
		{
			for (int i : path)
				m_waypoints.push_back(m_graph[i].pos);
			m_waypoints.push_back(v);
		}
	}
	else
	{
		return;
	}

}

CMyGame::CMyGame(void) :
	m_npc(544, 96, 64, 64, 0)
	
{
	m_npc.LoadAnimation("Spider64.png", "walk", CSprite::Sheet(4, 2).Col(0).From(0).To(1));
	m_npc.LoadAnimation("Spider64.png", "idle", CSprite::Sheet(4, 2).Col(2).From(0).To(1));
	m_npc.SetAnimation("idle", 4);

	// create graph structure - nodes
	for (float* coord : Coords)
		m_graph.push_back(NODE{ CVector(coord[0], coord[1]) });

	// create graph structure - connections
	for (int* conn : Connections)
	{
		int ind1 = conn[0];
		int ind2 = conn[1];
		NODE& node1 = m_graph[ind1];
		NODE& node2 = m_graph[ind2];
		float dist = Distance(node1.pos, node2.pos);

		node1.conlist.push_back(CONNECTION{ ind2, dist });
		node2.conlist.push_back(CONNECTION{ ind1, dist });
	}
}

CMyGame::~CMyGame(void)
{
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();

	player.Update(t);
	ZombieControl();
	zomb1.Update(t);
	// NPC: follow the waypoints
	if (!m_waypoints.empty())
	{
		// If NPC not moving, start moving to the first waypoint
		if (zomb1.GetSpeed() < 1)
		{
			zomb1.SetSpeed(50);
			zomb1.SetDirection(m_waypoints.front() - zomb1.GetPosition());
			zomb1.SetRotation(zomb1.GetDirection() - 90);
		}

		// Passed the waypoint?
		CVector v = m_waypoints.front() - zomb1.GetPosition();
		if (Dot(zomb1.GetVelocity(), v) < 0)
		{
			// Stop movement
			m_waypoints.pop_front();
			if (m_waypoints.empty())
			zomb1.SetVelocity(0, 0);
			zomb1.SetRotation(0);
			zomb1.SetState(wait);
		}
	}
	
	m_npc.Update(t);
	zomb1.Update(t);
	map.Update(t);
	for (CSprite* zomb : zombieList) zomb->Update(t);

	PlayerControl();


}



void CMyGame::OnDraw(CGraphics* g)
{
	// ----- scrolling -------------------------------
////////// Scrolling before Draw(g); //////////
// game world (background image) is of size 6086 x 2119
//1280, 768
	int leftScreenLimit = 640; // 1080 / 2 = 960
	int rightScreenLimit = 5446; // 2160 - 540 = 1620
	int downScreenLimit = 384; // 1080 / 2 = 540
	int upScreenLimit = 1735; // 2160 - 540 = 1620
	int screenWidth = 6086; // game world Width
	int screenHeight = 2119; // game world Height




	// INFO ->> Side and Up scrolling
	// we scroll the whole game world according to the player position unless we reached the left or right screen limits
	if (player.GetX() >= leftScreenLimit && player.GetX() <= rightScreenLimit)
	{
		g->SetScrollPos(leftScreenLimit - player.GetX(), g->GetScrollPos().GetY());
	}
	// we stop scrolling with the player once we have reached the right limit and remain in that scrolling position at the right limit
	else if (player.GetX() > rightScreenLimit)
	{
		g->SetScrollPos(leftScreenLimit - rightScreenLimit, g->GetScrollPos().GetY());
	}
	// scroll the whole game world according to the player position unless we reached the up or down screen limits
	if (player.GetY() >= downScreenLimit && player.GetY() <= upScreenLimit)
	{
		g->SetScrollPos(g->GetScrollPos().GetX(), downScreenLimit - player.GetY());
	}
	// we stop scrolling with the player once we have reached the up limit and remain in that scrolling position at the up limit
	else if (player.GetY() > upScreenLimit)
	{
		g->SetScrollPos(g->GetScrollPos().GetX(), downScreenLimit - upScreenLimit);
	}


	
	map.LoadImage("map3.png");
	map.SetImage("map3.png");
	map.Draw(g);

	m_npc.Draw(g);

	player.LoadImage("player.png");
	player.SetImage("player.png");
	player.Draw(g);

	zomb1.LoadImage("Zombie.png");
	zomb1.SetImage("Zombie.png");
	zomb1.Draw(g);

	for (NODE n : m_graph)
		for (CONNECTION c : n.conlist)
			g->DrawLine(n.pos, m_graph[c.nEnd].pos, CColor::Black());
	m_nodes.for_each(&CSprite::Draw, g);

	for (CSprite* zomb : zombieList) zomb->Draw(g);
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{

	// Create Nodes
	int i = 0;
	for (NODE n : m_graph)
	{
		stringstream s;
		s << i++;
		m_nodes.push_back(new CSpriteOval(n.pos, 12, CColor::White(), CColor::Black(), 0));
		m_nodes.push_back(new CSpriteText(n.pos, "arial.ttf", 14, s.str(), CColor::Black(), 0));
	}
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	StartGame();	// exits the menu mode and starts the game mode
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	player.SetPosition(640, 384); //640,384

	zomb1.SetPosition(40, 530);
	zomb1.SetState(wait);

	map.SetPosition(6086 / 2, 2119 / 2); // 6086 x 2119

	
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
}

// called when the game is over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}


void CMyGame::PlayerControl()
{

	float hwalkSpeed = 270; // horizontal walking speed 70
	float vwalkSpeed = 270; // vertical walking speed 70

	float lowerlimit = 30;  // setting the limits
	float upperlimit = 2119 - 30;
	float leftlimit = 30;
	float rightlimit = 6086 - 30;

	enum walk_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT };



	// resetting the player to standing each time and then
	player.SetMotion(0, 0);
;

	// setting the speeds and walk status according to the keyboard controls
	if (IsKeyDown(SDLK_d)) 
	{ 
		player.SetXVelocity(hwalkSpeed); 
		player.SetStatus(WALKRIGHT); 
		player.SetRotation(player.GetDirection());
	}

	if (IsKeyDown(SDLK_a)) 
	{ 
		player.SetXVelocity(-hwalkSpeed); 
		player.SetStatus(WALKLEFT);
		player.SetRotation(player.GetDirection());
	}

	if (IsKeyDown(SDLK_w))
	{
		if (player.GetStatus() == STANDLEFT) player.SetStatus(WALKLEFT);
		if (player.GetStatus() == STANDRIGHT) player.SetStatus(WALKRIGHT);
		player.SetYVelocity(vwalkSpeed);
		player.SetRotation(player.GetDirection());
	}
	if (IsKeyDown(SDLK_s))
	{
		if (player.GetStatus() == STANDLEFT) player.SetStatus(WALKLEFT);
		if (player.GetStatus() == STANDRIGHT) player.SetStatus(WALKRIGHT);
		player.SetYVelocity(-vwalkSpeed);
		player.SetRotation(player.GetDirection());
	}


	// setting the player movement limits
	if (player.GetY() > upperlimit) player.SetY(upperlimit);
	if (player.GetY() < lowerlimit) player.SetY(lowerlimit);
	if (player.GetX() < leftlimit) player.SetX(leftlimit);
	if (player.GetX() > rightlimit) player.SetX(rightlimit);

	player.Update(GetTime());
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x, Uint16 y)
{
	CVector v(x, y);	// destination

	

	// find the first node: the closest to the NPC
	vector<NODE>::iterator iFirst =
		min_element(m_graph.begin(), m_graph.end(), [this](NODE& n1, NODE& n2) -> bool {
		return Distance(n1.pos, m_npc.GetPos()) < Distance(n2.pos, m_npc.GetPos());
			});

	// find the last node: the closest to the destination
	vector<NODE>::iterator iLast =
		min_element(m_graph.begin(), m_graph.end(), [v](NODE& n1, NODE& n2) -> bool {
		return Distance(n1.pos, v) < Distance(n2.pos, v);
			});

	int nFirst = iFirst - m_graph.begin();
	int nLast = iLast - m_graph.begin();


	// remove the current way points and reset the NPC
	if (!m_waypoints.empty())
	{
		m_waypoints.clear();
		m_npc.SetVelocity(0, 0);
	}

	// call the path finding algorithm to complete the waypoints
	vector<int> path;
	if (PathFind(m_graph, nFirst, nLast, path))
	{
		for (int i : path)
			m_waypoints.push_back(m_graph[i].pos);
		m_waypoints.push_back(v);
	}
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
