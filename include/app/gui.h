#ifndef GUI_H
#define GUI_H

#include <glad/glad.h>
#include <iostream>
#include <vector>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "utils.h"

namespace gui {
inline static constexpr auto window_flags =
    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;

inline static constexpr auto popup_flags = ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;

inline static constexpr auto popup_size = ImVec2(400.0f, 300.0f);

/**
 * Creates an ImGui button that is centered horizontally in the window.
 * @param text The button text.
 * @return True if the button was activated (i.e. clicked), otherwise false.
 */
inline auto centered_button(std::string const& text) -> bool {
  const auto width = ImGui::CalcTextSize(text.c_str()).x + 20.0f;
  ImGui::SetCursorPosX((ImGui::GetWindowWidth() - width) / 2.0f);

  return ImGui::Button(text.c_str(), ImVec2(width, 0.0f));
}

inline auto render(GLFWwindow* window) -> void {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // Set up window dimensions to match the size of the viewport
  static const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);

  ImGui::Begin("App", nullptr, window_flags);
  {
    //--------------------------------------------------- Window Content --//
    ImGui::TextWrapped(
        "This is a short description of the program and what it does.\n\n");

    //----------------------------------------------------- Action Buttons --//
    if (ImGui::Button("Import Video")) {
      std::string path;
      if (utils::get_video_path(window, path)) {
        std::cout << "Opening file: " << path << '\n';
      }
    }
    ImGui::SameLine();

    if (ImGui::Button("Import Frames")) {
      std::vector<std::string> paths;
      if (utils::get_frame_paths(window, paths)) {
        for (const auto& path : paths) {
          std::cout << "Opening file: " << path << '\n';
        }
      }
    }
    ImGui::SameLine();

    ImGui::BeginDisabled(true);
    if (ImGui::Button("Stabilize")) {
      std::cout << "Stabilize and crop the video...\n";
    }
    ImGui::EndDisabled();

    ImGui::BeginDisabled(true);
    if (ImGui::Button("Save")) {
      std::cout << "Save current video project...\n";
    }
    ImGui::EndDisabled();

    //------------------------------------------------------ Logger window --//

    if (ImGui::Button("Help")) {
      // ImGui::OpenPopup("help_popup");
      // TODO: decide whether we like the help popup or help log better
      app::logger.add_log("This will display the help menu.\n");
    }
    ImGui::SameLine();
    app::logger.draw();
    ImGui::Spacing();

    //------------------------------------------------------------- Footer --//
    if (ImGui::Button("License")) ImGui::OpenPopup("license_popup");

    ImGui::TextLinkOpenURL("View this project on GitHub",
                           "https://github.com/tessapower/video-stabilizer");
    // ImGui::SameLine();
    ImGui::TextLinkOpenURL(
        "Report an issue",
        "https://github.com/tessapower/video-stabilizer/issues/new");

    //------------------------------------------------------- Modal Popups --//
    // Always center this window when appearing
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowSize(popup_size);
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("license_popup", nullptr, popup_flags)) {
      ImGui::TextWrapped(
          "MIT License\n"
          "Copyright (c) 2024 Tessa Power\n\n"

          "Permission is hereby granted, free of charge, to any person "
          "obtaining a copy of this software and associated documentation "
          "files (the \"Software\"), to deal in the Software without "
          "restriction, including without limitation the rights to use, "
          "copy, modify, merge, publish, distribute, sublicense, and/or "
          "sell copies of the Software, and to permit persons to whom "
          "the Software is furnished to do so, subject to the "
          "following conditions:\n\n"

          "The above copyright notice and this permission notice shall be "
          "included in all copies or substantial portions of the "
          "Software.\n\n"

          "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, "
          "EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES "
          "OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND "
          "NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT "
          "HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, "
          "WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING "
          "FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE "
          "OR OTHER DEALINGS IN THE SOFTWARE.\n\n");
      ImGui::Spacing();

      if (centered_button("Close")) ImGui::CloseCurrentPopup();

      ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("help_popup", nullptr, popup_flags)) {
      ImGui::TextWrapped(
          "This will contain helpful instructions for using the program. "
          "This will contain helpful instructions for using the program. "
          "This will contain helpful instructions for using the program. "
          "This will contain helpful instructions for using the program. "
          "This will contain helpful instructions for using the program. "
          "This will contain helpful instructions for using the program. "
          "This will contain helpful instructions for using the program. "
          "This will contain helpful instructions for using the program. ");
      ImGui::Spacing();

      if (centered_button("Close")) ImGui::CloseCurrentPopup();

      ImGui::EndPopup();
    }
  }

  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
}  // namespace gui

#endif  // GUI_H
