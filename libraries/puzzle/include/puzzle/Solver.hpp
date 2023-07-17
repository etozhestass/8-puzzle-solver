#pragma once

#include "Board.hpp"

class Solver {
    class Solution {
    public:
        friend class Solver;

        std::size_t moves() const { return m_moves.empty() ? 0 : m_moves.size() - 1; }

        using const_iterator = std::vector<Board>::const_iterator;

        const_iterator begin() const { return m_moves.begin(); }

        const_iterator end() const { return m_moves.end(); }

        Solution(const Solution& solution) = default;

        Solution& operator=(const Solution& other) = default;

    private:
        std::vector<Board> m_moves;

        Solution() = default;

        Solution(std::vector<Board>&& moves) : m_moves(std::move(moves)) {}
    };

public:
    static Solution solve(const Board& initial);
};