#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <thread>
#include <math.h>

std::string map[]{
	"#######################",
	"#                     #",
	"#                     #",
	"#                     #",
	"#                     #",
	"#                     #",
	"#       g             #",
	"#                     #",
	"#               #######",
	"#               #     #",
	"#               #  C  #",
	"#######################",
};

struct position {
	int x;
	int y;
};

struct entity {
	std::string name;
	int health = 40;
	int maxHealth = 40;
	int strength = 10;
	int defense = 10;
	position pos{ 0, 0 };
	long gold = 0;
};

enum state {
	ENTER_NAME,
	MAIN_GAME,
	GAME_OVER,
	EXIT
} currentState;

enum command {
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT,
	ATTACK,
	PLAYER_INFO,
	EXIT_GAME,
	OPEN_CHEST,
	HELP,
	VERY_SECRET_POWER_BUFF
};

entity player;
entity goblin{ "Goblin", 40, 40, 20, 20, { 8, 6 } };
std::string resultMessage;

state enter_name() {
	std::cout << "   ______            __  __           __   ______                   \r\n";
	std::cout << "  / ____/___  ____  / / / /___ ______/ /__/ ____/___ _____ ___  ___ \r\n";
	std::cout << " / /   / __ \\/ __ \\/ /_/ / __ `/ ___/ //_/ / __/ __ `/ __ `__ \\/ _ \\\r\n";
	std::cout << "/ /___/ /_/ / /_/ / __  / /_/ / /__/ ,< / /_/ / /_/ / / / / / /  __/\r\n";
	std::cout << "\\____/ .___/ .___/_/ /_/\\__,_/\\___/_/|_|\\____/\\__,_/_/ /_/ /_/\\___/ \r\n";
	std::cout << "     /_/   /_/                     b y     D a r k E c l i p z        \r\n";
	std::cout << "\r\n";
	std::cout << "  Welcome to the CppHackGame!\r\n\r\n";
	std::cout << "  You will enter a map that contains a chest (C) that is impossible to\r\n";
	std::cout << "  open. Can you trick the program and open the chest?\r\n\r\n";
	std::cout << "  Enter a name: ";
	std::cin >> player.name;
	if (player.name.size() < 2) {
		std::cout << "  Name is too short, use atleast 2 characters . . .";
		std::this_thread::sleep_for(std::chrono::seconds(1));
		return state::ENTER_NAME;
	}
	player.pos = { 18, 2 };
	map[player.pos.y][player.pos.x] = 'P';
	resultMessage = "[tip] Use 'help' to see available commands.";
	return state::MAIN_GAME;
}

void draw_map() {
	for (auto& line : map) {
		std::cout << line << std::endl;
	}
}

command parse_command(std::string cmd) {
	if (cmd == "up"			|| cmd == "u")  return MOVE_UP;
	if (cmd == "down"		|| cmd == "d")  return MOVE_DOWN;
	if (cmd == "left"		|| cmd == "l")  return MOVE_LEFT;
	if (cmd == "right"		|| cmd == "r")  return MOVE_RIGHT;
	if (cmd == "attack"		|| cmd == "a")  return ATTACK;
	if (cmd == "player-info"|| cmd == "pi") return PLAYER_INFO;
	if (cmd == "open")					    return OPEN_CHEST;
	if (cmd == "help"		|| cmd == "h")  return HELP;
	if (cmd == "exit")				        return EXIT_GAME;
	if (cmd == "very-secret-power-buff")	return VERY_SECRET_POWER_BUFF;
	resultMessage = "Invalid command, try help for more information.";
}

command get_command() {
	std::cout << "cmd> ";
	std::string cmd;
	std::cin >> cmd;
	return parse_command(cmd);
}

bool can_move_to(int x, int y) {
	return !(x < 0 || y < 0 || x > map[0].size() || y > map->size() || map[y][x] != ' ');
}

void move_up() {
	if (can_move_to(player.pos.x, player.pos.y - 1)) {
		map[player.pos.y][player.pos.x] = ' ';
		player.pos.y--;
		map[player.pos.y][player.pos.x] = 'P';
	}
	else {
		resultMessage = "Unable to move there, the path is obstructed.";
	}
}

void move_down() {
	if (can_move_to(player.pos.x, player.pos.y + 1)) {
		map[player.pos.y][player.pos.x] = ' ';
		player.pos.y++;
		map[player.pos.y][player.pos.x] = 'P';
	}
	else {
		resultMessage = "Unable to move there, the path is obstructed.";
	}
}

void move_left() {
	if (can_move_to(player.pos.x - 1, player.pos.y)) {
		map[player.pos.y][player.pos.x] = ' ';
		player.pos.x--;
		map[player.pos.y][player.pos.x] = 'P';
	}
	else {
		resultMessage = "Unable to move there, the path is obstructed.";
	}
}

void move_right() {
	if (can_move_to(player.pos.x + 1, player.pos.y)) {
		map[player.pos.y][player.pos.x] = ' ';
		player.pos.x++;
		map[player.pos.y][player.pos.x] = 'P';
	}
	else {
		resultMessage = "Unable to move there, the path is obstructed.";
	}
}

int get_player_damage() {
	int dmg = std::max(0, player.strength - goblin.defense / 2);
	if (!dmg)
		return 0;
	return std::rand() % dmg;
}

int get_enemy_damage() {
	int dmg = std::max(0, goblin.strength - player.defense / 2);
	if (!dmg)
		return 0;
	return std::rand() % dmg;
}

bool is_symbol_nearby(char symbol) {
	return map[player.pos.y + 1][player.pos.x] == symbol
		|| map[player.pos.y - 1][player.pos.x] == symbol
		|| map[player.pos.y][player.pos.x + 1] == symbol
		|| map[player.pos.y][player.pos.x - 1] == symbol;
}

void attack() {
	if (!is_symbol_nearby('g')) {
		resultMessage = "There is no enemy nearby.";
		return;
	}
	std::stringstream result;
	int playerDamage = get_player_damage();
	if (playerDamage > 0) {
		result << "You dealt " << playerDamage << " damage to the goblin.\r\n";
	}
	else {
		result << "You missed your attack.\r\n";
	}
	
	goblin.health -= playerDamage;
	if (goblin.health > 0) {
		result << "The goblin has " << goblin.health << " health remaining.\r\n";
		int enemyDamage = get_enemy_damage();
		result << "You received " << enemyDamage << " damage from the goblin.";
		player.health -= enemyDamage;
	}
	else {
		int a = 1; int b = a; int c = b; 
		long goldGained = b << 22; goldGained <<= 24;
		int d = c << a << 3; d += a << 5; d += a << 18;
		result << "You have slaughtered the goblin.\r\nYou gained " << goldGained + d << " gold.";
		map[goblin.pos.y][goblin.pos.x] = ' ';
		map[9][16] = ' ';
		player.gold += goldGained + d; 
	}

	resultMessage = result.str();
}

void player_info() {
	std::stringstream output;
	output << "-- Player statistics --\r\n\r\n";
	output << "Health: " << player.health << "/" << player.maxHealth << std::endl;
	output << "Strength: " << player.strength << std::endl;
	output << "Defense: " << player.defense << std::endl;
	output << "Position: " << player.pos.x << ", " << player.pos.y << std::endl;
	resultMessage = output.str();
}

bool check_password(std::string password) {
	int checksum = 0;
	for (int i = 0; i < player.name.size(); i++) 
		checksum += player.name[i] * i % (int)std::pow(2, i);
	for (auto& c : password)
		checksum -= (int)c;
	return !checksum;
}

void open_chest() {
	if (!is_symbol_nearby('C')) {
		resultMessage = "There is no chest nearby.";
		return;
	}
	std::cout << "  [GOBLIN CHEST AUTHENTICATOR]\r\n\r\n";
	std::cout << "     GoblinAuth(R) is secured with a powerful name checksum.\r\n\r\n";
	std::cout << "     password: ";
	std::string password;
	std::cin >> password;
	if (password == "its this easy") {
		resultMessage = "No it isn't!";
	}
	else if (check_password(password)) {
		long value = 3141592653*3;
		for (int i = 0; i++ < 3; value >>= 1);
		player.gold += value;
		resultMessage = "You opened the chest!\r\nYou have found " + std::to_string(value) + " gold.";
		map[10][19] = ' ';
	}
	else {
		resultMessage = "The password is incorrect!";
	}
}

void draw_info() {
	std::cout << "HP: " << player.health << "/" << player.maxHealth << "  Gold: " << player.gold << std::endl;
}

state game_over() {
	std::stringstream msg;
	msg << "██╗   ██╗  ██████╗  ██╗   ██╗    ██████╗   ██╗  ███████╗  ██████╗ \r\n";
	msg << "╚██╗ ██╔╝ ██╔═══██╗ ██║   ██║    ██╔══██╗  ██║  ██╔════╝  ██╔══██╗\r\n";
	msg << " ╚████╔╝  ██║   ██║ ██║   ██║    ██║  ██║  ██║  █████╗    ██║  ██║\r\n";
	msg << "  ╚██╔╝   ██║   ██║ ██║   ██║    ██║  ██║  ██║  ██╔══╝    ██║  ██║\r\n";
	msg << "   ██║    ╚██████╔╝ ╚██████╔╝    ██████╔╝  ██║  ███████╗  ██████╔╝\r\n";
	msg << "   ╚═╝     ╚═════╝   ╚═════╝     ╚═════╝   ╚═╝  ╚══════╝  ╚═════╝ \r\n\r\n";
	msg << "  Game restarts in 5 seconds . . . ";
	std::cout << msg.str();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	player.health = player.maxHealth;
	return state::ENTER_NAME;
}

void draw_message() {
	if (resultMessage.size() > 0) {
		std::cout << resultMessage << std::endl;
		resultMessage = "";
	}
}

void help() {
	std::stringstream output;
	output << "-- HELP --\r\n\r\n";
	output << "On the map you are represented with the symbol P.\r\nThere also is a goblin (g) and a chest (C).\r\n\r\n";
	output << "Available commands are: \r\n";
	output << "  - up, u: move up\r\n";
	output << "  - down, d: move down\r\n";
	output << "  - left, l: move left\r\n";
	output << "  - right, r: move right\r\n";
	output << "  - attack, a: attack nearby monster\r\n";
	output << "  - player-info, pi: player information\r\n";
	output << "  - open: opens a nearby chest\r\n";
	output << "  - help: shows this\r\n";
	output << "  - exit: quits game\r\n";
	resultMessage = output.str();
}

void heal_player() {
	if (player.health < player.maxHealth) {
		player.health++;
	}
}

state main_game() {
	draw_info();
	draw_map();
	draw_message();
	command cmd = get_command();
	switch (cmd) {
	case MOVE_UP:		move_up();		break;
	case MOVE_DOWN:		move_down();	break;
	case MOVE_LEFT:		move_left();	break;
	case MOVE_RIGHT:	move_right();	break;
	case ATTACK:		attack();		break;
	case PLAYER_INFO:	player_info();	break;
	case OPEN_CHEST:	open_chest();	break;
	case HELP:			help();			break;
	case VERY_SECRET_POWER_BUFF:
		player.strength = 20;
		player.defense = 20;
		resultMessage = "You have gained 10 strength and 10 defense!";
		break;
	case EXIT_GAME:							 
		return state::EXIT;
	}
	if (player.health <= 0) {
		return state::GAME_OVER;
	}
	heal_player();
	return state::MAIN_GAME;
}

int main() {
	currentState = state::ENTER_NAME;
	while (currentState != state::EXIT) {
		std::system("cls");
		switch (currentState) {
		case state::ENTER_NAME: currentState = enter_name(); break;
		case state::MAIN_GAME:  currentState = main_game();  break;
		case state::GAME_OVER:	currentState = game_over();  break;
		default:
			std::cout << "State " << currentState << " is invalid. Exiting application.\r\n";
			currentState = state::EXIT;
		}
	}
}
