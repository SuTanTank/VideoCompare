# Video Compare Tool (OpenCV)

As an Algorithm engineer, I need compare video algorithm effect a lot, sometimes back and forth, sometimes side by side
Beyond Compare doesn't support video, which is painful for algorithm engineer. So I wrote one for myself.

- Support compare two video side-by-side (horizontally or Vertical placed) and back-and-forth.
- Support zoom in/out and move ROI

## Usage

- Play
  - space - pause / resume
  - esc - quit
- View
  - q - zoom out
  - e - zoom in
  - wasd - pan all video
  - WASD - pan the first video in pixel (not finished yet)
  - 1 - switch to #1 video
  - 2 - switch to #2 video
  - 3 - switch to horitonzal view
  - 4 - switch to vertical view
  - f - switch between scale and crop (when video size are different)
- Speed
  - z - speed down
  - x - speed reset
  - c - speed up
