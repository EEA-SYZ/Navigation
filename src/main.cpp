/**
 * @brief 主函数
 * @note 使用ui.hpp模块创建GUI界面
 */
#include "ui.hpp"
#include <iostream>
#include <SFML/Graphics.hpp>
#include "data_maker.hpp"
#include "data_manager.hpp"
#include "shortest_path_algorithm.hpp"

// 视口结构体
struct Viewport {
    double left;      // 左边界
    double right;     // 右边界
    double top;       // 上边界
    double bottom;    // 下边界
    int level;        // 当前层级
    Viewport(double l = 0, double r = 10000, double t = 10000, double b = 0, int lv = 0)
        : left(l), right(r), top(t), bottom(b), level(lv) {}
    // 获取视口宽度
    double getWidth() const { return right - left; }
    // 获取视口高度
    double getHeight() const { return top - bottom; }
};

void showMap(int mapWidth, int mapHeight, int nodeCount, int edgeCount) {
    // 创建数据
    DataMaker dataMaker(0, mapWidth, 0, mapHeight, nodeCount, edgeCount);
    DataManager dataManager(dataMaker.getGraph());
    ShortestPathAlgorithm shortestPathAlgorithm(dataMaker.getGraph());
    shortestPathAlgorithm.setFlowQueryInterface([&dataMaker](const Edge *edge) {
        return dataMaker.queryCurrentFlowInEdge(edge);
    });
    // 创建SFML窗口
    sf::RenderWindow window(sf::VideoMode(1200, 800), "地图可视化");
    window.setFramerateLimit(60);
    // 初始化视口（显示整个图区域）
    Viewport viewport(0, mapWidth, mapHeight, 0, 0);
    // 主循环
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // 滚轮缩放事件
            if (event.type == sf::Event::MouseWheelScrolled) {
                // 计算缩放中心（鼠标位置对应的图坐标）
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                double graphX = viewport.left + (static_cast<double>(mousePos.x) / window.getSize().x) * viewport.getWidth();
                double graphY = viewport.bottom + (static_cast<double>(mousePos.y) / window.getSize().y) * viewport.getHeight();
                // 缩放因子（每次缩放10%）
                const double zoomDelta = 0.1;
                double zoomFactor = (event.mouseWheelScroll.delta > 0) ? (1.0 - zoomDelta) : (1.0 + zoomDelta);
                // 计算新的视口大小
                double newWidth = viewport.getWidth() * zoomFactor;
                double newHeight = viewport.getHeight() * zoomFactor;
                // 限制视口最小和最大尺寸
                const double minSize = 100;
                const double maxSize = std::max(mapWidth, mapHeight) * 2;
                newWidth = std::max(minSize, std::min(maxSize, newWidth));
                newHeight = std::max(minSize, std::min(maxSize, newHeight));
                // 以鼠标位置为中心进行缩放
                double ratioX = (graphX - viewport.left) / viewport.getWidth();
                double ratioY = (graphY - viewport.bottom) / viewport.getHeight();
                viewport.left = graphX - ratioX * newWidth;
                viewport.right = viewport.left + newWidth;
                viewport.bottom = graphY - ratioY * newHeight;
                viewport.top = viewport.bottom + newHeight;
                // 确保视口边界不超出图的范围
                viewport.left = std::max(0.0, viewport.left);
                viewport.right = std::min(static_cast<double>(mapWidth), viewport.right);
                viewport.bottom = std::max(0.0, viewport.bottom);
                viewport.top = std::min(static_cast<double>(mapHeight), viewport.top);
            }
        }
        // 清空窗口
        window.clear(sf::Color::White);
        // 查询当前视口内的图数据
        Graph visibleGraph = dataManager.queryDataInViewport(
            static_cast<int>(viewport.left),
            static_cast<int>(viewport.right),
            static_cast<int>(viewport.top),
            static_cast<int>(viewport.bottom),
            viewport.level
        );
        // 计算坐标转换比例
        double scaleX = window.getSize().x / viewport.getWidth();
        double scaleY = window.getSize().y / viewport.getHeight();
        // 绘制边
        for (const Edge* edge : visibleGraph.second) {
            // 将图坐标转换为窗口坐标
            sf::Vector2f start(
                (edge->from->x - viewport.left) * scaleX,
                (viewport.top - edge->from->y) * scaleY
            );
            sf::Vector2f end(
                (edge->to->x - viewport.left) * scaleX,
                (viewport.top - edge->to->y) * scaleY
            );
            // 创建边的线条
            sf::Vertex line[] = {
                sf::Vertex(start, sf::Color::Black),
                sf::Vertex(end, sf::Color::Black)
            };
            window.draw(line, 2, sf::Lines);
        }
        // 绘制节点
        for (const Node* node : visibleGraph.first) {
            // 将图坐标转换为窗口坐标
            sf::Vector2f position(
                (node->x - viewport.left) * scaleX,
                (viewport.top - node->y) * scaleY
            );
            // 创建节点圆
            sf::CircleShape circle(3);
            circle.setFillColor(sf::Color::Blue);
            circle.setPosition(position - sf::Vector2f(3, 3));
            window.draw(circle);
        }
        // 显示绘制内容
        window.display();
    }
}

int main() {
    // 创建窗口
    ui::Screen screen(1200, 800, "地图参数设置");

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
        edgeCount->SetText("25000");
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
    int mapWidthInt = 0;
    int mapHeightInt = 0;
    int nodeCountInt = 0;
    int edgeCountInt = 0;
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
    
    showMap(mapWidthInt, mapHeightInt, nodeCountInt, edgeCountInt);
    
    return 0;
}