#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class TicTacToe : public olc::PixelGameEngine
{
private:
	std::vector<int>
		game_board{ 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		select_map{ 0, 0, 0, 0, 1, 0, 0, 0, 0 };
	int
		human_player = 2,
		robot_player = 1,

		x_score = 0,
		o_score = 0,

		box_size, board_size,
		margin_w, tile_size;

	void DrawGrid()
	{
		Clear(olc::BLACK);

		DrawLine({ margin_w + box_size / 3, 0 }, { margin_w + box_size / 3, board_size });
		DrawLine({ margin_w + box_size / 3 * 2, 0 }, { margin_w + box_size / 3 * 2, board_size });

		DrawLine({ margin_w, box_size / 3 }, { box_size + margin_w - 1, box_size / 3 });
		DrawLine({ margin_w, box_size / 3 * 2 }, { box_size + margin_w - 1, box_size / 3 * 2 });
	}

	void DrawMarkers()
	{
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
			{
				int index = i * 3 + j;
				if (game_board[index])
				{
					static const int marker_size = tile_size / 3;
					olc::vi2d pos = { margin_w + j * tile_size, i * tile_size };

					if (game_board[index] == 1)
					{
						DrawLine(
							{ pos.x + marker_size, pos.y + marker_size },
							{ pos.x + tile_size - marker_size, pos.y + tile_size - marker_size },
							olc::RED);
						DrawLine(
							{ pos.x + tile_size - marker_size, pos.y + marker_size },
							{ pos.x + marker_size, pos.y + tile_size - marker_size },
							olc::RED);
					}
					if (game_board[index] == 2)
						DrawCircle({ pos.x + tile_size / 2, pos.y + tile_size / 2 }, marker_size / 2, olc::BLUE);
				}
			}
		}
	}

	void DrawSelectBox()
	{
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
			{
				int index = i * 3 + j;
				if (select_map[index])
				{
					olc::vi2d pos = { margin_w + j * tile_size, i * tile_size };
					olc::Pixel col = olc::RED;

					if (human_player == 2) col = olc::BLUE;
					DrawRect({ pos.x, pos.y }, { tile_size, tile_size }, col);
				}
			}
		}
	}

	std::vector<int> RotateTiles(
		int dx, int dy,
		const std::vector<int>& tile_map,
		const std::vector<int>& indices)
	{
		std::vector<int> new_tile_map(tile_map.size());
		for (size_t i = 0; i < indices.size(); i++)
		{
			int x = indices[i] % 3,
				y = indices[i] / 3,

				new_x = (x + dx + 3) % 3,
				new_y = (y + dy + 3) % 3;

			new_tile_map[static_cast<size_t>(new_y) * 3 + static_cast<size_t>(new_x)] =
				tile_map[static_cast<size_t>(y) * 3 + static_cast<size_t>(x)];
		}
		return new_tile_map;
	}

	void MoveSelectBox()
	{
		if (GetKey(olc::Key::UP).bPressed || GetKey(olc::Key::W).bPressed)
			select_map = RotateTiles(0, -1, select_map, { 0, 1, 2, 3, 4, 5, 6, 7, 8 });
		if (GetKey(olc::Key::LEFT).bPressed || GetKey(olc::Key::A).bPressed)
			select_map = RotateTiles(-1, 0, select_map, { 0, 1, 2, 3, 4, 5, 6, 7, 8 });
		if (GetKey(olc::Key::DOWN).bPressed || GetKey(olc::Key::S).bPressed)
			select_map = RotateTiles(0, 1, select_map, { 0, 1, 2, 3, 4, 5, 6, 7, 8 });
		if (GetKey(olc::Key::RIGHT).bPressed || GetKey(olc::Key::D).bPressed)
			select_map = RotateTiles(1, 0, select_map, { 0, 1, 2, 3, 4, 5, 6, 7, 8 });
	}

	void DrawScores()
	{
		static const int pos = margin_w / 6;
		std::string score_str = "X:" + std::to_string(x_score);

		DrawString({ 1, pos }, score_str);
		score_str = "O:" + std::to_string(o_score);
		DrawString({ 1, pos * 2 }, score_str);
	}

	int isGameOver(std::vector<int> map) {
		std::vector<std::vector<int>> win_conds
		{
			{0, 1, 2}, {3, 4, 5}, {6, 7, 8},
			{0, 3, 6}, {1, 4, 7}, {2, 5, 8},
			{0, 4, 8}, {2, 4, 6}
		};

		int winner = 0;
		for (const auto& win : win_conds) {
			int cell = map[win[0]];
			if (
				cell == map[win[1]] &&
				cell == map[win[2]] &&
				cell != 0)
			{
				winner = cell;
				break;
			}
		}

		if (winner != 0) return winner;
		else if (std::find(map.begin(), map.end(), 0) == map.end())
			return -1;
		else return 0;
	}

	int EvaluateBoard(std::vector<int> game_board)
	{
		int winner = isGameOver(game_board);

		if (winner == robot_player) return 10;
		else if (winner == human_player) return -10;
		else return 0;
	}

	int Minimax(std::vector<int> game_board, int depth, bool maximize)
	{
		int best_score, score;
		if (isGameOver(game_board))
		{
			score = EvaluateBoard(game_board);

			if (score == 10) return score;
			else if (score == -10) return score;
			else return 0;
		}

		if (maximize)
		{
			best_score = INT_MIN;
			for (int i = 0; i < game_board.size(); i++)
			{
				if (!game_board[i])
				{
					game_board[i] = robot_player;
					best_score = std::max(best_score, Minimax(game_board, depth + 1, !maximize));
					game_board[i] = 0;
				}
			}
		}

		else
		{
			best_score = INT_MAX;
			for (int i = 0; i < game_board.size(); i++)
			{
				if (!game_board[i])
				{
					game_board[i] = human_player;
					best_score = std::min(best_score, Minimax(game_board, depth + 1, !maximize));
					game_board[i] = 0;
				}
			}
		}
		return best_score;
	}

	int BestMove()
	{
		int best_score = INT_MIN;
		int best_move = 0;

		for (int i = 0; i < game_board.size(); i++)
		{
			if (!game_board[i])
			{
				game_board[i] = robot_player;
				int score = Minimax(game_board, 0, false);
				if (score > best_score)
				{
					best_score = score;
					best_move = i;
				}
				game_board[i] = 0;
			}
		}
		return best_move;
	}

	void ComputerMove(bool& player_turn)
	{
		/*while (!player_turn)
		{
			int move = rand() % 9;
			if (!game_board[move]) 
			{
				game_board[move] = robot_player;
				player_turn = true;
			}
		}*/

		if (!player_turn)
		{
			game_board[BestMove()] = robot_player;
			player_turn = true;
		}
	}

	void HumanMove(bool& player_turn)
	{
		if (GetKey(olc::Key::ENTER).bPressed ||
			GetKey(olc::Key::SPACE).bPressed)
		{
			int tile = 0;
			for (int i = 0; i < select_map.size(); i++)
				if (select_map[i]) tile = i;

			if (game_board[tile] == 0) {
				game_board[tile] = human_player;
				player_turn = !player_turn;
			}
		}
	}

public:
	int board_w = 200,
		board_h = 100;

	TicTacToe() {
		sAppName = "TicTacToe";
	}

	bool OnUserCreate() override
	{
		board_size = ScreenHeight(),
			margin_w = (ScreenWidth() - board_size) / 2,
			box_size = ScreenWidth() - (margin_w * 2),
			tile_size = box_size / 3;
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		static bool player_turn = true;
		if (GetKey(olc::Key::ESCAPE).bPressed)
			return false;
		MoveSelectBox();

		if (!isGameOver(game_board))
		{
			if (player_turn) HumanMove(player_turn);
			else ComputerMove(player_turn);
		}

		else
		{
			int winner = isGameOver(game_board);
			if (winner == 1) x_score++;
			if (winner == 2) o_score++;

			game_board = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			game_board[rand() % 9] = robot_player;
			player_turn = true;
		}

		DrawGrid();
		DrawMarkers();
		DrawSelectBox();
		DrawScores();

		return true;
	}
};

int main()
{
	TicTacToe game;
	if (game.Construct(
		game.board_w,
		game.board_h,
		4, 4, true))
		game.Start();
	return 0;
}