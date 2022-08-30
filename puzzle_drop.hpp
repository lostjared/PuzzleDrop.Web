// just practicing

#ifndef _PUZZLE_DROP_H_
#define _PUZZLE_DROP_H_

#include<iostream>
#include<string>

namespace puzzle {

    enum class BlockType { BLOCK_NULL=0, BLOCK_CLEAR, RED1, RED2, RED3, GREEN1, GREEN2, GREEN3, BLUE1, BLUE2, BLUE3, MATCH };

    class Piece;

    class Block {
        int x,y;
        BlockType type;
        int clear;
    public:
        Block();
        Block(const Block &b);
        Block(Block &&b) = delete;
        Block &operator=(const Block &b);
        Block &operator=(Block &&b);
        Block &operator=(const BlockType &t);
        Block(int xx, int yy, BlockType t);
        int getX() const;
        int getY() const;
        BlockType getType() const;
        bool operator==(const Block &b);
        bool operator==(const BlockType &type);
        int &clearValue();
        void clearBlock();
        friend class Piece;
        friend std::ostream &operator<<(std::ostream &out, Block &b);
    };

    enum Direction { D_DOWN=1, D_UP =2 };

    class Piece {
    public:
        Piece();
        Piece(const Piece &p);
        Piece(Piece &&p);
        Piece &operator=(const Piece &p);
        Piece &operator=(Piece &&p);
        void newPiece(int start_x, int start_y);
        void randColors();
        void moveLeft();
        void moveRight();
        void moveDown();
        void rotateLeft();
        void rotateRight();
        void shift(Direction dir);
        int pos() const;
        Block blocks[3];
        friend std::ostream &operator<<(std::ostream &out, Piece &p);
    private:
        int position;
    };

    class Grid {
    public:
        Grid(int w, int h);
        ~Grid();
        void clearGrid();
        Block *grid(int x, int y);
        int grid_int(int x, int y);
        int block_pos(int num);
        int block_getx(int num);
        int block_gety(int num);
        int getWidth() const;
        int getHeight() const;
        Piece &getPiece();
        void keyLeft();
        void keyRight();
        void keyDown();
        void keyShiftUp();
        void keyShiftDown();
        void keyRotateLeft();
        void keyRotateRight();
        int level() const;
        int currentLines() const;
        void levelNext();
        bool procMoveDown();
        bool procBlocks();
        bool gameOver() const;
        void init_rand(int n);
    protected:
        Block **blocks;
        Piece piece;
        int grid_w, grid_h;
        int game_level;
        int lines;
        bool game_over;
        bool checkPiece(Piece &p, int x, int y);
        void setPiece();
        bool checkBlock(int x, int y, const BlockType &type);
        void addScore();
    };

}


#endif

