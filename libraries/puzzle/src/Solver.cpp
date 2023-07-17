#include "puzzle/Solver.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <queue>
#include <utility>

struct Node_coord {
    std::size_t prev_x{};
    std::size_t prev_y{};
    unsigned distance = UINT_MAX;

    using Coordinate = std::pair<std::size_t, std::size_t>;

    Node_coord() = default;

    Node_coord(const Coordinate& coordinate) : prev_x(coordinate.first), prev_y(coordinate.second) {}
};

struct Node {
    unsigned move{};
    unsigned eur{};
    Board board{};

    friend bool operator<(const Node& lhs, const Node& rhs) {
        if (rhs.move + rhs.eur == lhs.move + lhs.eur) {
            return lhs.move < rhs.move;
        }
        return (rhs.move + rhs.eur) < (lhs.move + lhs.eur);
    }

    Node(unsigned move, const Board& board) : move(move), eur(board.manhattan()), board(board) {
        unsigned hamming       = board.hamming();
        std::size_t half_board = board.size() * board.size() / 2;

        if (board.size() >= 5) {
            if (hamming >= half_board) {
                eur *= 2 * board.size();
            } else if (hamming >= 3 * board.size() / 2) {
                eur *= std::max(2.0 * atan(hamming), board.size() / 2.0) * sqrt(hamming);
            }
            eur *= std::max(static_cast<double>(hamming), 1.0);
        } else {
            if (hamming > half_board) {
                eur *= 2;
            } else if (hamming > 3 * board.size() / 2) {
                eur *= 1 + static_cast<double>(hamming) / (board.size() * board.size());
            }
        }
    }
};

Solver::Solution Solver::solve(const Board& initial) {
    if (initial.size() < 2 || initial.is_goal()) {
        return Solution({initial});
    } else if (!initial.is_solvable()) {
        return {};
    }

    std::priority_queue<Node> node_queue;
    node_queue.emplace(0, initial);

    std::map<Board, Node_coord> node_coord_map{};
    node_coord_map[initial] = {initial.board_free_space};

    while (!node_queue.empty()) {
        auto eur = node_queue.top().eur;
        if (eur == 0) {
            break;
        }
        auto moves = node_queue.top().move;
        auto board = node_queue.top().board;
        node_queue.pop();

        for (auto& new_board : board.move()) {
            auto& [new_x, new_y, other_dist] = node_coord_map[new_board];
            if (other_dist - 1 > moves) {
                other_dist             = moves + 1;
                std::tie(new_x, new_y) = board.board_free_space;
                node_queue.emplace(moves + 1, std::move(new_board));
            }
        }
    }

    std::vector<Board> result;
    auto move  = node_queue.top().move;
    auto board = node_queue.top().board;
    auto current_board(board.board_cells);
    std::size_t x = board.size() - 1;
    std::size_t y = board.size() - 1;

    for (unsigned i = 0; i < move + 1; ++i) {
        Board temp(current_board);
        auto [nx, ny, nodeDist] = node_coord_map[temp];
        result.push_back(std::move(temp));
        std::swap(current_board[x][y], current_board[nx][ny]);
        std::tie(x, y) = std::tie(nx, ny);
    }

    std::reverse(result.begin(), result.end());
    return Solution(std::move(result));
}
