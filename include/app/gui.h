#ifndef GUI_H
#define GUI_H

#include <glad/glad.h>

#include <iostream>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "utils.h"

namespace gui {
inline static constexpr auto window_flags =
    ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoTitleBar;

inline static constexpr auto popup_flags = ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;

inline static constexpr auto popup_size = ImVec2(250.0f, 0.0f);

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
    //----------------------------------------------------------- Menu Bar --//
    // Remember that we use if-statements here because we
    // should only call EndMenu(Bar) if BeginMenu(Bar) returns true
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Import Video")) {
          std::string path;
          if (utils::get_video_path(window, path)) {
            std::cout << "Opening file: " << path << '\n';
          }
        }

        if (ImGui::MenuItem("Import Frames")) {
          std::vector<std::string> paths;
          if (utils::get_frame_paths(window, paths)) {
            for (const auto& path : paths) {
              std::cout << "Opening file: " << path << '\n';
            }
          }
        }

        if (ImGui::MenuItem("Save")) {
          std::cout << "Save current video project\n";
        }

        if (ImGui::MenuItem("Close")) {
          std::cout << "Exiting...\n";
          glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }

    //--------------------------------------------------- Window Content --//
    ImGui::Text("Video Stabilizer");

    // TODO: Add output console here so user can see what's happening

    for (auto i = 0; i < 100; ++i) ImGui::Spacing();

    //---------------------------------------------------------- Buttons --//
    if (ImGui::Button("About")) ImGui::OpenPopup("about_popup");
    ImGui::SameLine();
    if (ImGui::Button("License")) ImGui::OpenPopup("license_popup");
    ImGui::SameLine();
    if (ImGui::Button("Help")) ImGui::OpenPopup("help_popup");
    ImGui::SameLine();
    if (ImGui::Button("Release Notes")) ImGui::OpenPopup("release_notes_popup");

    //------------------------------------------------------- Modal Popups --//
    // Always center this window when appearing
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowSize(popup_size);
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("about_popup", nullptr, popup_flags)) {
      ImGui::TextWrapped(
          "This is a description of the program and what it does.");
      ImGui::Spacing();

      if (centered_button("Close")) ImGui::CloseCurrentPopup();

      ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("license_popup", nullptr, popup_flags)) {
      ImGui::TextWrapped(
          "MIT License\n"
          "Copyright (c) 2024 Tessa Power\n\n"

          "Permission is hereby granted, free of charge, to any person "
          "obtaining a copy of this software and associated documentation "
          "files (the \"Software \"), to deal in the Software without "
          "restriction, including without limitation the rights to use, "
          "copy, modify, merge, publish, distribute, sublicense, and/or "
          "sell copies of the Software, and to permit persons to whom "
          "the Software is furnished to do so, subject to the"
          "following conditions:\n\n"

          "The above copyright notice and this permission notice shall be"
          "included in all copies or substantial portions of the "
          "Software.\n\n"

          "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, "
          "EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES "
          "OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND"
          " NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT "
          "HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, "
          "WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING "
          "FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE "
          "OR OTHER DEALINGS IN THE SOFTWARE.");

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

    if (ImGui::BeginPopupModal("release_notes_popup", nullptr, popup_flags)) {
      ImGui::TextWrapped("This will contain the latest release notes.");
      ImGui::Spacing();

      if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();

      ImGui::EndPopup();
    }

    //------------------------------------- Repo and Issue Tracker Links --//
    ImGui::TextLinkOpenURL("View this project on GitHub",
                           "https://github.com/tessapower/video-stabilizer");

    ImGui::TextLinkOpenURL(
        "Report an issue",
        "https://github.com/tessapower/video-stabilizer/issues/new");
  }

  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
}  // namespace gui

#endif  // GUI_H
