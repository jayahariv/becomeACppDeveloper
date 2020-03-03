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
    std::cout << "\n(" << current_node->x << ", " << current_node->y << ")";
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
    std::sort(open_list.begin(), open_list.end(), [](const RouteModel::Node* n1, const RouteModel::Node* n2) {
        return (n1->h_value + n1->g_value) > (n2->h_value + n2->g_value);
    });

    auto next = open_list.back();
    open_list.pop_back();
    return next;
}


// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    // TODO: Implement your solution here.

    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;

}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;

    // TODO: Implement your solution here.

}