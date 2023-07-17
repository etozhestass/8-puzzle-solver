#ifndef PUZZLE_BOARD_HPP
#define PUZZLE_BOARD_HPP

#include <memory>
#include <string>
#include <vector>

class Board {
public:
    static Board create_random(unsigned size);

    Board() = default;

    explicit Board(std::size_t size);

    explicit Board(const std::vector<std::vector<unsigned>>& data);

    [[nodiscard]] std::size_t size() const;

    [[nodiscard]] bool is_goal() const;

    [[nodiscard]] unsigned hamming() const;
    [[nodiscard]] unsigned manhattan() const;

    [[nodiscard]] std::pair<unsigned int, unsigned int> hamming_manhattan();
    [[nodiscard]] std::string to_string() const;

    [[nodiscard]] bool is_solvable() const;

    friend bool operator==(const Board& lhs, const Board& rhs) { return lhs.board_cells == rhs.board_cells; }

    friend bool operator!=(const Board& lhs, const Board& rhs) { return !(lhs == rhs); }

    friend std::ostream& operator<<(std::ostream& out, const Board& board) { return out << board.to_string(); }

    std::vector<unsigned>& operator[](const unsigned rhs) { return board_cells[rhs]; }
    const std::vector<unsigned>& operator[](const unsigned rhs) const { return board_cells[rhs]; }

    friend bool operator<(const Board& lhs, const Board& rhs) { return lhs.board_cells < rhs.board_cells; }
    enum direction { inaction, left, right, up, down };
    [[nodiscard]] bool valid_move(direction dir) const;

    [[nodiscard]] Board move(direction dir) const;

    std::shared_ptr<Board> parent = nullptr;

    std::vector<std::vector<unsigned>> board_cells;

    std::pair<std::size_t, std::size_t> board_free_space;

    std::vector<Board> move() const;

protected:
    std::vector<std::vector<int>> index = {
        {0, -1, 0, -1, 0, -1}, {0, 1, 0, 1, 0, 1}, {-1, 0, -1, 0, -1, 0}, {1, 0, 1, 0, 1, 0}};
    template <class T>
    T& get_answer_by_table(T& result, void (*f)(T&, const Board& board, std::size_t row, std::size_t column)) const;
    static void string_build(std::string& res, const Board& board, std::size_t row, std::size_t column);
    static void manhattan_calc(unsigned& res, const Board& board, std::size_t row, std::size_t column);
    static void hamming_calc(unsigned& res, const Board& board, size_t row, size_t column);
    static void hamming_manhattan_calc(std::pair<unsigned, unsigned>& res, const Board& board, size_t row,
                                       size_t column);
    static bool is_hamming(const Board& board, size_t row, size_t column);
    static bool is_manhattan(const Board& board, size_t row, size_t column);
};

#endif  // PUZZLE_BOARD_HPP