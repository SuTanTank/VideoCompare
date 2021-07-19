# Video Compare Tool (OpenCV)

As an CV algorithm engineer, I need compare algorithm result a lot, sometimes back and forth, sometimes side by side. 

Beyond Compare doesn't support video, which is painful. So I wrote one for myself.

## Feature

- Support compare two video side-by-side (horizontally or Vertical placed) and back-and-forth.
- Support zoom in/out and move ROI
- Support slow down the speed (no frame interpolation)

## Usage

- Play
  - space - pause / resume
  - esc - quit
- View
  - q - zoom out
  - e - zoom in
  - wasd - pan all video
  - 1 - switch to #1 video
  - 2 - switch to #2 video
  - 3 - switch to horitonzal view
  - 4 - switch to vertical view
  - f - switch between scale and crop (when video size are different)
- Speed
  - z - speed down
  - x - speed reset
  - c - speed up
