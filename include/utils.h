#ifndef UTILS_H
#define UTILS_H

#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3.h>
#include <nfd.h>
#include <nfd_glfw3.h>

#include <filesystem>
#include <iostream>
#include <vector>

namespace utils {

/**
 * @brief Opens a native file dialog so the user can select a video file. Only
 * files with the following extensions can be selected: *.mov, *.mp4, *.mpeg4,
 * *.wmv, *.avi, and *.flv.
 *
 * @param window The parent window of the file picker.
 * @param path A string to store the selected file path.
 * @return Whether the user successfully selected a file.
 */
inline auto get_video_path(GLFWwindow* window, std::string& path) -> bool {
  nfdchar_t* out_path = nullptr;
  constexpr nfdu8filteritem_t filters[1] = {
      {"video", "mov,mp4,mpeg4,wmv,avi,flv"}};
  nfdopendialogu8args_t args = {nullptr, 0, nullptr};
  NFD_GetNativeWindowFromGLFWWindow(window, &args.parentWindow);
  args.filterList = filters;
  args.filterCount = 1;
  nfdresult_t result = NFD_OpenDialogU8_With(&out_path, &args);

  switch (result) {
    case NFD_OKAY: {
      std::cout << "Success!\n";
      path = std::string{out_path};
      NFD_FreePathU8(out_path);
      return true;
    }
    case NFD_CANCEL: {
      std::cout << "User pressed cancel.\n";
      return false;
    }
    case NFD_ERROR: {
      std::cerr << "Error: " << NFD_GetError() << '\n';
      return false;
    }
  }

  return false;
}

inline auto get_save_directory(std::string& out_dir) -> bool {
  nfdchar_t* dir = nullptr;
  nfdresult_t result = NFD_PickFolderU8(&dir, nullptr);

  switch (result) {
    case NFD_OKAY: {
      std::cout << "Success!\n";
      out_dir = dir;
      NFD_FreePathU8(dir);

      return true;
    }
    case NFD_CANCEL: {
      std::cout << "User pressed cancel.\n";

      return false;
    }
    case NFD_ERROR: {
      std::cerr << "Error: " << NFD_GetError() << '\n';

      return false;
    }
  }

  return false;
}
}  // namespace utils

#endif  // UTILS_H
