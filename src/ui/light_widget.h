#ifndef LIGHT_WIDGET_HPP
#define LIGHT_WIDGET_HPP

#include <format>
#include <string>
#include <imgui/imgui.h>
#include <optional>

#include "scene/light.hpp"
#include "ui/model_widget.h"
#include "ui/widgets.hpp"


namespace
{
    void light_settings(ruya::scene::LightBasic& light)
    {
        ruya::ui::color_widget(light.diffuse, "color");
    }
} 


namespace ruya::ui
{
    /* Widget displaying the light settings.
    Args:
        - ligth: the light component representing light characteristics
        - model: an optional visual representation of the light
    */
    void light_widget(int id, LightBasic& light, Model* model)
    {
        ImGui::PushID(id);

        std::string label = std::format("Light {}", id);
        if (ImGui::TreeNode(label.c_str()))
        {
            light_settings(light);
            if (model != nullptr)
                model_widget(*model, std::string("model"));
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}



#endif // LIGHT_WIDGET_HPP