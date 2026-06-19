# PuzzleDrop Mobile

This build is tuned for phones and tablets.

## Layout

- Portrait mode shows the game board and touch buttons.
- Landscape mode hides the button panel and uses the full screen for gameplay.
- The board keeps its aspect ratio in portrait.
- In landscape, the board is expanded to fill the viewport.

## How To Play

Your goal is to arrange falling blocks so they form valid 3-block sequences in the same color.

Sequences can clear when they match:

- `1-2-3`
- `3-2-1`

Matches can happen horizontally, vertically, or diagonally.

The flashing block is a wildcard and can stand in for any value in a sequence.

## Touch Controls

On the canvas:

- Swipe left: move left
- Swipe right: move right
- Tap: shift colors up
- Swipe down: hard drop
- Two-finger tap: rotate left

In portrait mode, the on-screen buttons provide the same actions:

- Left and Right
- Rotate Left and Rotate Right
- Down
- Drop
- Shift Up
- Shift Down
- Restart

## Notes

- The game is still playable with keyboard input on desktop.
- Restart is available from the touch panel in portrait and with `R` on keyboard.
