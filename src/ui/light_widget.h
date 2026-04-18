#ifndef LIGHT_WIDGET_HPP
#define LIGHT_WIDGET_HPP

#include <format>
#include <string>
#include <imgui/imgui.h>

#include "scene/light.hpp"
#include "ui/object_widget.h"
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
    void light_widget(LightBasic& light)
    {
        ImGui::PushID(light.id.value);

        // TODO: update
        std::string label = std::format("Light {}", light.id.value);
        if (ImGui::TreeNode(label.c_str()))
        {
            light_settings(light);
            model_widget(light.model, std::string("model"));
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}



#endif // LIGHT_WIDGET_HPP