#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm> 

class Node {
public:
    int id;
    std::string number;
    std::string name;
    std::string region;

    // Fix : while using Unique_ptr and raw pointer at the same time, I realized that I cannot avoid 'Dangling pointer' issue.
    // Decided to make vector that refers to userID.

    // 댕글링 포인터 문제를 해결하기 위한 ID 참조 vector를 만듬.

    std::vector<int> userIds;
    
    Node(int _id, std::string _number, std::string _name, std::string _region)
        : id(_id), number(std::move(_number)), name(std::move(_name)), region(std::move(_region)) {}
};

using nodeContainer = std::vector<std::unique_ptr<Node>>;

// Fix 2.0 : I literally hated to write the same codes down over and over again.
// Decided to refactor with findNode() Function.

// 코드 단순 반복하는 게 싫어서, findNode() 함수를 만들어 리팩토링 했습니다.

/**
 * @brief Search for a specific node by name within the container.
 * 이름을 기반으로 컨테이너 내에서 특정 노드를 검색합니다.
 * @param nodes Vector of smart pointers containing all user nodes.
 * 모든 사용자 노드가 담긴 스마트 포인터 벡터입니다.
 * @param name The name of the user to look up.
 * 찾고자 하는 사용자의 이름입니다.
 * @return Memory address of the found node, or nullptr if not found.
 * 찾은 노드의 메모리 주소를 반환하며, 없을 경우 nullptr를 반환합니다.
 */

Node* findNode(const nodeContainer& nodes, const std::string& name) {
    for (const auto& n : nodes) {
        if (n->name == name) return n.get();
    }
    return nullptr;
}

/**
 * @brief Display the basic information and relations of all stored nodes.
 * 저장된 모든 노드의 기본 정보와 Relation을 화면에 출력합니다.
 * @param nodes The container of nodes to be displayed.
 * 출력할 노드들이 담긴 컨테이너입니다.
 */

void viewAllNodes(const nodeContainer& nodes) {
    std::cout << "\n========== [ SOCIAL GRAPH VIEW ] ==========" << std::endl;

    // While Tranversing IDs, find names by ID
    // ID 목록을 순회하며 각 ID에 해당하는 이름을 찾아 출력
    for(const auto& current : nodes) {
        // 1. Basic Info Output - 기본 정보 출력
        std::cout << "[ID:" << current->id << "] Name: " << current->name 
                  << " (" << current->region << ") | Tel: " << current->number << std::endl;

        // 2. Relation Output - 관계 출력
        std::cout << "   └─ Relation: ";

        // 3. Data Validation - 데이터 유효성 검사
        
        if (!current->userIds.empty()) {
            std::cout << "connected to -> ";
            for(size_t i = 0; i < current->userIds.size(); ++i) {

                for(const auto& n : nodes) {
                    if(n->id == current->userIds[i]) {
                        std::cout << n->name;
                        break;
                    }
                }
                if (i < current->userIds.size() - 1) std::cout << ", ";
            }
        } else {
            std::cout << "No connections found.";
        }
        std::cout << "\n" << std::endl;
    }
    std::cout << "===========================================\n" << std::endl;
}

/**
 * @brief Remove the friendship (ID reference) between two users.
 * 두 사용자 사이의 친구 관계(ID 참조)를 삭제합니다.
 * @param nodes The container of all nodes.
 * 모든 노드가 담긴 컨테이너입니다.
 * @param nameA Name of the first user to disconnect.
 * 관계를 끊을 첫 번째 사용자의 이름입니다.
 * @param nameB Name of the second user to disconnect.
 * 관계를 끊을 두 번째 사용자의 이름입니다.
 * @return Returns 0 on success, or 1 if a user is not found.
 * 성공 시 0을 반환하고, 사용자를 찾지 못하면 1을 반환합니다.
 */
int removeRelation(nodeContainer& nodes, const std::string& nameA, const std::string& nameB) {
    // Looking up for user's name by using findNode()
    // findNode 함수를 사용하여 사람을 찾습니다.
    Node* nodeA = findNode(nodes, nameA);
    Node* nodeB = findNode(nodes, nameB);

    if (nodeA == nullptr || nodeB == nullptr) {
        std::cout << "User not found." << std::endl;
        return 1;
    }
    
    // Getting rid of NodeB from the list of nodeA - nodeA의 목록에서 nodeB 지우기
    auto itA = std::find(nodeA->userIds.begin(), nodeA->userIds.end(), nodeB->id);
    if (itA != nodeA->userIds.end()) {
        nodeA->userIds.erase(itA);
    }

    // Getting rid of NodeA from the list of nodeB - nodeB의 목록에서 nodeA 지우기
    auto itB = std::find(nodeB->userIds.begin(), nodeB->userIds.end(), nodeA->id);
    if (itB != nodeB->userIds.end()) {
        nodeB->userIds.erase(itB);
    }

    std::cout << "[System] Relationship removed between " << nameA << " and " << nameB << std::endl;
    return 0; 
}

/**
 * @brief Establish a new friendship (ID reference) between two users.
 * 두 사용자 사이의 새로운 친구 관계(ID 참조)를 추가합니다.
 * @param nodes The container of all nodes.
 * 모든 노드가 담긴 컨테이너입니다.
 * @param nameA Name of the first user to connect.
 * 관계를 맺을 첫 번째 사용자의 이름입니다.
 * @param nameB Name of the second user to connect.
 * 관계를 맺을 두 번째 사용자의 이름입니다.
 * @return Returns 0 on success, or 1 on validation failure.
 * 성공 시 0을 반환하며, 유효성 검사 실패 시 1을 반환합니다.
 */
int addRelation(nodeContainer& nodes, const std::string& nameA, const std::string& nameB) {

    Node* nodeA = findNode(nodes, nameA);
    Node* nodeB = findNode(nodes, nameB);
    
    // Validating if data is not found

    if(!nodeA || !nodeB) {
        std::cout << "User not found" << std::endl;
        return 1;
    }

    // 
    if(nameA == nameB) {
        std::cout << "User cannot be friend themselves." << std::endl;
        return 1;
    }
     for(const auto& friendId : nodeA->userIds) {
        if(friendId == nodeB->id) {
            std::cout << "Users already in relation." << std::endl;
            return 1;
        }
    }
    nodeA->userIds.push_back(nodeB->id);
    nodeB->userIds.push_back(nodeA->id);
    
    return 0;
}

int main() {
    nodeContainer allNodes;

    allNodes.push_back(std::make_unique<Node>(1, "010-1234-5678", "Hong", "Seoul")); 
    allNodes.push_back(std::make_unique<Node>(2, "010-2345-6789", "Kim", "Busan")); 
    allNodes.push_back(std::make_unique<Node>(3, "010-9999-8888", "Lee", "Jeju"));

    // Addition Test
    addRelation(allNodes, "Hong", "Kim");
    addRelation(allNodes, "Hong", "Lee");

    // View Function Test
    viewAllNodes(allNodes);

    // Removal Test
    removeRelation(allNodes,"Hong","Lee");
    viewAllNodes(allNodes);
    
    return 0;
}