#include "puzzle/Board.hpp"

#include <iostream>
#include <memory>
#include <numeric>
#include <random>

#include "algorithm"
Board::Board(std::size_t size) : board_cells(size, std::vector<unsigned>(size, 0)) {}

Board::Board(const std::vector<std::vector<unsigned>>& data) : board_cells(data) {
    for (std::size_t i = 0; i < data.size(); i++) {
        for (std::size_t j = 0; j < data.size(); j++) {
            if (!data[i][j]) {
                board_free_space = {i, j};
                return;
            }
        }
    }
}

Board Board::create_random(const unsigned size) {
    Board random_board(size);

    std::vector<unsigned> vec;
    vec.resize(size * size);
    std::iota(vec.begin(), vec.end(), 0);

    std::shuffle(vec.begin(), vec.end(), std::mt19937(std::random_device{}()));
    for (unsigned i = 0; i < size; i++) {
        for (unsigned j = 0; j < size; j++) {
            random_board.board_cells[i][j] = vec[i * size + j];
            if (vec[i * size + j] == 0) {
                random_board.board_free_space = {i, j};
            }
        }
    }

    return random_board;
}

std::size_t Board::size() const {
    return board_cells.size();
}

bool Board::is_goal() const {
    return !manhattan();
}

unsigned Board::hamming() const {
    unsigned res = 0;
    return get_answer_by_table<unsigned>(res, hamming_calc);
}
unsigned Board::manhattan() const {
    unsigned res = 0;
    return get_answer_by_table<unsigned>(res, manhattan_calc);
}

std::pair<unsigned, unsigned> Board::hamming_manhattan() {
    std::pair<unsigned, unsigned> res(0, 0);
    return get_answer_by_table(res, hamming_manhattan_calc);
}

bool Board::is_manhattan(const Board& board, size_t row, size_t column) {
    return board.board_cells[row][column] != 0;
}

template <class T>
T& Board::get_answer_by_table(T& result, void (*f)(T&, const Board&, std::size_t, std::size_t)) const {
    std::size_t size_board = size();
    for (unsigned i = 0; i < size_board; ++i) {
        for (unsigned j = 0; j < size_board; ++j) {
            f(result, *this, i, j);
        }
    }
    return result;
}
std::string Board::to_string() const {
    std::string str;
    return get_answer_by_table<std::string>(str, string_build);
}
bool Board::is_solvable() const {
    int flag                      = 0;
    std::size_t inversion_counter = 0;
    std::vector<unsigned> count_inversions;
    unsigned board_size = size();
    if (board_size < 2) {
        return true;
    }
    count_inversions.resize(board_size * board_size);
    for (unsigned i = 0; i < board_size; i++) {
        for (unsigned j = 0; j < board_size; j++) {
            if (board_cells[i][j] != 0) {
                count_inversions[i * board_size + j + flag] = board_cells[i][j] ? board_cells[i][j] : -1;
            } else {
                flag = -1;
            }
        }
    }
    for (unsigned i = 0; i < count_inversions.size() - 2; i++) {
        for (unsigned j = i + 1; j < count_inversions.size() - 1; j++) {
            if (count_inversions[i] > count_inversions[j]) {
                inversion_counter++;
            }
        }
    }
    return (inversion_counter + (size() % 2 == 0 ? board_free_space.first + 1 : 0)) % 2 == 0;
}
bool Board::is_hamming(const Board& board, std::size_t row, std::size_t column) {
    return board.board_cells[row][column] != (row * board.size() + column + 1) % (board.size() * board.size());
}
void Board::hamming_calc(unsigned int& res, const Board& board, size_t row, size_t column) {
    if (is_hamming(board, row, column)) {
        res++;
    }
}

void Board::manhattan_calc(unsigned& res, const Board& board, std::size_t row, std::size_t column) {
    if (is_manhattan(board, row, column)) {
        std::size_t goal_y = ((board.board_cells[row][column] % board.size()) + board.size() - 1) % board.size();
        std::size_t goal_x = (board.board_cells[row][column] - 1) / board.size();
        res += std::abs(static_cast<int>(goal_x - row)) + std::abs(static_cast<int>(goal_y - column));
    }
}
void Board::hamming_manhattan_calc(std::pair<unsigned, unsigned>& res, const Board& board, size_t row, size_t column) {
    hamming_calc(res.first, board, row, column);
    manhattan_calc(res.second, board, row, column);
}
void Board::string_build(std::string& res, const Board& board, std::size_t row, std::size_t column) {
    res.append(std::to_string(board.board_cells[row][column]).append(" "));
    if (column == board.size() - 1) {
        res.append("\n");
    }
}
bool Board::valid_move(direction dir) const {
    if (((dir == direction::left) && (board_free_space.second > 0)) ||
        ((dir == direction::right) && (board_free_space.second < size() - 1)) ||
        ((dir == direction::up) && (board_free_space.first > 0)) ||
        ((dir == direction::down) && (board_free_space.first < size() - 1))) {
        return true;
    }
    return false;
}
Board Board::move(direction dir) const {
    Board boardAfterMove = *this;
    if (valid_move(dir) && dir != inaction) {
        std::vector<int> shift = index[dir - 1];
        boardAfterMove.board_cells[board_free_space.first][board_free_space.second] =
            boardAfterMove.board_cells[board_free_space.first + shift[0]][board_free_space.second + shift[1]];
        boardAfterMove.board_cells[board_free_space.first + shift[2]][board_free_space.second + shift[3]] = 0;
        boardAfterMove.board_free_space = {board_free_space.first + shift[4], board_free_space.second + shift[5]};
    }
    return boardAfterMove;
}
std::vector<Board> Board::move() const {
    return {move(Board::inaction), move(Board::left), move(Board::right), move(Board::up), move(Board::down)};
}