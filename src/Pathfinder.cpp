#include "Pathfinder.h"
#include "Map.h"
#include <iostream>

// 우선순위 큐에서 사용할 비교 구조체
struct CompareNode {
    bool operator()(const Node* a, const Node* b) const {
        return a->fCost > b->fCost;
    }
};

Pathfinder::Pathfinder() {}

Pathfinder::~Pathfinder() {}

std::vector<Node*> Pathfinder::findPath(int startX, int startY, int endX, int endY, Map* map) {
    // 시작점과 끝점이 벽이면 빈 경로 반환
    if (map->isWall(startX, startY) || map->isWall(endX, endY)) {
        return {};
    }

    // 열린 목록(방문할 노드)과 닫힌 목록(방문한 노드)
    std::priority_queue<Node*, std::vector<Node*>, CompareNode> openList;
    std::map<int, Node*> allNodes; // 모든 생성된 노드를 관리하여 메모리 해제

    Node* startNode = new Node(startX, startY);
    startNode->hCost = calculateHeuristic(startX, startY, endX, endY);
    startNode->fCost = startNode->gCost + startNode->hCost;
    
    openList.push(startNode);
    allNodes[startY * map->getWidth() + startX] = startNode;

    std::vector<Node*> path;

    while (!openList.empty()) {
        Node* currentNode = openList.top();
        openList.pop();

        // 목적지에 도달한 경우
        if (currentNode->x == endX && currentNode->y == endY) {
            path = reconstructPath(currentNode);
            break;
        }

        // 8방향 이웃 노드 탐색 (상하좌우 + 대각선)
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;

                int neighborX = currentNode->x + dx;
                int neighborY = currentNode->y + dy;

                // 맵 범위를 벗어나거나 벽인 경우 무시
                if (neighborX < 0 || neighborX >= map->getWidth() || neighborY < 0 || neighborY >= map->getHeight() || map->isWall(neighborX, neighborY)) {
                    continue;
                }
                
                // 대각선 이동 시, 양 옆이 벽으로 막혀있으면 통과하지 못하도록 처리
                if (std::abs(dx) == 1 && std::abs(dy) == 1) {
                    if (map->isWall(currentNode->x + dx, currentNode->y) || map->isWall(currentNode->x, currentNode->y + dy)) {
                        continue;
                    }
                }

                float moveCost = (dx == 0 || dy == 0) ? 1.0f : 1.414f; // 직선 1, 대각선 sqrt(2)
                float newGCost = currentNode->gCost + moveCost;

                Node* neighborNode = nullptr;
                int neighborIndex = neighborY * map->getWidth() + neighborX;

                if (allNodes.find(neighborIndex) == allNodes.end() || newGCost < allNodes[neighborIndex]->gCost) {
                    if (allNodes.find(neighborIndex) != allNodes.end()) {
                        neighborNode = allNodes[neighborIndex];
                    } else {
                        neighborNode = new Node(neighborX, neighborY, currentNode);
                        allNodes[neighborIndex] = neighborNode;
                    }
                    
                    neighborNode->parent = currentNode;
                    neighborNode->gCost = newGCost;
                    neighborNode->hCost = calculateHeuristic(neighborX, neighborY, endX, endY);
                    neighborNode->fCost = neighborNode->gCost + neighborNode->hCost;
                    
                    openList.push(neighborNode);
                }
            }
        }
    }

    // 메모리 정리
    for (auto const& [key, val] : allNodes) {
        if (std::find(path.begin(), path.end(), val) == path.end()) {
            delete val;
        }
    }

    return path;
}

float Pathfinder::calculateHeuristic(int x1, int y1, int x2, int y2) {
    // 유클리드 거리 사용
    return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}

std::vector<Node*> Pathfinder::reconstructPath(Node* endNode) {
    std::vector<Node*> path;
    Node* currentNode = endNode;
    while (currentNode != nullptr) {
        path.push_back(currentNode);
        currentNode = currentNode->parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}
