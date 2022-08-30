
var grid;
var GRID_WIDTH=1280/64;
var GRID_HEIGHT=720/32;
var BLOCK_NULL = 0;
var BLOCK_CLEAR = 1;
var BLOCK_RED1 = 2;
var BLOCK_RED2 = 3;
var BLOCK_RED3 = 4;
var BLOCK_GREEN1 = 5;
var BLOCK_GREEN2 = 6;
var BLOCK_GREEN3 = 7;
var BLOCK_BLUE1 = 8;
var BLOCK_BLUE2 = 9;
var BLOCK_BLUE3 = 10;
var BLOCK_MATCH = 11;

var Module = {
      onRuntimeInitialized: function() {
        grid = new Module.PuzzleGrid(GRID_WIDTH, GRID_HEIGHT);
        grid.init_rand(Math.floor(Math.random()*1000));
        main(grid);
      }
};

var can = document.getElementById("can1");
var context = can.getContext("2d");
var images = [];
var level = []

function loadGraphics() {
    var graphics = ['red1.png', 'red2.png', 'red3.png', 'green1.png', 'green2.png', 'green3.png', 'blue1.png', 'blue2.png', 'blue3.png'];
    var level_gfx = ['level1', 'level2', 'level3', 'level4', 'level5', 'level6', 'level7' ];

    for(var i = 0; i < graphics.length; ++i) {
		images[i+2] = new Image();
		images[i+2].onload = function() {}
		images[i+2].src = "img/" + graphics[i];	
	}

    for(i = 0; i <= 6; ++i) {
        level[i] = new Image();
        level[i].onload = function() {}
        level[i].src = "img/" + level_gfx[i] + ".png";
    }
}

function main(puzzle) {
    loadGraphics();
    grid.clearGrid();
    window.addEventListener("keydown", keyPressed, true);
    setInterval(drawScreen, 25);
    drawScreen();
}

function keyPressed(key) {

    if(grid.gameOver() === false) {
        switch(key.keyCode) {
		    case 37:
		        grid.keyLeft();
		        key.preventDefault();
		    break;
		    case 38:
                grid.keyShiftUp();
		        key.preventDefault();
		    break;
            case 32:
                grid.keyRotateLeft();
                key.preventDefault();
                break;
		    case 39:
	    	    grid.keyRight();
    		    key.preventDefault();
		    break;
		    case 40:
		        grid.keyDown();
		        key.preventDefault();
		    break;
	    }
    } else {
        if(key.keyCode === 13) {
            grid.clearGrid();
            key.preventDefault();
        }
    }
}

function drawBlock(num) {

    var block_t = grid.block_pos(num);
    var block_x = grid.block_getx(num);
    var block_y = grid.block_gety(num);

    if(block_t === BLOCK_MATCH) {
        var r = Math.floor(Math.random()*8);
        context.drawImage(images[2+r], block_x*64, block_y*32);
    } else { 
        context.drawImage(images[block_t], block_x*64, block_y*32);
    }
}

function logic() {
    grid.procMoveDown();
    grid.procBlocks();
}

var counter = 0;
var clear_colors = ['#FF0000', '#00FF00', '#0000FF' ];

function drawScreen() {

    logic();

    ++counter;
    if(grid.gameOver() === false && counter > 25) {
        counter = 0;
        grid.keyDown();
    }
    context.drawImage(level[grid.level()-1], 0, 0, 1280, 720);
    context.font="12px Verdana";
    if(grid.gameOver() === false) {
        for(var x = 0; x < GRID_WIDTH; ++x) {
            for(var y = 0; y < GRID_HEIGHT; ++y) {
                var pos = grid.grid_int(x, y);
                switch(pos) {
                    case -1:
                        break;
                    case BLOCK_NULL:
                        context.fillStyle = "#000000";
                        context.fillRect(x*64, y*32, 62, 30);
                        break;
                    case BLOCK_CLEAR:
                        context.fillStyle = clear_colors[Math.floor(Math.random()*3)];
                        context.fillRect(x*64, y*32, 62, 30);
                        break;
                    case BLOCK_MATCH:
                        var r = Math.floor(Math.random()*8);
                         context.drawImage(images[2+r], x*64, y*32, 62, 30);
                        break;
                    default:
                        context.drawImage(images[pos], x*64,y*32, 62, 30);
                        break;
                }
                
            }
        }

        drawBlock(0);
        drawBlock(1);
        drawBlock(2);
    } else {

        context.fillStyle = "#000000";
        context.fillRect( 50, 50, 1280-100, 720-100 );
        context.fillStyle="#FFFFFF";
        context.font="54px Verdana";
        context.fillText("Game Over Press Return to Start Over", 100, 150);

    }
    context.fillStyle="#FFFFFF";
    context.font="24px Verdana";
    context.fillText("Score: " + grid.currentLines() + " Level: " + grid.level(), 25, 25);

}
