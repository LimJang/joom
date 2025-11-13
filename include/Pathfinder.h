#pragma once

#include <vector>
#include <cmath>
#include <queue>
#include <map>
#include <algorithm>

// 맵 클래스 전방 선언
class Map;

// A* 알고리즘에 사용될 노드 구조체
struct Node {
    int x, y;
    float gCost; // 시작점으로부터의 비용
    float hCost; // 목적지까지의 예상 비용 (Heuristic)
    float fCost; // gCost + hCost
    Node* parent;

    Node(int x, int y, Node* parent = nullptr)
        : x(x), y(y), gCost(0), hCost(0), fCost(0), parent(parent) {}

    // fCost를 기준으로 노드를 비교하기 위한 연산자
    bool operator>(const Node& other) const {
        return fCost > other.fCost;
    }
};

class Pathfinder {
public:
    Pathfinder();
    ~Pathfinder();

    // A* 알고리즘을 사용하여 경로를 찾는 메인 함수
    std::vector<Node*> findPath(int startX, int startY, int endX, int endY, Map* map);

private:
    // 목적지까지의 휴리스틱 비용 계산 (유클리드 거리)
    float calculateHeuristic(int x1, int y1, int x2, int y2);

    // 경로를 역추적하여 생성하는 함수
    std::vector<Node*> reconstructPath(Node* endNode);
};
