#include"puzzle_drop.hpp"
#include<emscripten.h>
#include<emscripten/bind.h>

namespace puzzle {
// *** Block
    Block::Block() : x{0}, y{0}, type{BlockType::BLOCK_NULL}, clear{0} {}

    Block::Block(int xx, int yy, BlockType t) : x{xx}, y{yy}, type{t}, clear{0} {}

    Block::Block(const Block &b) : x{b.x}, y{b.y}, type{b.type}, clear{b.clear} {}
    
    Block &Block::operator=(const Block &b) {
        x = b.x;
        y = b.y;
        type = b.type;
        clear = b.clear;
        return *this;
    }

    Block &Block::operator=(Block &&b) {
        x = b.x;
        y = b.y;
        type = b.type;
        clear = b.clear;
        return *this;
    }

    Block &Block::operator=(const BlockType &t) {
        type = t;
        return *this;
    }


    bool Block::operator==(const Block &b) {
        if(type == b.type)
            return true;
        return false;
    }

    bool Block::operator==(const BlockType &t) {
        if(type == t)
            return true;
        return false;
    }

    int Block::getX() const {
        return x;
    }
    
    int Block::getY() const {
        return y;
    }
    
    BlockType Block::getType() const {
        return type;
    }

    int &Block::clearValue() {
        return clear;
    }

    void Block::clearBlock() {
        type = BlockType::BLOCK_CLEAR;
        clear = 1;
    }

    std::ostream &operator<<(std::ostream &out, Block &b) {
        out << "Block [ x:" << b.x << " y: " << b.y << "]  ";
        return out;
    }

// *** Piece

    Piece::Piece() {

    }

    Piece::Piece(const Piece &p) {
        blocks[0] = p.blocks[0];
        blocks[1] = p.blocks[1];
        blocks[2] = p.blocks[2];
        position = p.position;
    }

    Piece::Piece(Piece &&p) {
        blocks[0] = p.blocks[0];
        blocks[1] = p.blocks[1];
        blocks[2] = p.blocks[2];
        position = p.position;
    }
    
    Piece &Piece::operator=(const Piece &p) {
        blocks[0] = p.blocks[0];
        blocks[1] = p.blocks[1];
        blocks[2] = p.blocks[2];
        position = p.position;
        return *this;
    }
    
    Piece &Piece::operator=(Piece &&p) {
        blocks[0] = p.blocks[0];
        blocks[1] = p.blocks[1];
        blocks[2] = p.blocks[2];
        position = p.position;
        return *this;
    }

    void Piece::newPiece(int start_x, int start_y) {
        blocks[0].x = start_x;
        blocks[0].y = start_y;
        blocks[1].x = start_x;
        blocks[1].y = start_y+1;
        blocks[2].x = start_x;
        blocks[2].y = start_y+2;
        position = 0;
        randColors();
    }

    void Piece::randColors() {
        blocks[0].type = static_cast<BlockType>(2+rand()%10);
        blocks[1].type = static_cast<BlockType>(2+rand()%10);
        blocks[2].type = static_cast<BlockType>(2+rand()%10);
    }

    void Piece::shift(Direction dir) {
        BlockType b[3];
        b[0] = blocks[0].type;
        b[1] = blocks[1].type;
        b[2] = blocks[2].type;
        if(dir == D_DOWN) {
            blocks[0].type = b[2];
            blocks[1].type = b[0];
            blocks[2].type = b[1];
        } else if(dir == D_UP) {
            blocks[0] = b[1];
            blocks[1] = b[2];
            blocks[2] = b[0];
        }
    }

    void Piece::moveLeft() {
        for(int i = 0; i < 3; ++i)
            blocks[i].x--;
    }
    void Piece::moveRight() {
        for(int i = 0; i < 3; ++i)
            blocks[i].x++;
    }
    void Piece::moveDown() {
        for(int i = 0; i < 3; ++i)
            blocks[i].y++;
    }

    void Piece::rotateLeft() {
        if(position == 0) {
            blocks[1].y -= 1;
            blocks[1].x -= 1;
            blocks[2].x -= 2;
            blocks[2].y -= 2;
            position = 1;
        } else if(position == 1) {
            blocks[1].y += 1;
            blocks[1].x += 1;
            blocks[2].y += 2;
            blocks[2].x += 2;
            position = 0;
        }
    }
    
    void Piece::rotateRight() {
        if(position == 0) {
            blocks[1].x += 1;
            blocks[1].y -= 1;
            blocks[2].x += 2;
            blocks[2].y -= 2;
            position = 2;
        } else if(position == 2) {
            blocks[1].x -= 1;
            blocks[1].y += 1;
            blocks[2].x -= 2;
            blocks[2].y += 2;
            position = 0;
        }
    }

    int Piece::pos() const {
        return position;
    }


    std::ostream &operator<<(std::ostream &out, Piece &p) {
        out << "Piece: " << p.blocks[0] << p.blocks[1] << p.blocks[2] << "\n";
        return out;
    }

// *** Grid

    Grid::Grid(int w, int h) : grid_w{w}, grid_h{h} {
        srand(static_cast<unsigned int>(time(0)));
        blocks = new Block*[w];
        for(int i = 0; i < grid_w; ++i) {
            blocks[i] = new Block[h];
        }
        clearGrid();
        piece.newPiece(w/2, 0);

    }

    void Grid::clearGrid() {
        for(int i = 0; i < getWidth(); ++i) {
            for(int z = 0; z < getHeight(); ++z) {
                blocks[i][z] = Block(i, z, BlockType::BLOCK_NULL);
            }
        }
        game_level = 1;
        lines = 0;
        game_over = false;
        clearPiece();
    }

    void Grid::clearPiece() {
        piece.newPiece(grid_w/2, 0);
    }
    
    Grid::~Grid() {
        if(blocks != nullptr) {
            for(int i = 0; i < getWidth(); ++i)
                delete [] blocks[i];
            delete [] blocks;
        }
    }

    int Grid::getWidth() const {
        return grid_w;
    }
    
    int Grid::getHeight() const {
        return grid_h;
    }

    Block *Grid::grid(int x, int y) {
        if(x >= 0 && x < grid_w && y >= 0 && y < grid_h) {
            return &blocks[x][y];
        }
        return nullptr;
    }

    int Grid::grid_int(int x, int y) {
        if(x >= 0 && x < grid_w && y >= 0 && y < grid_h) {
            return static_cast<int>(blocks[x][y].getType());
        }
        return -1;
    }



    Piece &Grid::getPiece() {
        return piece;
    }

    void Grid::keyLeft() {
        if(checkPiece(piece, -1, 0)) {
            piece.moveLeft();
        }
    }
    void Grid::keyRight() {
        if(checkPiece(piece, 1, 0)) {
            piece.moveRight();
        }
    }

    void Grid::keyDown() {
        if(checkPiece(piece, 0, 1)) {
            piece.moveDown();
        } else {
            setPiece();
            piece.newPiece(grid_w/2, 0);
        }
    }

    void Grid::keyRotateLeft() {
        Piece test_piece(piece);
        test_piece.rotateLeft();
        if(checkPiece(test_piece, 0, 0)) {
            piece.rotateLeft();
        }
    }
    
    void Grid::keyRotateRight() {
        Piece test_piece(piece);
        test_piece.rotateRight();
        if(checkPiece(test_piece, 0, 0)) {
            piece.rotateRight();
        }
    }


    void Grid::keyShiftUp() {
        piece.shift(D_UP);
    }

    void Grid::keyShiftDown() {
        piece.shift(D_DOWN);
    }

    bool Grid::checkPiece(Piece &p, int x, int y) {
        for(int i = 0; i < 3; ++i) {
            Block *b = grid(p.blocks[i].getX()+x, p.blocks[i].getY()+y);
            if(b == nullptr || (b->getType() != BlockType::BLOCK_NULL && b->getType() != BlockType::BLOCK_CLEAR)) {
                return false;
            }
        }
        return true;
    }

    void Grid::setPiece() {
        for(int i = 0; i < 3; ++i) {
            int px = piece.blocks[i].getX();
            int py = piece.blocks[i].getY();
            Block *b = grid(px, py);
            if(b != nullptr) {
                *b = piece.blocks[i].getType();
            }
            if(py == 0) {
                game_over = true;
            }
        }

        
    }

    bool Grid::procBlocks() {
        for(int x = 0; x < grid_w; ++x) {
            for(int y = 0; y < grid_h; ++y) {

               if(checkBlock(x,y,BlockType::RED1) && checkBlock(x,y+1,BlockType::RED2) && checkBlock(x,y+2,BlockType::RED3)) {
                   addScore();
                   blocks[x][y].clearBlock();
                   blocks[x][y+1].clearBlock();
                   blocks[x][y+2].clearBlock();
                   return true;
               }

                if(checkBlock(x,y,BlockType::RED3) && checkBlock(x,y+1,BlockType::RED2) && checkBlock(x,y+2,BlockType::RED1)) {
                    addScore();
                    blocks[x][y].clearBlock();
                     blocks[x][y+1].clearBlock();
                    blocks[x][y+2].clearBlock();
                    return true;
                }

                if(checkBlock(x,y,BlockType::GREEN1) && checkBlock(x,y+1,BlockType::GREEN2) && checkBlock(x,y+2,BlockType::GREEN3)) {
                       addScore();
                        blocks[x][y].clearBlock();
                        blocks[x][y+1].clearBlock();
                        blocks[x][y+2].clearBlock();
                        return true;
                }

                if(checkBlock(x,y,BlockType::GREEN3) && checkBlock(x,y+1,BlockType::GREEN2) && checkBlock(x,y+2,BlockType::GREEN1)) {
                       addScore();
                       blocks[x][y].clearBlock();
                       blocks[x][y+1].clearBlock();
                       blocks[x][y+2].clearBlock();
                       return true;
                }
            
                if(checkBlock(x,y,BlockType::BLUE1) && checkBlock(x,y+1,BlockType::BLUE2) && checkBlock(x,y+2,BlockType::BLUE3)) {
                        addScore();
                        blocks[x][y].clearBlock();
                        blocks[x][y+1].clearBlock();
                        blocks[x][y+2].clearBlock();
                       return true;
                }

                if(checkBlock(x,y,BlockType::BLUE3) && checkBlock(x,y+1,BlockType::BLUE2) && checkBlock(x,y+2,BlockType::BLUE1)) {
                       addScore();
                       blocks[x][y].clearBlock();
                       blocks[x][y+1].clearBlock();
                       blocks[x][y+2].clearBlock();
                       return true;
                }

                // grid across
                 if(checkBlock(x,y,BlockType::RED1) && checkBlock(x+1,y,BlockType::RED2) && checkBlock(x+2,y,BlockType::RED3)) {
                   addScore();
                   blocks[x][y].clearBlock();
                   blocks[x+1][y].clearBlock();
                   blocks[x+2][y].clearBlock();
                   return true;
               }

                if(checkBlock(x,y,BlockType::RED3) && checkBlock(x+1,y,BlockType::RED2) && checkBlock(x+2,y,BlockType::RED1)) {
                    addScore();
                    blocks[x][y].clearBlock();
                    blocks[x+1][y].clearBlock();
                    blocks[x+2][y].clearBlock();
                    return true;
                }

                if(checkBlock(x,y,BlockType::GREEN1) && checkBlock(x+1,y,BlockType::GREEN2) && checkBlock(x+2,y,BlockType::GREEN3)) {
                       addScore();
                       blocks[x][y].clearBlock();
                       blocks[x+1][y].clearBlock();
                       blocks[x+2][y].clearBlock();
                       return true;
                }

                if(checkBlock(x,y,BlockType::GREEN3) && checkBlock(x+1,y,BlockType::GREEN2) && checkBlock(x+2,y,BlockType::GREEN1)) {
                       addScore();
                       blocks[x][y].clearBlock();
                       blocks[x+1][y].clearBlock();
                       blocks[x+2][y].clearBlock();
                      return true;
                }
            
                if(checkBlock(x,y,BlockType::BLUE1) && checkBlock(x+1,y,BlockType::BLUE2) && checkBlock(x+2,y,BlockType::BLUE3)) {
                        addScore();
                        blocks[x][y].clearBlock();
                        blocks[x+1][y].clearBlock();
                        blocks[x+2][y].clearBlock();
                       return true;
                }

                if(checkBlock(x,y,BlockType::BLUE3) && checkBlock(x+1,y,BlockType::BLUE2) && checkBlock(x+2,y,BlockType::BLUE1)) {
                       addScore();
                       blocks[x][y].clearBlock();
                       blocks[x+1][y].clearBlock();
                       blocks[x+2][y].clearBlock();
                       return true;
                }

                // grid digaonal
                if(checkBlock(x,y, BlockType::RED1) && checkBlock(x+1, y+1, BlockType::RED2) && checkBlock(x+2, y+2, BlockType::RED3)) {
                    addScore();
                    blocks[x][y].clearBlock();
                    blocks[x+1][y+1].clearBlock();
                    blocks[x+2][y+2].clearBlock();
                    return true;
                }

                if(checkBlock(x,y, BlockType::RED3) && checkBlock(x+1, y+1, BlockType::RED2) && checkBlock(x+2, y+2, BlockType::RED1)) {
                    addScore();
                    blocks[x][y].clearBlock();
                    blocks[x+1][y+1].clearBlock();
                    blocks[x+2][y+2].clearBlock();
                    return true;
                }

                if(checkBlock(x,y, BlockType::GREEN1) && checkBlock(x+1, y+1, BlockType::GREEN2) && checkBlock(x+2, y+2, BlockType::GREEN3)) {
                    addScore();
                    blocks[x][y].clearBlock();
                    blocks[x+1][y+1].clearBlock();
                    blocks[x+2][y+2].clearBlock();
                    return true;
                }

                if(checkBlock(x,y, BlockType::GREEN3) && checkBlock(x+1, y+1, BlockType::GREEN2) && checkBlock(x+2, y+2, BlockType::GREEN1)) {
                    addScore();
                    blocks[x][y].clearBlock();
                    blocks[x+1][y+1].clearBlock();
                    blocks[x+2][y+2].clearBlock();
                    return true;
                }

                if(checkBlock(x,y, BlockType::BLUE1) && checkBlock(x+1, y+1, BlockType::BLUE2) && checkBlock(x+2, y+2, BlockType::BLUE3)) {
                    addScore();
                    blocks[x][y].clearBlock();
                    blocks[x+1][y+1].clearBlock();
                    blocks[x+2][y+2].clearBlock();
                    return true;
                }

                if(checkBlock(x,y, BlockType::BLUE3) && checkBlock(x+1, y+1, BlockType::BLUE2) && checkBlock(x+2, y+2, BlockType::BLUE1)) {
                    addScore();
                    blocks[x][y].clearBlock();
                    blocks[x+1][y+1].clearBlock();
                    blocks[x+2][y+2].clearBlock();
                    return true;
                }

                if(checkBlock(x,y, BlockType::RED1) && checkBlock(x+1, y-1, BlockType::RED2) && checkBlock(x+2, y-2, BlockType::RED3)) {
                    addScore();
                    blocks[x][y].clearBlock();
                    blocks[x+1][y-1].clearBlock();
                    blocks[x+2][y-2].clearBlock();
                    return true;
                }

                if(checkBlock(x,y, BlockType::RED3) && checkBlock(x+1, y-1, BlockType::RED2) && checkBlock(x+2, y-2, BlockType::RED1)) {
                    addScore();
                    blocks[x][y].clearBlock();
                    blocks[x+1][y-1].clearBlock();
                    blocks[x+2][y-2].clearBlock();
                    return true;
                }

                if(checkBlock(x,y, BlockType::GREEN1) && checkBlock(x+1, y-1, BlockType::GREEN2) && checkBlock(x+2, y-2, BlockType::GREEN3)) {
                    addScore();
                    blocks[x][y].clearBlock();
                    blocks[x+1][y-1].clearBlock();
                    blocks[x+2][y-2].clearBlock();
                    return true;
                }

                if(checkBlock(x,y, BlockType::GREEN3) && checkBlock(x+1, y-1, BlockType::GREEN2) && checkBlock(x+2, y-2, BlockType::GREEN1)) {
                    addScore();
                    blocks[x][y].clearBlock();
                    blocks[x+1][y-1].clearBlock();
                    blocks[x+2][y-2].clearBlock();
                    return true;
                }
                if(checkBlock(x,y, BlockType::BLUE1) && checkBlock(x+1, y-1, BlockType::BLUE2) && checkBlock(x+2, y-2, BlockType::BLUE3)) {
                    addScore();
                    blocks[x][y].clearBlock();
                    blocks[x+1][y-1].clearBlock();
                    blocks[x+2][y-2].clearBlock();
                    return true;
                }

                if(checkBlock(x,y, BlockType::BLUE3) && checkBlock(x+1, y-1, BlockType::BLUE2) && checkBlock(x+2, y-2, BlockType::BLUE1)) {
                    addScore();
                    blocks[x][y].clearBlock();
                    blocks[x+1][y-1].clearBlock();
                    blocks[x+2][y-2].clearBlock();
                    return true;
                }
            }
        }
        return false;
    }

    bool Grid::procMoveDown() {
        for(int x = 0; x < grid_w; ++x) {
            for(int y = 0; y < grid_h; ++y) {
                Block *b1, *b2;
                b1 = grid(x, y);
                b2 = grid(x, y+1);
                if(b1 != nullptr && b2 != nullptr && (b1->getType() != BlockType::BLOCK_NULL && b1->getType() != BlockType::BLOCK_CLEAR && b2->getType() == BlockType::BLOCK_NULL)) {
                    *b2 = *b1;
                    *b1 = BlockType::BLOCK_NULL;
                    return true;
                }
            }
        }
        bool update = false;
        for(int x = 0; x < grid_w; ++x) {
            for(int y = 0; y < grid_h; ++y) {
                Block *b = grid(x,y);
                if(b != nullptr && b->getType() == BlockType::BLOCK_CLEAR) {
                    b->clearValue() += 1;
                    if(b->clearValue() > 50) {
                        *b = BlockType::BLOCK_NULL;
                    }
                    update = true;
                }
            }
        }
        return update;
    }

    bool Grid::checkBlock(int x, int y, const BlockType &type) {
        Block *b = grid(x,y);
        if(b != nullptr && (b->getType() == type || b->getType() == BlockType::MATCH))
            return true;
        return false;
    }


    int Grid::level() const {
        return game_level;
    }
        
    void Grid::levelNext() {
        if(game_level < 7) 
            ++game_level;
    }

    bool Grid::gameOver() const {
        return game_over;
    }
    
    void Grid::addScore() {
        lines++;
        if((lines%6)==0) {
            levelNext();
        }
    }

    int Grid::currentLines() const {
        return lines;
    }

    int Grid::block_pos(int num) {
        Piece &p = getPiece();
        return static_cast<int>(p.blocks[num].getType());
    }

    int Grid::block_getx(int num) {
        Piece &p = getPiece();
        return p.blocks[num].getX();
    }

    int Grid::block_gety(int num) {
        Piece &p = getPiece();
        return p.blocks[num].getY();
    }

    void Grid::init_rand(int n) {
        srand(static_cast<unsigned int>(n));
    }

}
using namespace puzzle;
using namespace emscripten;

EMSCRIPTEN_BINDINGS(my_class_Example) {
  class_<Grid>("PuzzleGrid")
    .constructor<int, int>()
    .function("clearGrid", &Grid::clearGrid)
    .function("getWidth", &Grid::getWidth)
    .function("getHeight", &Grid::getHeight)
    .function("keyLeft", &Grid::keyLeft)
    .function("keyRight", &Grid::keyRight)
    .function("keyDown", &Grid::keyDown)
    .function("keyShiftUp", &Grid::keyShiftUp)
    .function("keyShiftDown", &Grid::keyShiftDown)
    .function("keyRotateLeft", &Grid::keyRotateLeft)
    .function("keyRotateRight", &Grid::keyRotateRight)
    .function("level", &Grid::level)
    .function("currentLines", &Grid::currentLines)
    .function("levelNext", &Grid::levelNext)
    .function("procMoveDown", &Grid::procMoveDown)
    .function("procBlocks", &Grid::procBlocks)
    .function("gameOver", &Grid::gameOver)
    .function("grid_int", &Grid::grid_int)
    .function("block_pos", &Grid::block_pos)
    .function("block_getx", &Grid::block_getx)
    .function("block_gety", &Grid::block_gety)
    .function("init_rand", &Grid::init_rand)
    .function("clearPiece", &Grid::clearPiece)
  //  .property("x"incrementX, &MyClass::getX, &MyClass::setX)
    
    ;
}

