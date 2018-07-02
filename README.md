Ouroboros
========

## Homepage

	http://rottenvisions.com

## What is Ouroboros?

	Ouroboros is a MMORPG server engine built around the BigWorld networking model. It is built in C++ with an
	abstracted python scripting layer for all game logic. This includes hot reloading - adding and removing features
	while the server is running.

	It current supports Unity3D as the game client, but using the SDK's principle, it can be made to work with anything
	else including custom game engines, Unreal Engine 4, etc.

	Ouroboros theoretically supports and infinite amount of players. The underlying architecture is designed as a
	multi-process distributed dynamic load balancing solution. As long as you continuously expand vertically
	(faster and more powerful servers) and horizontally (adding more servers) the limit is infinite.

	This networking model is used in very few MMORPGs today, the only big game being World of Tanks,
	World of Planes and World of Warships. MMOs such as World of Warcraft use a different principle,
	favoring servers per continent and sharding (adding to their shared worlds).

	Ouroboros was developed to support any MMO projects Rotten Visions makes, including project IDMO.
