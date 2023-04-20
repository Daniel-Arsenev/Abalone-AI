///////////
// includes
//

#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <unordered_map>

#include <numeric>
#include <ranges>
#include <iterator>
#include <algorithm>

#include <iostream>
#include <cstdlib>
#include <climits>

#include <chrono>
#include <thread>




///////////////
// CONVENIENCE
// always wanted to try out the AAA style

using namespace std;

#define in :
#define is ==
#define not !

#define var auto
#define let const auto
#define func auto
#define init

#define nextcase break;case 
#define lambda(...) [&](__VA_ARGS__)
#define loop() while(true)




///////////////////
// DATA STRUCTURES
// by requirement it has to be only one file

struct point {
	int x;
	int y;

	func operator+(point) const;
	func operator-(point) const;
	func operator*(int)   const;

	func repr() const;
};

enum dir {
	UP = 0,
	FORWARD = 1,
	RIGHT = 2,
	DOWN = 3,
	BACK = 4,
	LEFT = 5,
};


let dirs = array<dir, 6> { UP, FORWARD, RIGHT, DOWN, BACK, LEFT };
let half_dirs = array<dir, 3> { UP, FORWARD, RIGHT };

let DIRS = array<point, 6> { point{ 0,  1 }, point{ 1,  1 }, point{ 1,  0 }, point{ 0, -1 }, point{ -1, -1 }, point{ -1,  0 } };



template<typename type, int size_x, int size_y>
struct array2d {
	array<array<type, size_y>, size_x> inner;

	func& operator[](int);
	func& operator[](point);
};

enum color {
	WHITE = -1,
	EMPTY = 0,
	BLACK = 1,
};


struct piecedata {
	color           piececolor;
	array<char, 6>  neighbors;
	array<bool, 6>  dirtyflags;
};


struct movedata {
	// data needed for a move
	color piececolor;
	point origin;
	dir direction;
	int pulled_neighbors;
	dir pulled_direction;

	// move score, needed for move ordering
	float score = 0;

	// data needed for undoing 
	int pushed_enemies = 0;
	bool captured_enemy = false;



	init movedata();
	init movedata(color, point, dir, int);        // for strait, non pushing moves
	init movedata(color, point, dir, int, bool, int);  // for strait, pushing moves
	init movedata(color, point, dir, int, dir);   // for broadside moves

	func evaluate() -> int;
	func is_valid() -> bool;

	func repr()->string;
};



struct board {

	array2d<piecedata, 9, 9> pieces;
	color current_turn;

	int captured_white_pieces = 0;
	int captured_black_pieces = 0;

	unordered_map<uint64_t, int> transposition_table;
	uint64_t boardhash = 0;



	func make_move(movedata&);
	func undo_move(movedata);

	func black_won();
	func white_won();

	func get_neighbor(point, dir) -> int;
	func dirty_neighbors(point) -> void;
	func update_neighbor(point, dir) -> int;

	func update_hash(point, color);
	func position_is_transposition();
	func transposition_value();
	func add_to_transposition_table(int);

	func evaluate() -> int;
	func search(int, int, int) -> int;
	func find_best(int)->movedata;


	func find_random()->movedata;
	func find_best_or_random()->movedata;


	func print_board() -> void;
};


/////////////
// CONSTANTS
// and data


let EMPTY_SPACE = piecedata{ EMPTY , array<char, 6> {char(0), char(0), char(0), char(0), char(0), char(0) },  array<bool, 6> {true, true, true, true, true, true} };
let BLACK_PIECE = piecedata{ BLACK , array<char, 6> {char(0), char(0), char(0), char(0), char(0), char(0) },  array<bool, 6> {true, true, true, true, true, true} };
let WHITE_PIECE = piecedata{ WHITE , array<char, 6> {char(0), char(0), char(0), char(0), char(0), char(0) },  array<bool, 6> {true, true, true, true, true, true} };

let NONE_MOVE = movedata();

let STARTING_BOARD = "B:BBBBBBBBBBB..BBB.............................WWW..WWWWWWWWWWW";
let TESTING_BOARD = "B:...B..BBBB..BBBBB....BB.......B......WB.W..WW...W..WWWWWW.WWW";



let REVESER_LIST = array<dir, 6> { DOWN, BACK, LEFT, UP, FORWARD, RIGHT };

let ROWS_LENGTH = array<int, 9> { 5, 6, 7, 8, 9, 8, 7, 6, 5 };
let ROWS_OFFSETS = array<int,9> { 0, 0, 0, 0, 0, 1, 2, 3, 4 };

let INDEX_TO_LETTER = array<string, 9> { "A", "B", "C", "D", "E", "F", "G", "H", "I" };
let INDEX_TO_NUMBER = array<string, 9> { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

let VADLID_SQUARES = array<array<bool, 9>, 9>{ array<bool, 9>{true, true, true, true, true, false, false, false, false}, array<bool, 9>{true, true, true, true, true, true, false, false, false}, array<bool, 9>{true, true, true, true, true, true, true, false, false}, array<bool, 9>{true, true, true, true, true, true, true, true, false}, array<bool, 9>{true, true, true, true, true, true, true, true, true}, array<bool, 9>{false, true, true, true, true, true, true, true, true}, array<bool, 9>{false, false, true, true, true, true, true, true, true}, array<bool, 9>{false, false, false, true, true, true, true, true, true}, array<bool, 9>{false, false, false, false, true, true, true, true, true} };
let SCORE_MAP = array<array<int, 9>, 9> { array<int, 9>{-6, -6, -6, -6, -6, 0, 0, 0, 0}, array<int, 9>{-6, 1, 1, 1, 1, -6, 0, 0, 0}, array<int, 9>{-6, 1, 5, 5, 5, 1, -6, 0, 0}, array<int, 9>{-6, 1, 5, 3, 3, 5, 1, -6, 0}, array<int, 9>{-6, 1, 5, 3, 0, 3, 5, 1, -6}, array<int, 9>{0, -6, 1, 5, 3, 3, 5, 1, -6}, array<int, 9>{0, 0, -6, 1, 5, 5, 5, 1, -6}, array<int, 9>{0, 0, 0, -6, 1, 1, 1, 1, -6}, array<int, 9>{0, 0, 0, 0, -6, -6, -6, -6, -6} };
let INWARDS_MAP = array<array<int, 9>, 9> { array<int, 9>{-1, -1, -1, -1, -1, 0, 0, 0, 0}, array<int, 9>{-1, 1, 1, 1, 1, -1, 0, 0, 0}, array<int, 9>{-1, 1, 3, 3, 3, 1, -1, 0, 0}, array<int, 9>{-1, 1, 3, 5, 5, 3, 1, -1, 0}, array<int, 9>{-1, 1, 3, 5, 7, 5, 3, 1, -1}, array<int, 9>{0, -1, 1, 3, 5, 5, 3, 1, -1}, array<int, 9>{0, 0, -1, 1, 3, 3, 3, 1, -1}, array<int, 9>{0, 0, 0, -1, 1, 1, 1, 1, -1}, array<int, 9>{0, 0, 0, 0, -1, -1, -1, -1, -1} };

let BLACK_PIECE_RANDOMS = array<uint64_t, 81>{ 11783152498764754964ull,9867829455511315619ull,9953171327645099357ull,10230630900545602954ull,11102429418757237286ull,11350259752494869987ull,11467272881717202554ull,11356642555556145607ull,11812603383225106406ull,11096409330424177249ull,13781805118507418293ull,12471865968944026484ull,9673720127685697236ull,13626237943228591603ull,11303210624070716086ull,13160847536653376646ull,13393811846437647898ull,10657426637371296306ull,10755195896827722928ull,9458204603351937453ull,12921212571068973295ull,13550225211166717028ull,12099414341044102258ull,11526969447764888394ull,11908810512577404677ull,10451341679312475918ull,11301084581107878659ull,10006312354074451749ull,12691585142716658303ull,11933176512198261560ull,9433884715535468742ull,9429342356176946875ull,10548896182922016207ull,10013099869414075073ull,10167888578794493837ull,11303849634804686333ull,10118660675244194786ull,13662584557934885822ull,9435638721905820797ull,11235078958594081182ull,13419505041365648673ull,9608237144802536248ull,10322681305975220883ull,13188015702987347907ull,9576579161821979690ull,13446277752215705410ull,9484220544563868019ull,9354140977878332379ull,10263961686532507291ull,13249504781537940243ull,11098647983862232251ull,12496116816804220936ull,12720074943079158622ull,11162019297037140609ull,13662194988197583121ull,10536317133337027215ull,9252174716792452343ull,9294258442456646586ull,10673134161247521303ull,10025921011931868104ull,12277094434512474741ull,12759221648540210377ull,9300663529115108601ull,11061126653644182366ull,11652264947928967488ull,9244017348478472533ull,13593155996171185529ull,13484130219249488737ull,13691048147066813283ull,13363552590376554307ull,10827649789896807957ull,12233347324514309796ull,9428091702473357442ull,10532017146898258264ull,13198887991073820392ull,13632423828222833010ull,12920573580477034444ull,12450210938822723412ull,13304009303463342020ull,9950994668981286941ull,9780068500029051552ull };
let WHITE_PIECE_RANDOMS = array<uint64_t, 81>{ 9869392211838688588ull,10970277029028301506ull,10728175530471809336ull,12731481666958377818ull,13445274546292859355ull,12424050560076925568ull,12727958821627837224ull,10207967553728354183ull,11534449272969929532ull,11664674669121499684ull,11291518937054180427ull,12419937292961978843ull,11506605825583124351ull,10650990300564705766ull,12121825900449026452ull,10076132039376622579ull,12833407139387578922ull,10594219414472450146ull,13468604630704580399ull,11304554173993872290ull,9845079204038305393ull,9691490399063526239ull,13387208310818753362ull,10730900318181525421ull,12698187930809962626ull,11551130065519944473ull,9351870919478183414ull,12257983920306285737ull,11652965683511301762ull,12338613003757642871ull,13719613663318242781ull,12849366871892505291ull,10391643968421388466ull,9663454876402393887ull,13515087364573334400ull,13515403549079924542ull,12543296692248526984ull,12132467741030880234ull,12594922666579370006ull,12817678942017398393ull,11558575008189378483ull,11761541476457953505ull,10036916449940305772ull,10828519572091715078ull,11659433027104797275ull,11924559436095767181ull,10413453564177964539ull,13676301116728826959ull,11361561210068092198ull,9797753047888746007ull,11578705621285454127ull,11884229135252586245ull,10982733998208643292ull,10988800767842860943ull,13831429728857142990ull,10467404519904970528ull,11541909037446974052ull,10588652463993663706ull,12781589953466745131ull,11154475119862247315ull,10304171322672042193ull,11278634874100000857ull,13481919832032848457ull,12711935671227468246ull,12928939898585026338ull,9669031891010375855ull,12965755803904435008ull,12219584723983229849ull,11571965292062688098ull,13298421915966302174ull,13550470092093134076ull,12548598704541395783ull,11392679437668939295ull,12197492001238807990ull,13324130828672108844ull,12859966506837372548ull,12149482703062496625ull,13710605383790439429ull,11531606288508380890ull,11643511028142722172ull,10915808433911781394ull };



//////////////////
// UTIL FUNCTIONS
// 

func range(int end) {
	if (end > 0) {
		return views::iota(0, end);
	}
	else {
		return views::iota(0, 0);
	}
}

func range(int start, int end) {
	if (end > start) {
		return views::iota(start, end);
	}
	else {
		return views::iota(0, 0);
	}
}

func reverse_range(int start, int end) {
	return views::reverse(views::iota(start, end));

}

template<typename T>
func range_len(T container) {
	return views::iota(0, (int)container.size());
}


func opposite(dir dir) {
	return REVESER_LIST[dir];
}

func opposite(color c) -> color {
	return color(-c);
}


func is_valid(int x, int y)  -> bool {
	let x_in_bounds = (x >= 0 && x < 9);
	let y_in_bounds = (y >= 0 && y < 9);
	return x_in_bounds && y_in_bounds && VADLID_SQUARES[x][y];
}

func is_valid(point position)  -> bool {
	return is_valid(position.x, position.y);
}


func color_to_string(color color) -> string {
	switch (color) {
	case WHITE:
		return "w";

	case BLACK:
		return "B";

	case EMPTY:
		return ".";
	}
	return " ";
}

template<typename type>
func print(type s) {
	cout << s << "\n";
}





///////////////////
// IMPLEMENTATIONS
// 

// point
func point::operator  + (point p)    const {
	return point{ x + p.x, y + p.y };
}

func point::operator  - (point p)    const {
	return point{ x - p.x, y - p.y };
}

func point::operator  * (int skalar) const {
	return point{ skalar * x, skalar * y };
}

func point::repr() const {
	return "(x: " + to_string(x) + ", y: " + to_string(y) + ")";
}


// dir
func dirname(dir direction) {
	switch (direction) {
	case UP:
		return "UP";
	case FORWARD:
		return "FORWARD";
	case RIGHT:
		return "RIGHT";
	case DOWN:
		return "DOWN";
	case BACK:
		return "BACK";
	case LEFT:
		return "LEFT";
	}

	return " ";
}



// array2d
template<typename type, int x, int y>
func& array2d<type, x, y>::operator[](int i) {
	return inner[i];
}

template<typename type, int x, int y>
func& array2d<type, x, y>::operator[](point p) {
	return inner[p.x][p.y];
}



// movedata
movedata::movedata() {
	piececolor = EMPTY;
	origin = point {0, 0};
	direction = dir(0);
	pulled_neighbors = 0;
	pulled_direction = dir(0);
}

movedata::movedata(color input_color, point input_origin, dir input_direction, int input_pulled_neighbors) {
	piececolor = input_color;

	origin = input_origin;
	direction = input_direction;

	pulled_neighbors = input_pulled_neighbors;
	pulled_direction = opposite(input_direction);

	score = evaluate();
}

movedata::movedata(color input_color, point input_origin, dir input_direction, int input_pulled_neighbors, dir input_pulled_direction) {
	piececolor = input_color;

	origin = input_origin;
	direction = input_direction;

	pulled_neighbors = input_pulled_neighbors;
	pulled_direction = input_pulled_direction;

	score = evaluate();
}

movedata::movedata(color input_color, point input_origin, dir input_direction, int input_pulled_neighbors, bool capture, int pushed) {
	piececolor = input_color;

	origin = input_origin;
	direction = input_direction;

	pulled_neighbors = input_pulled_neighbors;
	pulled_direction = opposite(input_direction);

	captured_enemy = capture;
	pushed_enemies = pushed;

	score = evaluate();
}

func movedata::is_valid() -> bool {
	return not (piececolor == EMPTY);
}

func movedata::repr() -> string {
	return "from: " + origin.repr() + ", in dir: " + dirname(direction) + ", with " + to_string(pulled_neighbors) + " pulled from dir: " + dirname(pulled_direction);
}

func movedata::evaluate() -> int {
	if (captured_enemy) {
		return 100;
	}
	else {
		let target = origin + DIRS[direction];

		let start_value = INWARDS_MAP[origin.x][origin.y];
		let end_value = INWARDS_MAP[target.x][target.y];

		if (pushed_enemies > 0) {
			return pulled_neighbors + pushed_enemies + start_value - end_value;
		}
		else {
			return pulled_neighbors + end_value - start_value;
		}
	}
}

func better_move(movedata smaller, movedata bigger) {
	return smaller.score < bigger.score;
}



// board
func board::black_won() {
	return captured_white_pieces >= 6;
}

func board::white_won() {
	return captured_black_pieces >= 6;
}

func board::get_neighbor(point position, dir direction) -> int {
	var target_piece = pieces[position];

	// if entry in the table is no longer up to date, update it
	if (target_piece.dirtyflags[direction]) {
		return update_neighbor(position, direction);
	}
	// if entry in the table is up to date, just return it
	else {
		return target_piece.neighbors[direction];
	}
}

func board::dirty_neighbors(point position) -> void {
	var& origin_piece = pieces[position];
	var  origin_color = origin_piece.piececolor;

	origin_piece.dirtyflags = array<bool, 6>{ true, true, true, true, true, true };

	for (var dir in dirs) {
		var  close_target_position = position - DIRS[dir];
		if (not is_valid(close_target_position)) continue;

		var& close_target_pice = pieces[close_target_position];
		var  close_target_color = close_target_pice.piececolor;

		if (close_target_color == origin_color || close_target_color == EMPTY) {
			close_target_pice.dirtyflags[dir] = true;
		}


		var  far_target_position = position - DIRS[dir] * 2;
		if (not is_valid(far_target_position)) continue;

		var& far_target_pice = pieces[far_target_position];
		var  far_target_color = close_target_pice.piececolor;

		if (far_target_color == origin_color || far_target_color == EMPTY) {
			far_target_pice.dirtyflags[dir] = true;
		}

	}
}

func board::update_neighbor(point position, dir direction) -> int {
	var& update_piece = pieces[position];

	let close_target_position = position + DIRS[direction];

	if (is_valid(close_target_position) && pieces[close_target_position].piececolor == update_piece.piececolor) {
		let far_target_position = position + DIRS[direction] * 2;

		if (is_valid(far_target_position) && pieces[far_target_position].piececolor == update_piece.piececolor) {
			update_piece.neighbors[direction] = 2;
		}
		else {
			update_piece.neighbors[direction] = 1;
		}
	}
	else {
		update_piece.neighbors[direction] = 0;
	}

	update_piece.dirtyflags[direction] = false;
	return update_piece.neighbors[direction];

}

func board::update_hash(point position, color c) {

	int index = position.x * 9 + position.y;

	if (c == BLACK) {
		boardhash ^= BLACK_PIECE_RANDOMS[index];
	}
	if (c == WHITE) {
		boardhash ^= WHITE_PIECE_RANDOMS[index];
	}
}

func board::position_is_transposition() {
	// return false;
	return transposition_table.find(boardhash) != transposition_table.end();
}

func board::transposition_value() {
	return  transposition_table[boardhash];
}

func board::add_to_transposition_table(int score) {
	transposition_table[boardhash] = score;
}

func board::print_board() -> void {
	for (int i in reverse_range(0, 9)) {
		string toprint = "";
		for (int j in range(0, 9)) {
			if (is_valid(i, j)) {
				toprint += color_to_string(pieces[j][i].piececolor) + " ";
			}
			else {
				toprint += "  ";
			}
		}
		cout << toprint << "\n";
	}
	cout << "\n";
	cout << "captures black: " << captured_white_pieces << ", captures white: " << captured_black_pieces;
	cout << "\n" << "\n" << "\n";
}


func parse_to_board(string s) {
	var ret = board();

	var turn = s[0];
	switch (turn) {
		nextcase 'B':
		ret.current_turn = BLACK;
		nextcase 'W':
		ret.current_turn = WHITE;
	}

	var black_pieces = 0;
	var white_pieces = 0;

	var read_letters = 2;
	for (int i in range(9)) {
		for (int j in range(ROWS_LENGTH[i])) {

			var x = j + ROWS_OFFSETS[i];
			var y = i;

			var letter = s[read_letters++];
			switch (letter) {
				nextcase 'B':
				ret.pieces[x][y] = BLACK_PIECE;
				black_pieces++;
				nextcase 'W':
				ret.pieces[x][y] = WHITE_PIECE;
				white_pieces++;
				nextcase '.':
				ret.pieces[x][y] = EMPTY_SPACE;
			}
		}
	}

	ret.captured_black_pieces = 14 - black_pieces;
	ret.captured_white_pieces = 14 - white_pieces;

	for (int x in range(9)) {
		for (int y in range(9)) {
			for (var dir in dirs) {
				ret.update_neighbor(point{ x, y }, dir);
			}
		}
	}

	return ret;


}

func serilize_move(movedata move) -> string {
	func position_to_string = lambda(point position) {
		string x = INDEX_TO_NUMBER[position.x];
		string y = INDEX_TO_LETTER[position.y];
		return y + x;

	};

	let strait_move = (opposite(move.direction) == move.pulled_direction);
	if (strait_move) {

		let initial_offset = DIRS[move.pulled_direction] * move.pulled_neighbors;
		let propper_origin = move.origin + initial_offset;
		let first = position_to_string(propper_origin);

		let move_offset = DIRS[move.direction];
		let propper_target = propper_origin + move_offset;
		let last = position_to_string(propper_target);

		return first + "," + last + "\n";

	}
	else {

		let propper_origin = move.origin;
		let first = position_to_string(propper_origin);

		let broadside_offset = DIRS[move.pulled_direction] * move.pulled_neighbors;
		let broadside_endpos = propper_origin + broadside_offset;
		let second = position_to_string(broadside_endpos);

		let move_offset = DIRS[move.direction];
		let propper_target = propper_origin + move_offset;
		let last = position_to_string(propper_target);


		return first + "-" + second + "," + last + "\n";

	}
}




////////////////
// STRATEGY CODE
// includes some implementation for board

class movegen {
private:
	vector<movedata> moves;
	int picked_moves = 0;
public:
	init movegen(board* board) {
		generate(board);
	}

	func generate(board* board) -> void {

		// separate out a partial function to decrease nesting
		func generate_for_target_position = lambda(int x, int y) {

			let move_origin = point{ x, y };
			let moved_piece = board->pieces[move_origin];

			if (moved_piece.piececolor != board->current_turn) {
				return;
			}

			for (var dir in dirs) {
				let target_position = move_origin + DIRS[dir];
				if (not is_valid(target_position)) continue;

				let target_piece = board->pieces[target_position];

				// if target square is friendly
				if (target_piece.piececolor == board->current_turn) {
					continue;
				}

				let moved_support = board->get_neighbor(move_origin, opposite(dir));


				// if target square is empty
				if (target_piece.piececolor == EMPTY) {

					// all strait moves
					for (int i in range(moved_support + 1)) {
						var move = movedata(board->current_turn, point{ x, y }, dir, i);
						moves.push_back(move);
					}

					// all broadside moves
					for (var pull_dir in half_dirs) {

						if (pull_dir == dir || pull_dir == opposite(dir)) {
							continue;
						}



						let target_neighbors = board->get_neighbor(target_position, pull_dir);
						let moved_neighbors = board->get_neighbor(move_origin, pull_dir);
						let min_neighbors = min(moved_neighbors, target_neighbors);

						for (int i in range(min_neighbors)) {
							var move = movedata(board->current_turn, point{ x, y }, dir, i + 1, pull_dir);
							moves.push_back(move);
						}
					}

				}

				// if target square is enemy
				if (target_piece.piececolor == opposite(board->current_turn)) {
					let target_neighbors = board->get_neighbor(target_position, dir);
					let strength_difference = moved_support - target_neighbors;

					// if the target is an enemy, we must overpower by one or two
					if (strength_difference >= 1) {
						var pushed_to_position = target_position + DIRS[dir] * (target_neighbors + 1);
						var capture = not is_valid(pushed_to_position);

						var valid_push = capture || (board->pieces[pushed_to_position].piececolor == EMPTY);
						if (valid_push) {

							var move = movedata(board->current_turn, point{ x, y }, dir, target_neighbors + 1, capture, target_neighbors + 1);
							moves.push_back(move);
						}
					}

					if (strength_difference >= 2) {
						var pushed_to_position = target_position + DIRS[dir] * (target_neighbors + 1);
						var capture = not is_valid(pushed_to_position);

						var valid_push = capture || (board->pieces[pushed_to_position].piececolor == EMPTY);
						if (valid_push) {

							var move = movedata(board->current_turn, point{ x, y }, dir, target_neighbors + 2, capture, target_neighbors + 1);
							moves.push_back(move);
						}
					}
				}
			}
		};

		// actual code execution begins here
		for (int x in range(9)) {
			for (int y in range(9)) {
				generate_for_target_position(x, y);
			}
		}

		// best moves are executed first for alpha beta pruning optimisation 
		sort(moves.begin(), moves.end(), better_move);
	}

	func next() -> movedata {
		var size = moves.size();
		if (size == 0 || picked_moves++ == 20) return NONE_MOVE;

		var temp = moves[size - 1];
		moves.pop_back();
		return temp;

	}

	func& random() {
		var random = rand() % moves.size();
		return moves[random];
	}
};


func board::evaluate() -> int {
	func positional_score = lambda() {
		var score = 0;
		for (int x in range(9)) {
			for (int y in range(9)) {
				score += pieces[x][y].piececolor * SCORE_MAP[x][y];
			}
		}
		return score;
	};

	func neighbor_score = lambda() {
		var score = 0;
		for (int x in range(9)) {
			for (int y in range(9)) {
				for (var dir in dirs) {
					score += pieces[x][y].piececolor * get_neighbor(point{ x, y }, dir);
				}
			}
		}
		return score;
	};

	func captured_score = lambda() {
		if (captured_white_pieces == 6) {
			return  1000000;
		}
		if (captured_black_pieces == 6) {
			return -1000000;
		}

		return 30 * (captured_black_pieces - captured_white_pieces);
	};

	return positional_score() + neighbor_score() + captured_score();

}

// does the given move. assumes the given move was legal
func board::make_move(movedata& move) {

	let strait_move = (opposite(move.direction) == move.pulled_direction);
	if (strait_move) {
		// origin of the move
		let origin_position = move.origin;
		let origin_piece = pieces[origin_position];

		// what position/piece is actually moved
		let moved_position = move.origin - DIRS[move.direction] * move.pulled_neighbors;
		let moved_piece = pieces[moved_position];

		// what position/piece is moved to
		let target_position = move.origin + DIRS[move.direction];
		let target_piece = pieces[target_position];
		let target_empty = (target_piece.piececolor == 0);


		if (target_empty) {

			pieces[target_position] = moved_piece;
			update_hash(target_position, move.piececolor);
			dirty_neighbors(target_position);

			dirty_neighbors(moved_position);
			update_hash(moved_position, move.piececolor);
			pieces[moved_position] = EMPTY_SPACE;
		}

		else /* target not empty */ {

			let pushed_enemies = move.pushed_enemies;
			let displace_position = move.origin + DIRS[move.direction] * (pushed_enemies + 1);

			let is_a_capture = not is_valid(displace_position);
			if (is_a_capture) {
				if (move.piececolor == WHITE) {
					captured_black_pieces += 1;
				}
				if (move.piececolor == BLACK) {
					captured_white_pieces += 1;
				}
			}
			else {
				pieces[displace_position] = target_piece;
				update_hash(displace_position, opposite(move.piececolor));
				dirty_neighbors(displace_position);
			}

			dirty_neighbors(target_position);
			update_hash(target_position, opposite(move.piececolor));
			pieces[target_position] = moved_piece;
			update_hash(target_position, move.piececolor);
			dirty_neighbors(target_position);

			dirty_neighbors(moved_position);
			update_hash(moved_position, move.piececolor);
			pieces[moved_position] = EMPTY_SPACE;
		}
	}
	else /* move is not strait */ {
		for (int i in range(move.pulled_neighbors + 1)) {
			let moved_position = move.origin + DIRS[move.pulled_direction] * i;
			let moved_piece = pieces[moved_position];

			let target_position = moved_position + DIRS[move.direction];

			pieces[target_position] = moved_piece;
			update_hash(target_position, move.piececolor);
			dirty_neighbors(target_position);


			dirty_neighbors(moved_position);
			update_hash(moved_position, move.piececolor);
			pieces[moved_position] = EMPTY_SPACE;
		}
	}

	current_turn = opposite(current_turn);

}


func board::undo_move(movedata move) {

	let strait_move = (opposite(move.direction) == move.pulled_direction);
	if (strait_move) {

		let no_push_move = (move.pushed_enemies == 0);
		let single_push_and_capture = (move.pushed_enemies == 1 && move.captured_enemy);
		if (no_push_move || single_push_and_capture) {
			var reverse_move = move;

			reverse_move.direction = move.pulled_direction;
			reverse_move.pulled_direction = move.direction;

			reverse_move.origin = move.origin + DIRS[move.direction] * (1 - move.pulled_neighbors);


			make_move(reverse_move);

			if (move.captured_enemy) {
				let retured_position = move.origin + DIRS[move.direction];

				if (move.piececolor == WHITE) {
					captured_black_pieces -= 1;
					pieces[retured_position] = BLACK_PIECE;
				}
				if (move.piececolor == BLACK) {
					captured_white_pieces -= 1;
					pieces[retured_position] = WHITE_PIECE;
				}

			}

			return;

		}
		else /* was a pushing move */ {
			var reverse_move = move;

			reverse_move.direction = move.pulled_direction;
			reverse_move.pulled_direction = move.direction;

			reverse_move.origin = move.origin + DIRS[move.direction] * 2;

			reverse_move.pulled_neighbors = move.pushed_enemies - 1 - move.captured_enemy;
			reverse_move.pushed_enemies = move.pulled_neighbors + 1;

			reverse_move.piececolor = opposite(move.piececolor);


			make_move(reverse_move);

			if (move.captured_enemy) {
				let retured_position = move.origin + DIRS[move.direction] * (move.pushed_enemies);

				if (move.piececolor == WHITE) {
					captured_black_pieces -= 1;
					pieces[retured_position] = BLACK_PIECE;
				}
				if (move.piececolor == BLACK) {
					captured_white_pieces -= 1;
					pieces[retured_position] = WHITE_PIECE;
				}

			}

			return;

		}

	}
	else /* was a broadside move */ {
		var reverse_move = move;
		reverse_move.origin = move.origin + DIRS[move.direction];
		reverse_move.direction = opposite(move.direction);


		make_move(reverse_move);
		return;
	}

}

// simple minimax with alpha beta pruning
func board::search(int alpha, int beta, int depthleft) -> int {

	if (depthleft == 0) {
		return evaluate();
	}

	var score = 0;
	var move = movedata();
	var movepick = movegen(this);

	while ((move = movepick.next()).is_valid()) {

		make_move(move);

		if (position_is_transposition()) {
			score = transposition_value();
		}
		else /*first time position is searched */ {
			score = -search(-beta, -alpha, depthleft - 1);
		}

		undo_move(move);


		// beta cutoff
		if (score > beta) {
			return beta;
		}
		// alpha improvement
		if (score > alpha) {
			alpha = score;
		}
	}

	add_to_transposition_table(alpha);
	return alpha;
}


// simple minimax for the root move
func board::find_best(int maxdepth) -> movedata {

	var move = movedata();
	var bestmove = movedata();
	var movepick = movegen(this);
	var bestscore = INT_MIN;

	while ((move = movepick.next()).is_valid()) {

		make_move(move);

		var score = -search(INT_MAX, INT_MAX, maxdepth - 1);

		if (bestscore < score) {
			bestscore = score;
			bestmove = move;
		}

		undo_move(move);
	}

	return bestmove;
}


func board::find_random() -> movedata {
	var movepick = movegen(this);
	var random_move = movepick.random();
	return random_move;
}

func board::find_best_or_random() -> movedata {
	if (current_turn == WHITE) {
		return find_random();
	}
	if (current_turn == BLACK) {
		return find_best(5);
	}
}




func main() -> int {

	var board = parse_to_board(STARTING_BOARD);

	for (int i in range(100)) {

		// Black players turn. My AI
		var move = board.find_best(5);
		print("After Blacks turn: \n ");
		board.make_move(move);
		board.print_board();
		print(""); print("");

		if (board.black_won()) {
			print("Black Wins");
			return 0;
		}

		this_thread::sleep_for(chrono::seconds(3));



		// White players turn. Random player
		var random = board.find_random();
		board.make_move(random);
		print("After Whites turn: \n ");
		board.print_board();
		print(""); print("");

		if (board.white_won()) {
			print("White Wins");
			return 0;
		}

		this_thread::sleep_for(chrono::seconds(3));

	}

	print("Draw. Nobody wins");
	return 0;
}







