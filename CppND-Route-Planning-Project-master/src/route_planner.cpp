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
    return node->distance((*end_node));
}

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
#ifdef ENABLE_LOG
    std::cout << "\n>> Adding Neighbors";
    std::cout << "\n(" << current_node->x << ", " << current_node->y << ", " << current_node->h_value << ", " << current_node->g_value << ")\n";
#endif
    current_node->FindNeighbors();
    int neighborCount = current_node->neighbors.size();
    for (int idx = 0; idx < neighborCount; idx++) {
        const RouteModel::Node *node = current_node->neighbors[idx];
        current_node->neighbors[idx]->parent = current_node;
        current_node->neighbors[idx]->h_value = CalculateHValue(node);
        current_node->neighbors[idx]->g_value = current_node->g_value + 1;
        current_node->neighbors[idx]->visited = true;

        open_list.push_back(current_node->neighbors[idx]);
#ifdef ENABLE_LOG
        std::cout << "\t=> (" << node->x << ", " << node->y << ")";
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
    std::cout << "\t(" << next->x << ", " << next->y << ", " << next->h_value << ", " << next->g_value << "): open-list count = " << open_list.size();
#endif
    return next;
}

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
#ifdef ENABLE_LOG
    std::cout << "\n>> ConstructFinalPath";
#endif
    
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;
    RouteModel::Node *current = current_node;

    while (current->parent != nullptr) {
        distance += current->distance(*current->parent);
        path_found.insert(path_found.begin(), *current); // linear complexity
        current = current_node->parent;
    }
    path_found.insert(path_found.begin(), *current);

    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
#ifdef ENABLE_LOG
    std::cout << "\tDistance" << distance;
#endif
    return path_found;

}

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = start_node;

    if (start_node->x == end_node->x && start_node->y == end_node->y) {
#ifdef ENABLE_LOG
            std::cout << "\nStart and End are same. Found Node!!\n";
#endif
            m_Model.path = ConstructFinalPath(end_node);
            return;
    }

    while (current_node != nullptr) {
        AddNeighbors(current_node);
        RouteModel::Node* next = NextNode();

        if (next->x == end_node->x && next->y == end_node->y) {
            m_Model.path = ConstructFinalPath(end_node);
#ifdef ENABLE_LOG
            std::cout << "\n(" << next->x << ", " << next->y << ") = " << "(" << end_node->x << ", " << end_node->y << ") \tFound Node!!\n";
#endif
            return;
        }

        current_node = next;
    }
}