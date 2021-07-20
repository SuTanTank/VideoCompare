# Video Compare Tool (OpenCV)

As an algorithm engineer in image processing, I need to compare two videos as algorithm results a lot, sometimes back and forth, sometimes side by side. 

Beyond Compare doesn't support video, which is painful. So I wrote one for myself.

## Feature

- Compare two video side-by-side (horizontally or Vertical placed) and back-and-forth.
- Zoom in/out and move ROI
- Slow down the speed (no frame interpolation)
- Step and seek forward and backward

## Usage

- Play
    - space - pause / resume
    - esc - quit
    - left arrow(play) - 3 second back
    - left arrow(pause) - 1 frame back
    - right arrow(play) - 3 second ahead
    - right arrow(pause) - 1 frame ahead
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

Usage: VideoCompare.exe [params] 1 2

        -?, -h, --help, --usage
                print this message
        -v, --view (value:3)
                initial view

        1 
                path to 1st video
        2 
                path to 2nd video
