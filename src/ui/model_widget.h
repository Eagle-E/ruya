#ifndef OBJECT_WIDGET_HPP
#define OBJECT_WIDGET_HPP

#include <format>
#include <string>
#include <ranges>
#include <imgui/imgui.h>

#include "scene/components.hpp"

using ruya::scene::Model;
using ruya::scene::Element;

namespace
{
    static inline void obj_vec3_float_widget(vec3 & vec, float step = 1.0f, const std::string& label = {})
    {
        ImGui::DragFloat3(label.c_str(), &vec.x, step);
    }

    inline void transform_settings(Transform& transform)
    {
        obj_vec3_float_widget(transform.position, 0.1f, "position");
        obj_vec3_float_widget(transform.scale, 0.1f, "scale");
        obj_vec3_float_widget(transform.rotation, 1.0f, "rotation");
    }

    inline void element_settings(Element& elem)
    {
        transform_settings(elem.transform);
    }

    inline void element_list_settings(std::vector<Element>& elem_list)
    {
        for (auto [i, elem] : std::views::enumerate(elem_list))
        {
            ImGui::PushID(i);

            // TODO WARNING: if the list of elements is mutated, the ui won't be in sync!!!
            std::string element_id = std::format("Element {}", i);
            
            if (ImGui::TreeNode(element_id.c_str()))
            {
                element_settings(elem);
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
    }
} 


namespace ruya::ui
{
    void model_widget(Model& model, const std::string& label)
    {
        ImGui::PushID(label.c_str());

        // TODO WARNING: if the list of elements is mutated, the ui won't be in sync!!!
        if (ImGui::TreeNode(label.c_str()))
        {
            element_list_settings(model.elements);
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
}



#endif // OBJECT_WIDGET_HPP