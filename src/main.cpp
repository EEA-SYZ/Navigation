/**
 * @brief 主函数
 * @note 使用ui.hpp模块创建GUI界面
 */
#include "ui.hpp"
#include <iostream>
#include <SFML/Graphics.hpp>
#include "global.hpp"
#include "data_maker.hpp"
#include "data_manager.hpp"
#include "shortest_path_algorithm.hpp"
#include "Shower.hpp"

void create(ui::Screen& screen, int &mapWidthInt, int &mapHeightInt, int &nodeCountInt, int &edgeCountInt)
{
    // 创建主布局容器
    ui::VerticalBox* ver = new ui::VerticalBox;{
        ver->AddTo(&screen);
        ver->SetVPreset(ui::Control::Preset::WRAP_AT_CENTER);
        ver->SetHPreset(ui::Control::Preset::FILL_FROM_CENTER);
        ver->SetHSize(50);
        ver->SetGap(15);
    }

    // 创建标题
    ui::Label* title = new ui::Label("地图参数设置");{
        title->AddTo(ver);
        title->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
    }

    // 地图长
    ui::HorizontalBox* hor = new ui::HorizontalBox;{
        hor->AddTo(ver);
        hor->SetPreset(ui::Control::Preset::FILL_FROM_CENTER);
    }
    ui::InputBox* mapWidth = new ui::InputBox;{
        hor->Add(new ui::Label("地图长: ", ui::Control::Preset::WRAP_AT_CENTER));
        mapWidth->AddTo(hor);
        mapWidth->SetPreset(ui::Control::Preset::FILL_FROM_CENTER);
        mapWidth->SetContentLimit(ui::InputBox::ContentLimit::ALLOW_SPECIAL_CHARACTERS_ONLY);
        mapWidth->SetSpecialCharacters(ui::InputBox::NUMBER);
        mapWidth->SetLengthLimit(6);
        mapWidth->SetText("10000");
    }

    // 地图高
    hor = new ui::HorizontalBox;{
        hor->AddTo(ver);
        hor->SetPreset(ui::Control::Preset::FILL_FROM_CENTER);
    }
    ui::InputBox* mapHeight = new ui::InputBox;{
        hor->Add(new ui::Label("地图高: ", ui::Control::Preset::WRAP_AT_CENTER));
        mapHeight->AddTo(hor);
        mapHeight->SetPreset(ui::Control::Preset::FILL_FROM_CENTER);
        mapHeight->SetContentLimit(ui::InputBox::ContentLimit::ALLOW_SPECIAL_CHARACTERS_ONLY);
        mapHeight->SetSpecialCharacters(ui::InputBox::NUMBER);
        mapHeight->SetLengthLimit(6);
        mapHeight->SetText("10000");
    }

    // 节点数
    hor = new ui::HorizontalBox;{
        hor->AddTo(ver);
        hor->SetPreset(ui::Control::Preset::FILL_FROM_CENTER);
    }
    ui::InputBox* nodeCount = new ui::InputBox;{
        hor->Add(new ui::Label("节点数: ", ui::Control::Preset::WRAP_AT_CENTER));
        nodeCount->AddTo(hor);
        nodeCount->SetPreset(ui::Control::Preset::FILL_FROM_CENTER);
        nodeCount->SetContentLimit(ui::InputBox::ContentLimit::ALLOW_SPECIAL_CHARACTERS_ONLY);
        nodeCount->SetSpecialCharacters(ui::InputBox::NUMBER);
        nodeCount->SetLengthLimit(6);
        nodeCount->SetText("10000");
    }

    // 边数
    hor = new ui::HorizontalBox;{
        hor->AddTo(ver);
        hor->SetPreset(ui::Control::Preset::FILL_FROM_CENTER);
    }
    ui::InputBox* edgeCount = new ui::InputBox;{
        hor->Add(new ui::Label("连边数: ", ui::Control::Preset::WRAP_AT_CENTER));
        edgeCount->AddTo(hor);
        edgeCount->SetPreset(ui::Control::Preset::FILL_FROM_CENTER);
        edgeCount->SetContentLimit(ui::InputBox::ContentLimit::ALLOW_SPECIAL_CHARACTERS_ONLY);
        edgeCount->SetSpecialCharacters(ui::InputBox::NUMBER);
        edgeCount->SetLengthLimit(6);
        edgeCount->SetText("18000");
    }

    // 确认按钮
    ui::Button* confirmBtn = new ui::Button;{
        confirmBtn->AddTo(ver);
        confirmBtn->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
        confirmBtn->SetCaption("确认");
    }
    ui::Label* confirmLabel = new ui::Label("请正确输入！");{
        confirmLabel->AddTo(ver);
        confirmLabel->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
        confirmLabel->SetVisible(false);
        confirmLabel->SetFontColor(sf::Color::Red);
    }

    bool isConfirm = false;
    // 设置按钮回调
    confirmBtn->SetClickCallback([&](const std::string&, const sf::Event&) {
        if (
            mapWidth->GetText().empty() ||
            mapHeight->GetText().empty() ||
            nodeCount->GetText().empty() ||
            edgeCount->GetText().empty()
        ) {
            confirmLabel->SetVisible(true);
            return;
        }
        mapWidthInt = std::stoi(mapWidth->GetText());
        mapHeightInt = std::stoi(mapHeight->GetText());
        nodeCountInt = std::stoi(nodeCount->GetText());
        edgeCountInt = std::stoi(edgeCount->GetText());
        isConfirm = true;
    });

    // 主循环
    while (screen.IsOpen()) {
        screen.Tick();
        screen.Draw();
        if (isConfirm) {
            break;
        }
    }

    screen.FreeAll();
}

void showing(ui::Screen& screen, int mapWidthInt, int mapHeightInt, int nodeCountInt, int edgeCountInt)
{
    // 初始化起点和终点
    const Node* startNode = nullptr;
    const Node* endNode = nullptr;
    
    // 创建顶部信息栏布局
    auto* topBar = new ui::VerticalBox;{
        topBar->AddTo(&screen);
        topBar->SetVPreset(ui::Control::Preset::FILL_FROM_CENTER);
        topBar->SetHPreset(ui::Control::Preset::FILL_FROM_CENTER);
        topBar->SetVSize(50);
    }

    // 创建标签显示起点和终点
    ui::Label* startLabel = new ui::Label("起点: 未选择");{
        startLabel->AddTo(topBar);
        startLabel->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
    }
    ui::Label* endLabel = new ui::Label("终点: 未选择");{
        endLabel->AddTo(topBar);
        endLabel->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
    }
    
    // 创建按钮容器
    ui::HorizontalBox* btnBox = new ui::HorizontalBox;{
        btnBox->AddTo(topBar);
        btnBox->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
        btnBox->SetGap(15);
    }
    
    // 计算距离最短路按钮
    ui::Button* distanceBtn = new ui::Button;{
        distanceBtn->AddTo(btnBox);
        distanceBtn->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
        distanceBtn->SetCaption("距离最短");
        distanceBtn->SetClickCallback([&](const std::string&, const sf::Event&) {
            // 空实现，后续补充
        });
    }
    
    // 计算时间最短路按钮
    ui::Button* timeBtn = new ui::Button;{
        timeBtn->AddTo(btnBox);
        timeBtn->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
        timeBtn->SetCaption("时间最短");
        timeBtn->SetClickCallback([&](const std::string&, const sf::Event&) {
            // 空实现，后续补充
        });
    }
    
    // 交换起点终点按钮
    ui::Button* swapBtn = new ui::Button;{
        swapBtn->AddTo(btnBox);
        swapBtn->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
        swapBtn->SetCaption("交换");
        swapBtn->SetClickCallback([&](const std::string&, const sf::Event&) {
            std::swap(startNode, endNode);
            // 更新标签显示
            if (startNode) {
                startLabel->SetContent("起点: (" + std::to_string(static_cast<int>(startNode->x)) + ", " + std::to_string(static_cast<int>(startNode->y)) + ")");
            } else {
                startLabel->SetContent("起点: 未选择");
            }
            if (endNode) {
                endLabel->SetContent("终点: (" + std::to_string(static_cast<int>(endNode->x)) + ", " + std::to_string(static_cast<int>(endNode->y)) + ")");
            } else {
                endLabel->SetContent("终点: 未选择");
            }
        });
    }
    
    // 取消选中按钮
    ui::Button* cancelBtn = new ui::Button;{
        cancelBtn->AddTo(btnBox);
        cancelBtn->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
        cancelBtn->SetCaption("取消选中");
        cancelBtn->SetClickCallback([&](const std::string&, const sf::Event&) {
            startNode = nullptr;
            endNode = nullptr;
            startLabel->SetContent("起点: 未选择");
            endLabel->SetContent("终点: 未选择");
        });
    }

    static Shower shower(mapWidthInt, mapHeightInt, nodeCountInt, edgeCountInt);
    
    // 设置节点点击回调
    shower.SetNodeClickCallback([&](const Node* node) {
        // 如果点击的是当前选中的起点，则取消起点选中
        if (startNode == node) {
            startNode = nullptr;
            startLabel->SetContent("起点: 未选择");
        }
        // 如果点击的是当前选中的终点，则取消终点选中
        else if (endNode == node) {
            endNode = nullptr;
            endLabel->SetContent("终点: 未选择");
        }
        // 如果起点未选中，则设置为起点
        else if (!startNode) {
            startNode = node;
            startLabel->SetContent("起点: (" + std::to_string(static_cast<int>(node->x)) + ", " + std::to_string(static_cast<int>(node->y)) + ")");
        }
        // 如果终点未选中，则设置为终点
        else if (!endNode) {
            endNode = node;
            endLabel->SetContent("终点: (" + std::to_string(static_cast<int>(node->x)) + ", " + std::to_string(static_cast<int>(node->y)) + ")");
        }
        // 如果都已选中，设置为新起点，清除终点
        else {
            startNode = node;
            startLabel->SetContent("起点: (" + std::to_string(static_cast<int>(node->x)) + ", " + std::to_string(static_cast<int>(node->y)) + ")");
            endNode = nullptr;
            endLabel->SetContent("终点: 未选择");
        }
    });
    
    while (screen.IsOpen() && shower.IsOpen()) {
        screen.Tick();
        screen.Draw();
        shower.Tick(startNode, endNode);
    }

    screen.FreeAll();
}

int main() {
    // 创建窗口
    ui::Screen screen(1200, 800, "地图参数设置");

    int mapWidthInt = 0;
    int mapHeightInt = 0;
    int nodeCountInt = 0;
    int edgeCountInt = 0;
    create(screen, mapWidthInt, mapHeightInt, nodeCountInt, edgeCountInt);
    showing(screen, mapWidthInt, mapHeightInt, nodeCountInt, edgeCountInt);
    
    return 0;
}