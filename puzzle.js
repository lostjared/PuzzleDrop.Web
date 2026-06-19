var grid;
const GRID_WIDTH = 1280 / 64;
const GRID_HEIGHT = 720 / 32;
const BLOCK_NULL = 0;
const BLOCK_CLEAR = 1;
const BLOCK_MATCH = 11;
const CANVAS_WIDTH = 1280;
const CANVAS_HEIGHT = 720;

var Module = {
    onRuntimeInitialized: function() {
        grid = new Module.PuzzleGrid(GRID_WIDTH, GRID_HEIGHT);
        grid.init_rand(Math.floor(Math.random() * 1000));
        main(grid);
    }
};

var can = document.getElementById("can1");
var context = can.getContext("2d");
var boardFrame = document.getElementById("boardFrame");
var images = [];
var level = [];
var repeatTimers = new Map();

function loadGraphics() {
    const graphics = [
        "red1.png", "red2.png", "red3.png",
        "green1.png", "green2.png", "green3.png",
        "blue1.png", "blue2.png", "blue3.png"
    ];
    const levelGfx = ["level1", "level2", "level3", "level4", "level5", "level6", "level7"];

    for (var i = 0; i < graphics.length; ++i) {
        images[i + 2] = new Image();
        images[i + 2].src = "img/" + graphics[i];
    }

    for (i = 0; i < levelGfx.length; ++i) {
        level[i] = new Image();
        level[i].src = "img/" + levelGfx[i] + ".png";
    }
}

function fitCanvas() {
    if (!boardFrame || !can) {
        return;
    }

    const rect = boardFrame.getBoundingClientRect();
    if (rect.width <= 0 || rect.height <= 0) {
        return;
    }

    const landscape = window.matchMedia && window.matchMedia("(orientation: landscape)").matches;
    const scale = landscape
        ? Math.max(rect.width / CANVAS_WIDTH, rect.height / CANVAS_HEIGHT)
        : Math.min(rect.width / CANVAS_WIDTH, rect.height / CANVAS_HEIGHT);
    const displayWidth = Math.max(1, Math.floor(CANVAS_WIDTH * scale));
    const displayHeight = Math.max(1, Math.floor(CANVAS_HEIGHT * scale));

    can.style.width = displayWidth + "px";
    can.style.height = displayHeight + "px";
}

function canInteract() {
    return grid && grid.gameOver() === false;
}

function runAction(action) {
    if (!grid) {
        return;
    }

    if (action === "restart") {
        grid.clearGrid();
        counter = 0;
        isDown = false;
        isMoved = false;
        downX = 0;
        downY = 0;
        upX = 0;
        upY = 0;
        return;
    }

    if (!canInteract()) {
        return;
    }

    switch (action) {
        case "left":
            grid.keyLeft();
            break;
        case "right":
            grid.keyRight();
            break;
        case "down":
            grid.keyDown();
            break;
        case "drop":
            grid.keyDrop();
            break;
        case "shift-up":
            grid.keyShiftUp();
            break;
        case "shift-down":
            grid.keyShiftDown();
            break;
        case "rotate-left":
            grid.keyRotateLeft();
            break;
        case "rotate-right":
            grid.keyRotateRight();
            break;
    }
}

function bindButton(button, action, repeat) {
    if (!button) {
        return;
    }

    const stopRepeat = function() {
        const timer = repeatTimers.get(button);
        if (timer) {
            clearInterval(timer);
            repeatTimers.delete(button);
        }
    };

    const start = function(event) {
        event.preventDefault();
        runAction(action);

        if (!repeat) {
            return;
        }

        stopRepeat();
        repeatTimers.set(button, setInterval(function() {
            runAction(action);
        }, 120));
    };

    const stop = function(event) {
        if (event) {
            event.preventDefault();
        }
        stopRepeat();
    };

    button.addEventListener("pointerdown", start);
    button.addEventListener("pointerup", stop);
    button.addEventListener("pointercancel", stop);
    button.addEventListener("pointerleave", stop);
    button.addEventListener("contextmenu", function(event) {
        event.preventDefault();
    });
}

function setupTouchButtons() {
    const buttons = document.querySelectorAll("[data-action]");
    for (const button of buttons) {
        const action = button.getAttribute("data-action");
        const repeat = action === "left" || action === "right" || action === "down";
        bindButton(button, action, repeat);
    }
}

function main(puzzle) {
    loadGraphics();
    grid.clearGrid();

    window.addEventListener("keydown", keyPressed, true);
    window.addEventListener("resize", fitCanvas, { passive: true });
    window.addEventListener("orientationchange", fitCanvas, { passive: true });

    setupTouchButtons();
    setupCanvasGestures();
    fitCanvas();

    setInterval(drawScreen, 25);
    drawScreen();
}

var downX = 0, downY = 0, upX = 0, upY = 0;
var isDown = false;
var isMoved = false;
var touchStartTime = 0;
var touchStartTouches = 0;
var multiTouchActive = false;
var multiTouchTriggered = false;
var touchMoveThreshold = 42;
var tapMoveThreshold = 12;
var tapDurationThreshold = 240;
var dropSwipeThreshold = 85;

function setupCanvasGestures() {
    if (!can) {
        return;
    }

    can.addEventListener("touchstart", mouseDown, { passive: false });
    can.addEventListener("touchend", mouseUp, { passive: false });
    can.addEventListener("touchmove", mouseMove, { passive: false });
    can.addEventListener("mousedown", mouseDown, true);
    can.addEventListener("mouseup", mouseUp, true);
    can.addEventListener("mousemove", mouseMoveB, true);
}

function mouseMove(e) {
    if (e.touches.length === 0) {
        return;
    }

    upX = e.touches[0].pageX;
    upY = e.touches[0].pageY;

    if (multiTouchActive === true) {
        e.stopPropagation();
        e.preventDefault();
        return;
    }

    if (isDown === true) {
        var xpos = e.touches[0].pageX;
        if (xpos > downX + touchMoveThreshold) {
            downX = xpos;
            runAction("right");
            isMoved = true;
        } else if (xpos < downX - touchMoveThreshold) {
            downX = xpos;
            runAction("left");
            isMoved = true;
        }
    }

    e.stopPropagation();
    e.preventDefault();
}

function mouseMoveB(e) {
    if (isDown === true) {
        var xpos = e.pageX;
        if (xpos > downX + 50) {
            downX = xpos;
            runAction("right");
            isMoved = true;
        } else if (xpos < downX - 50) {
            downX = xpos;
            runAction("left");
            isMoved = true;
        }
    }
    e.preventDefault();
}

function mouseDown(e) {
    if (e.type === "touchstart") {
        touchStartTime = Date.now();
        touchStartTouches = e.touches.length;
        multiTouchActive = e.touches.length > 1;
        multiTouchTriggered = false;
        downX = e.touches[0].pageX;
        downY = e.touches[0].pageY;
    } else {
        downX = e.pageX;
        downY = e.pageY;
    }

    isDown = true;
}

function mouseUp(e) {
    if (downX === 0 && downY === 0) {
        return;
    }

    if (e.type === "touchend") {
        if (touchStartTouches > 1) {
            if (e.touches.length === 0 && multiTouchActive === true && multiTouchTriggered === false) {
                runAction("rotate-left");
                multiTouchTriggered = true;
            }
            if (e.touches.length === 0) {
                multiTouchActive = false;
                touchStartTouches = 0;
                isDown = false;
            }
            e.preventDefault();
            return;
        }

        if (e.changedTouches.length === 0) {
            return;
        }
        upX = e.changedTouches[0].pageX;
        upY = e.changedTouches[0].pageY;
    } else {
        upX = e.pageX;
        upY = e.pageY;
    }

    var deltaX = downX - upX;
    var deltaY = downY - upY;
    var tapDuration = Date.now() - touchStartTime;
    var absX = Math.abs(deltaX);
    var absY = Math.abs(deltaY);

    if (deltaY < -dropSwipeThreshold && absY > absX * 1.15) {
        runAction("drop");
    } else if (absX > 100) {
        if (isMoved === false && deltaX < 0) {
            runAction("right");
        }
        if (isMoved === false && deltaX > 0) {
            runAction("left");
        }
    } else if (absX <= tapMoveThreshold && absY <= tapMoveThreshold && tapDuration <= tapDurationThreshold) {
        runAction("shift-up");
    }

    if (isMoved === true) {
        isMoved = false;
    }

    isDown = false;
    touchStartTouches = 0;
    multiTouchActive = false;
    multiTouchTriggered = false;

    e.preventDefault();
}

function keyPressed(key) {
    if (grid.gameOver() === false) {
        switch (key.keyCode) {
            case 37:
                runAction("left");
                key.preventDefault();
                break;
            case 38:
                runAction("shift-up");
                key.preventDefault();
                break;
            case 32:
                runAction("drop");
                key.preventDefault();
                break;
            case 39:
                runAction("right");
                key.preventDefault();
                break;
            case 40:
                runAction("down");
                key.preventDefault();
                break;
            case 90:
                runAction("rotate-left");
                key.preventDefault();
                break;
            case 88:
                runAction("rotate-right");
                key.preventDefault();
                break;
            case 82:
                runAction("restart");
                key.preventDefault();
                break;
        }
    } else {
        if (key.keyCode === 13 || key.keyCode === 82) {
            runAction("restart");
            key.preventDefault();
        }
    }
}

function newGame() {
    grid.clearGrid();
}

function drawBlock(num) {
    var block_t = grid.block_pos(num);
    var block_x = grid.block_getx(num);
    var block_y = grid.block_gety(num);

    if (block_t === BLOCK_MATCH) {
        var r = Math.floor(Math.random() * 8);
        context.drawImage(images[2 + r], block_x * 64, block_y * 32, 63, 31);
    } else {
        context.drawImage(images[block_t], block_x * 64, block_y * 32, 63, 31);
    }
}

function logic() {
    grid.procMoveDown();
    grid.procBlocks();
}

var counter = 0;
const clear_colors = ["#FF0000", "#00FF00", "#0000FF"];

function drawScreen() {
    logic();

    ++counter;
    if (grid.gameOver() === false && counter > 25) {
        counter = 0;
        grid.keyDown();
    }

    context.drawImage(level[grid.level() - 1], 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);
    context.font = "12px Verdana";

    if (grid.gameOver() === false) {
        for (var x = 0; x < GRID_WIDTH; ++x) {
            for (var y = 0; y < GRID_HEIGHT; ++y) {
                var pos = grid.grid_int(x, y);
                switch (pos) {
                    case -1:
                        break;
                    case BLOCK_NULL:
                        context.fillStyle = "#000000";
                        context.fillRect(x * 64, y * 32, 63, 31);
                        break;
                    case BLOCK_CLEAR:
                        context.fillStyle = clear_colors[Math.floor(Math.random() * 3)];
                        context.fillRect(x * 64, y * 32, 63, 31);
                        break;
                    case BLOCK_MATCH:
                        var r = Math.floor(Math.random() * 8);
                        context.drawImage(images[2 + r], x * 64, y * 32, 63, 31);
                        break;
                    default:
                        context.drawImage(images[pos], x * 64, y * 32, 63, 31);
                        break;
                }
            }
        }

        drawBlock(0);
        drawBlock(1);
        drawBlock(2);
    } else {
        context.fillStyle = "#000000";
        context.fillRect(50, 50, CANVAS_WIDTH - 100, CANVAS_HEIGHT - 100);
        context.fillStyle = "#FFFFFF";
        context.font = "54px Verdana";
        context.fillText("Game Over Press Return to Start Over", 100, 150);
    }

    context.fillStyle = "#FFFFFF";
    context.font = "24px Verdana";
    context.fillText("Score: " + grid.currentLines() + " Level: " + grid.level(), 25, 25);
}
