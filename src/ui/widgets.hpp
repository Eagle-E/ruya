#ifndef WIDGETS_H
#define WIDGETS_H

#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <string>

using glm::vec3;	

namespace ruya::ui
{
    inline void color_widget(vec3& vec, const std::string& label = {})
    {
        ImVec4 im_rgb {vec.x, vec.y, vec.z, 1.0f};

        if (ImGui::ColorButton("Background color", im_rgb))
        {
            ImGui::OpenPopup("Color Picker");
        }
        ImGui::SameLine();
        if (label == "")
            ImGui::Text("color");
        else
            ImGui::Text(label.c_str());

        
        // Color picker popup
        if (ImGui::BeginPopup("Color Picker"))
        {
            ImGui::ColorPicker4(
                "##picker",
                (float*)&im_rgb,
                ImGuiColorEditFlags_DisplayRGB |   // RGB sliders
                ImGuiColorEditFlags_NoSidePreview | // Clean layout
                ImGuiColorEditFlags_AlphaBar |      // Show alpha bar
                ImGuiColorEditFlags_PickerHueWheel  // Hue wheel style [3]
            );
            ImGui::EndPopup();
        }

        vec.x = im_rgb.x;
        vec.y = im_rgb.y;
        vec.z = im_rgb.z;
    }

    inline void vec3_widget(vec3 & vec, float step = 1.0f, const std::string& label = {})
    {
        ImGui::DragFloat3(label.c_str(), &vec.x, step);
    }
}


#endif // WIDGETS_H