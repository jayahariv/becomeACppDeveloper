#include "route_planner.h"
#include <algorithm>
#include <iostream>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    start_node = &(m_Model.FindClosestNode(start_x, start_y));
    end_node = &(m_Model.FindClosestNode(end_x, end_y));

#ifdef ENABLE_LOG
    std::cout << "\n>> Find Closest Node ";
    std::cout << "\nStart = (" << start_node->x << ", " << start_node->y << ")";
    std::cout << "\nEnd = (" << end_node->x << ", " << end_node->y << ")\n";
#endif
}

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    return node->distance(*end_node);
}

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
#ifdef ENABLE_LOG
    std::cout << "\n>> Adding Neighbors";
    std::cout << "\n(x/" << current_node->x << ", y/" << current_node->y << ", h/" << current_node->h_value << ", g/" << current_node->g_value << ")\n";
#endif
    
    current_node->FindNeighbors();
    for (int idx = 0; idx < current_node->neighbors.size(); idx++) {
        const RouteModel::Node *neighbor = current_node->neighbors[idx];
        if (neighbor->visited)
            continue;
        current_node->neighbors[idx]->parent = current_node;
        current_node->neighbors[idx]->h_value = CalculateHValue(neighbor);
        current_node->neighbors[idx]->g_value = current_node->g_value + (current_node->distance(*current_node->neighbors[idx]));
        current_node->neighbors[idx]->visited = true;

        open_list.push_back(current_node->neighbors[idx]);
#ifdef ENABLE_LOG
        std::cout << "\t=> (x/" << neighbor->x << ", y/" << neighbor->y << ", g/" << neighbor->g_value << ", h/" << neighbor->h_value << ")\n";
#endif
    }
}

RouteModel::Node *RoutePlanner::NextNode() {
#ifdef ENABLE_LOG
    std::cout << "\n>> NextNode";
#endif
    std::sort(open_list.begin(), open_list.end(), [](const RouteModel::Node* n1, const RouteModel::Node* n2) {
        return (n1->h_value + n1->g_value) > (n2->h_value + n2->g_value);
    });

    if (open_list.size() == 0) {
#ifdef ENABLE_LOG
    std::cout << "\tOpen list became empty!";
#endif
        return nullptr;
    }
    
    auto next = open_list.back();
    open_list.pop_back();
#ifdef ENABLE_LOG
    std::cout << "\nNext = (x/" << next->x << ", y/" << next->y << ", h/" << next->h_value << ", g/" << next->g_value << ")";
#endif
    return next;
}

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;
    RouteModel::Node *current = current_node;

    while (current->x != start_node->x && current->y != start_node->y) {
        if (current->parent != nullptr)
            distance += current->distance(*current->parent);
        path_found.insert(path_found.begin(), *current); // linear complexity
        current = current->parent;
    }
    path_found.insert(path_found.begin(), *current); // start node!!

    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;
}

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = start_node;
    current_node->parent = nullptr;
    current_node->visited = true;
    current_node->h_value = CalculateHValue(start_node);

    while (current_node != nullptr) {
        AddNeighbors(current_node);
        RouteModel::Node* next = NextNode();

        if (next->x == end_node->x && next->y == end_node->y) {
            m_Model.path = ConstructFinalPath(end_node);
            return;
        }
        current_node = next;
    }
}