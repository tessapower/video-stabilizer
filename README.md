# Video Stabilizer

TODO: add CI badges

A small and simple program that utilizes computer vision algorithms to stabilize shaky videos by detecting motion and compensating for unwanted movements.

## How Does it Work?

By tracking key features across frames, estimating the camera's motion path, and applying smoothing techniques to reduce jitter. The stabilized output retains the original video's quality while creating a smoother viewing experience, which is ideal for correcting handheld footage or dynamic environments.

During stabilization, the algorithm attempts to counteract shaky movements by shifting the frames accordingly. These shifts can create gaps or black borders around the edges of the video. To avoid showing these unwanted borders, stabilized videos are slightly cropped—a small trade-off to hide the artifacts introduced. The stabilization process also smooths out the camera motion path, reducing jitters by focussing on a smaller, more consistent part of the frame.

## Usage


TODO: Write usage instructions
TODO: Add screenshots/gifs

## Limitations

- Currently only supports being run on Windows.
- Output videos are in MP4 format.

## Building

### Prerequisites

To build this project, please ensure that your system has the following software already installed:

- [vcpkg](https://vcpkg.io/)
- [CMake](https://cmake.org/)
- [git](https://git-scm.com/)

## Future Improvements

- [ ] Loading and progress status indicators
- [ ] Preview of original video
- [ ] Preview of stabilized video
- [ ] Run stabilization in dedicated thread
- [ ] Add support for logging
- [ ] Display video meta-data
- [ ] Support codecs for other platforms (currently only supports DIVX)
- [ ] Support multiple output formats
- [ ] Support running on MacOS and mainstream Linux distros

## Attributions

The user interface was created using [ImGui](https://github.com/ocornut/imgui), and the computer vision algorithms were implemented with the [OpenCV library](https://opencv.org/).

## Contributing

Contributions to enhance and improve this project are welcome! If you would like to contribute, please follow these steps:

1. **Fork the repository**: Create your own copy of the project by forking this repository.
2. **Create a branch**: Work on your feature, bug fix, or enhancement in a separate branch (`git checkout -b your-feature-branch`).
3. **Write clear code**: Ensure your code is readable, maintainable, and well-documented. Follow the project’s coding guidelines.
4. **Test your changes**: Run any existing tests and, if applicable, write new tests to validate your changes.
5. **Submit a pull request**: Once your changes are ready, open a pull request with a clear description of the problem solved or improvement made.

Your contributions and efforts to make this project better are greatly appreciated!
