/**
	Iron Peak
	A chilly mountain peak filled with iron ore and coal.
	
	@author Maikel
*/


// Scenario properties which can be set later by the lobby options.
static const SCENOPT_Material = 3; // Amount of material available from start.
static const SCENOPT_MapSize = 1; // Size of the map.
static const SCENOPT_Difficulty = 1; // Difficulty settings.

// Spawn location for all players.
static mountain_location;

protected func Initialize()
{
	// Goal: Expand your area of influence to secure the ore.
	var goal = CreateObject(Goal_Expansion);
	goal->SetExpansionGoal(300 + 100 * SCENOPT_Difficulty);
	
	// Second goal: Construct metal.
	var metal_cnt = 10 + 10 * SCENOPT_Difficulty;
	goal = CreateObject(Goal_Construction);
	goal->AddConstruction(Metal, metal_cnt);
	
	// Rules: team account and buying at flagpole.
	CreateObject(Rule_TeamAccount);
	CreateObject(Rule_BuyAtFlagpole);
	
	// Find A good location for the players to start.
	FindMountainLocation();
	
	// Initialize different parts of the scenario.
	InitEnvironment();
	InitVegetation(SCENOPT_MapSize);
	InitAnimals();
	InitMaterial(SCENOPT_Material);
	return;
}


/*-- Player Initialization --*/

protected func InitializePlayer(int plr)
{ 
	// Move clonks to location and give them a shovel.
	var index = 0, crew;
	while (crew = GetCrew(plr, index))
	{
		crew->SetPosition(mountain_location[0], mountain_location[1]);
		// First clonk can construct, others can mine.
		if (index == 0)
		{
			crew->CreateContents(Hammer);
			crew->CreateContents(Axe);
		}
		else
		{
			crew->CreateContents(Shovel);
			crew->CreateContents(Pickaxe);
		}
		if (SCENOPT_Material >= 2)
			crew->CreateContents(Loam, 2);
		if (SCENOPT_Material == 3)
			crew->CreateContents(DynamiteBox);
		index++;
	}
	
	// Harsh zoom range.
	SetPlayerZoomByViewRange(plr, 500, nil, PLRZOOM_Direct | PLRZOOM_LimitMax);
	SetPlayerViewLock(plr, true);
	
	// Give the player basic knowledge.
	GivePlayerBasicKnowledge(plr);
	GivePlayerSpecificKnowledge(plr, [Ropeladder]);
		
	// Set player wealth.
	SetWealth(plr, 20 + 20 * SCENOPT_Material);
	return;
}

private func FindMountainLocation()
{
	// Default to top middle of the map.
	mountain_location = [LandscapeWidth() / 2, LandscapeHeight() / 2];
	var x = 0, y = 0;
	for (var i = 0; i < 1000; i++)
	{
		// Random x coordinate.
		var x = Random(LandscapeWidth()), y = 0;
		// Find corresponding y coordinate.
		while (!GBackSolid(x, y) && y < 9 * LandscapeHeight() / 10)
			y += 2;
		// Check if surface is relatively flat (check for flatter surfaces first).
		var d = i / 250 + 1;		
		if (!GBackSolid(x + 10, y - 20) && !GBackSolid(x - 10, y - 20) && !GBackSolid(x + 10, y - d) && !GBackSolid(x - 10, y - d) && GBackSolid(x + 10, y + d) && GBackSolid(x - 10, y + d))
			if (y > LandscapeHeight() / 2)
			{
				mountain_location = [x, y - 10];
				break;
			}
	} 
	return;
}


/*-- Scenario Initialization --*/

private func InitEnvironment()
{
	// Cover the mountain in some snow already.
	GiveMountainSnowCover();
	// Add a snow storm effect, strong winds and lot's of snow.
	AddEffect("SnowStorm", nil, 100, 5, nil);
	
	// Set time of day to evening and create some clouds and celestials.
	Cloud->Place(20);
	CreateObject(Environment_Celestial);
	var time = CreateObject(Environment_Time);
	time->SetTime(60 * 22);
	time->SetCycleSpeed(0);
	
	// A light blue hue, to indicate the cold climate.
	var blue = 6;
	SetGamma(RGB(0, 0, blue), RGB(128 - blue, 128 - blue, 128 + blue), RGB(255 - blue, 255 - blue, 255));
	
	// Some natural disasters. 
	Earthquake->SetChance(5 + 5 * SCENOPT_Difficulty);
	// TODO: Rockfall.
	return;
}

private func InitVegetation(int map_size)
{
	// Place some coniferous trees, but only up to 2/3 of the mountain.
	Tree_Coniferous->Place(16 + Random(5), Rectangle(0, LandscapeHeight() / 3, LandscapeWidth(), 2 * LandscapeHeight() / 3));
	// Also some cave mushrooms as a source of wood.
	LargeCaveMushroom->Place(5 + 2 * map_size + Random(5), nil, { terraform = false });

	// Some mushrooms as source of food.
	Mushroom->Place(30 + Random(10));
	
	// Some objects in the earth.	
	PlaceObjects(Rock, 10 + 10 * map_size + Random(10),"Earth");
	PlaceObjects(Firestone, 20 + 10 * map_size + Random(5), "Earth");
	PlaceObjects(Loam, 20 + 10 * map_size + Random(5), "Earth");
	return;
}

private func InitAnimals()
{
	return;
}

private func InitMaterial(int amount)
{
	// An abandoned lorry somewhere close to the highest point of the peak.
	var lorry = CreateObject(Lorry, LandscapeWidth() / 2, FindHeight(LandscapeWidth() / 2));
	lorry->CreateContents(Pickaxe, 2);
	lorry->CreateContents(Shovel, 2);
	lorry->CreateContents(Bread, 2 * amount);
	lorry->CreateContents(Dynamite, 2 * amount);
	lorry->CreateContents(DynamiteBox, amount);	
	lorry->CreateContents(Ropeladder, amount);	
	return;
}

private func GiveMountainSnowCover()
{
	// Loop over the map horizontally.
	for (var x = 0; x < LandscapeWidth(); x += 20)
	{
		// Find height of mountain at this x.
		var y = 0;
		while (!GBackSolid(x, y) && y < LandscapeHeight())
			y += 10;
		if (y < 9 * LandscapeHeight() / 10)
			CastPXS("Snow", 10 + Random(50), 40, x, y - 40, 180, 40);
	}
	return;
}

// This effect should be called every 5 frames.
global func FxSnowStormStart(object target, proplist effect)
{
	// Always a strong wind, either to the left or the right.
	effect.wind = (2 * Random(2) - 1) * (90 + Random(10));
	// Accordingly a stormy sound.
	Sound("WindLoop.ogg", true, 80, nil, 1);
	return 1;
}

global func FxSnowStormTimer(object target, proplist effect)
{
	// Change wind every now and then.
	if (!Random(200))
		effect.wind = (2 * Random(2) - 1) * (90 + Random(10));

	// Adapt current wind to target wind and add a little random.
	var wind = GetWind(0, 0, true);
	if (effect.wind > wind)
		SetWind(wind + Random(3));
	else if (effect.wind < wind)
		SetWind(wind - Random(3));
	else 
		SetWind(wind + 1 - Random(3));
	return 1;
}

/*-- Some helper functions --*/

global func LogMatCounts()
{
	for (var i = 0; i < 128; i++)
	{
		if (GetMaterialCount(i) > 0)
			Log("Material %s has %d count.", MaterialName(i), GetMaterialCount(i) / 100);
	}
	return;
}
