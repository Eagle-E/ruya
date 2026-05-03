#ifndef LIGHT_WIDGET_HPP
#define LIGHT_WIDGET_HPP

#include <format>
#include <string>
#include <imgui/imgui.h>
#include <optional>

#include "scene/light.hpp"
#include "ui/model_widget.h"
#include "ui/widgets.hpp"

using ruya::scene::BasicLight;
using ruya::scene::DirectionalLight;
using ruya::scene::PointLight;

namespace
{
    void basic_light_settings(ruya::scene::BasicLight& light)
    {
        ruya::ui::vec3_widget(light.position, 0.1f, "position");
        ImGui::PushID("ambient");
        ruya::ui::color_widget(light.ambient, "ambient");
        ImGui::PopID();
        ImGui::PushID("diffuse");
        ruya::ui::color_widget(light.diffuse, "diffuse");
        ImGui::PopID();
        ImGui::PushID("specular");
        ruya::ui::color_widget(light.specular, "specular");
        ImGui::PopID();
    }

    void directional_light_settings(ruya::scene::DirectionalLight& light)
    {
        ruya::ui::vec3_widget(light.direction, 0.1f, "direction");
        ImGui::PushID("ambient");
        ruya::ui::color_widget(light.ambient, "ambient");
        ImGui::PopID();
        ImGui::PushID("diffuse");
        ruya::ui::color_widget(light.diffuse, "diffuse");
        ImGui::PopID();
        ImGui::PushID("specular");
        ruya::ui::color_widget(light.specular, "specular");
        ImGui::PopID();
    }

    void point_light_settings(ruya::scene::PointLight& light)
    {
        ruya::ui::vec3_widget(light.position, 0.1f, "position");
        ImGui::PushID("ambient");
        ruya::ui::color_widget(light.ambient, "ambient");
        ImGui::PopID();
        ImGui::PushID("diffuse");
        ruya::ui::color_widget(light.diffuse, "diffuse");
        ImGui::PopID();
        ImGui::PushID("specular");
        ruya::ui::color_widget(light.specular, "specular");
        ImGui::PopID();
        ImGui::PushID("constant");
        ruya::ui::float_widget(light.constant, 0.05f, "constant");
        ImGui::PopID();
        ImGui::PushID("linear");
        ruya::ui::float_widget(light.linear, 0.01f, "linear");
        ImGui::PopID();
        ImGui::PushID("quadratic");
        ruya::ui::float_widget(light.quadratic, 0.0025f, "quadratic");
        ImGui::PopID();
    }
    
} 


namespace ruya::ui
{
    /* Widget displaying the light settings.
    Args:
        - ligth: the light component representing light characteristics
        - model: an optional visual representation of the light
    */
    void light_widget(int id, BasicLight& light, Model* model)
    {
        ImGui::PushID(id);

        std::string label = std::format("Basic Light {}", id);
        if (ImGui::TreeNode(label.c_str()))
        {
            basic_light_settings(light);
            if (model != nullptr)
            {
                Model& model_ref = *model;
                model_widget(model_ref, std::string("model"));
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    void light_widget(int id, DirectionalLight& light)
    {
        ImGui::PushID(id);

        std::string label = std::format("Dir Light {}", id);
        if (ImGui::TreeNode(label.c_str()))
        {
            directional_light_settings(light);
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    void light_widget(int id, PointLight& light, Model* model)
    {
        ImGui::PushID(id);

        std::string label = std::format("Point Light {}", id);
        if (ImGui::TreeNode(label.c_str()))
        {
            point_light_settings(light);
            if (model != nullptr)
            {
                Model& model_ref = *model;
                model_widget(model_ref, std::string("model"));
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}



#endif // LIGHT_WIDGET_HPP