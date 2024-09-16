# Video Stabilizer

TODO: add CI badges

An application of Lowe's Scale-Invariant Feature Transform (SIFT)[^1] feature detection and Fischler and Bolles' RAndom SAmple Consesus (RANSAC)[^2] algorithm to compute robust estimate homography matrices to stabilize a video on a frame-by-frame basis.

[^1]: Lowe, David G. "Distinctive image features from scale-invariant keypoints." _International Journal of Computer Vision_, vol. 60, no. 2, 2004, pp. 91-110. [https://doi.org/10.1023/B:VISI.0000029664.99615.94](https://doi.org/10.1023/B:VISI.0000029664.99615.94)

[^2]: Fischler, Martin A., and Robert C. Bolles. "Random sample consensus: A paradigm for model fitting with applications to image analysis and automated cartography." _Communications of the ACM_, vol. 24, no. 6, 1981, pp. 381-395. [https://doi.org/10.1145/358669.358692](https://doi.org/10.1145/358669.358692).


## Example

| Original | Stabilized |
|:--------:|:----------:|
| ![Original Video](./docs/original.gif) | ![Stabilized Video](./docs/stabilized.gif) |

## Usage

The program should be very straight forward to use. Follow the steps below to stabilize a video:

- Start by clicking the `Import Video` button to select a video. Supported formats are `.MOV` ,`.MP4` or `.MPEG4`, `.WMV`, `.AVI`, and .`.FLV`.
- Once selected, the program will load your video.
- Click the `Stabilize` button. The program will begin stabilizing your video.
- When the video is finished, you'll see a message displayed in the log area.
- Click the `Save` button to select the folder where your stabilized video will be saved. The stabilized video will be saved as a new file with a different name.

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
- [ ] Custom file name for stabilized video
- [ ] Add support for logging
- [ ] Display video meta-data
- [ ] Support codecs for other platforms (currently only supports DIVX)
- [ ] Support multiple output formats
- [ ] Support running on MacOS and mainstream Linux distros
- [ ] Set up a vcpkg binary cache using [GitHub Actions Cache](https://learn.microsoft.com/en-us/vcpkg/consume/binary-caching-github-actions-cache)

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
