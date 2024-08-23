#ifndef UTILS_H
#define UTILS_H

#if defined(_WIN32)
  #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__linux__)
  #define GLFW_EXPOSE_NATIVE_X11
#elif defined(__APPLE__)
  #define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <nfd.h>
#include <nfd_glfw3.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace utils {
inline auto get_video_path(GLFWwindow* window, std::string& path) -> bool {
  nfdchar_t* out_path = nullptr;
  constexpr nfdu8filteritem_t filters[1] = {{"video", "mov,mp4,mpeg4,wmv,avi,flv"}};
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

inline auto get_frame_paths(GLFWwindow* window, std::vector<std::string>& paths) -> bool {
  const nfdpathset_t* out_paths;
  constexpr nfdu8filteritem_t filters[1] = {
      {"images", "jpg,jpeg,png"}};
  nfdopendialogu8args_t args = {nullptr, 0, nullptr};
  NFD_GetNativeWindowFromGLFWWindow(window, &args.parentWindow);
  args.filterList = filters;
  args.filterCount = 1;
  // Pass in the address of the pointer to the path set and the args
  nfdresult_t result = NFD_OpenDialogMultipleU8_With(&out_paths, &args);

  switch (result) {
    case NFD_OKAY: {
      std::cout << "Success!\n";

      // Iterate over the paths in the path set
      // Declare enumerator (not a pointer)
      nfdpathsetenum_t enumerator;
      NFD_PathSet_GetEnum(out_paths, &enumerator);
      nfdchar_t* path;
      unsigned i = 0;
      while (NFD_PathSet_EnumNext(&enumerator, &path) && path) {
        paths.emplace_back(path);
        std::cout << "Path " << i++ << ": " << path << "\n";

        // Free the path set path with NFD_PathSet_FreePath (not NFD_FreePath!)
        NFD_PathSet_FreePath(path);
      }

      // Free the path set enumerator memory (before freeing the path set)
      NFD_PathSet_FreeEnum(&enumerator);

      // Free the path set memory (since NFD_OKAY is returned)
      NFD_PathSet_Free(out_paths);

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
