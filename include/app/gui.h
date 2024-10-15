#ifndef GUI_H
#define GUI_H

#include <glad/glad.h>

#include <iostream>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "log/log.h"
#include "utils.h"
#include "video/vid.h"

namespace gui {
inline static constexpr auto window_flags =
    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;

inline static constexpr auto popup_flags = ImGuiWindowFlags_NoResize |
                                           ImGuiWindowFlags_NoMove |
                                           ImGuiWindowFlags_NoTitleBar;

inline static constexpr auto popup_size = ImVec2(400.0f, 300.0f);

inline static bool video_loaded = false;
inline static bool video_stabilized = false;

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

/**
 * Renders the graphical user interface in the given window. Forwards user
 * actions to the appropriate functions.
 */
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
        "Smooth out shaky footage with this video stabilization tool! "
        "Stabilized videos will be saved as a new file in your chosen folder.\n\n");

    //----------------------------------------------------- Action Buttons --//
    if (ImGui::Button("Import Video")) {
      std::string path;
      if (utils::get_video_path(window, path)) {
        app::video = vid::video(path);

        video_loaded = !app::video.empty();
        if (video_loaded) {
          log::instance()->add_log("Video loaded successfully!\n");
          // TODO: add original video preview?
        } else {
          log::instance()->add_log("Error: Could not load video.\n");
        }
      }
    }
    ImGui::SameLine();

    ImGui::BeginDisabled(!video_loaded && !video_stabilized);
    if (ImGui::Button("Stabilize")) {
      log::instance()->add_log("Stabilizing video...\n");
      // TODO: Add a progress bar for stabilization
      // TODO: Add callback function to update progress bar
      // TODO: Add callback function to update logger

      // TODO: stabilize video in another thread, temporarily disable
      // appropriate buttons
      video_stabilized = app::video.stabilize();
      if (video_stabilized) {
        log::instance()->add_log("Video stabilized successfully!\n");
        // TODO: Display stabilized video?
      } else {
        log::instance()->add_log(
            "There was a problem stabilizing your video.\n");
      }
    }
    ImGui::EndDisabled();
    ImGui::SameLine();

    ImGui::BeginDisabled(!video_stabilized);
    if (ImGui::Button("Save")) {
      std::string path;
      if (utils::get_save_directory(path)) {
        if (app::video.export_to_file(path)) {
          log::instance()->add_log("Saving video...\n");
          log::instance()->add_log("Video saved successfully!\n");

          video_loaded = false;
          video_stabilized = false;
        } else {
          log::instance()->add_log("Error: Could not save video.\n");
        }
      }
    }
    ImGui::EndDisabled();

    // TODO: Add stats like the file name, frame rate, resolution, etc.

    ImGui::Spacing();

    //------------------------------------------------------ Logger window --//

    if (ImGui::Button("Help")) {
      // ImGui::OpenPopup("help_popup");
      // TODO: decide whether we like the help popup or help log better
      log::instance()->add_log("This will display the help menu.\n");
    }
    ImGui::SameLine();
    log::instance()->draw();
    ImGui::Spacing();

    //------------------------------------------------------------- Footer --//
    if (ImGui::TextLink("License")) ImGui::OpenPopup("license_popup");

    ImGui::TextLinkOpenURL("View this project on GitHub",
                           "https://github.com/tessapower/video-stabilizer");
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

    // if (ImGui::BeginPopupModal("help_popup", nullptr, popup_flags)) {
    //   ImGui::TextWrapped(
    //       "This will contain helpful instructions for using the program. "
    //       "This will contain helpful instructions for using the program. "
    //       "This will contain helpful instructions for using the program. "
    //       "This will contain helpful instructions for using the program. "
    //       "This will contain helpful instructions for using the program. "
    //       "This will contain helpful instructions for using the program. "
    //       "This will contain helpful instructions for using the program. "
    //       "This will contain helpful instructions for using the program. ");
    //   ImGui::Spacing();

    //  if (centered_button("Close")) ImGui::CloseCurrentPopup();

    //  ImGui::EndPopup();
    //}
  }

  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
}  // namespace gui

#endif  // GUI_H
