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
            {
                Model& model_ref = *model;
                model_widget(model_ref, std::string("model"));
                
                // TODO: update if Model gets its own Transform variable
                // update light position to be the avg of element positions
                light.position = glm::vec3(0.0f);
                for (auto element : model_ref.elements)
                {
                    light.position += element.transform.position;
                }
                light.position /= model_ref.elements.size();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}



#endif // LIGHT_WIDGET_HPP