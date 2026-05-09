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
    sf::RenderWindow window(sf::VideoMode(1200, 800), L"地图可视化");
    window.setFramerateLimit(60);
    // 初始化视口（显示整个图区域）
    Viewport viewport(0, window.getSize().x, window.getSize().y, 0, 0);
    double oldWidth = window.getSize().x;
    double oldHeight = window.getSize().y;
    
    // 拖拽相关变量
    bool isDragging = false;
    sf::Vector2i lastMousePos;
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
                
                // 窗口坐标转图坐标（注意窗口Y轴是从顶部开始，图坐标Y轴是从底部开始）
                double ratioX = static_cast<double>(mousePos.x) / window.getSize().x;
                double ratioY = static_cast<double>(mousePos.y) / window.getSize().y;
                double graphX = viewport.left + ratioX * viewport.getWidth();
                double graphY = viewport.top - ratioY * viewport.getHeight(); // 关键修正：从top向下计算
                
                // 缩放因子（每次缩放10%）
                const double zoomDelta = 0.1;
                double zoomFactor = (event.mouseWheelScroll.delta > 0) ? (1.0 - zoomDelta) : (1.0 + zoomDelta);
                
                // 计算新的视口大小
                double newWidth = viewport.getWidth() * zoomFactor;
                double newHeight = viewport.getHeight() * zoomFactor;
                
                // 限制视口最小和最大尺寸
                double minSizeX = window.getSize().x * 0.2;
                double minSizeY = window.getSize().y * 0.2;
                double maxSizeX = window.getSize().x * 2;
                double maxSizeY = window.getSize().y * 2;
                newWidth = std::max(minSizeX, std::min(maxSizeX, newWidth));
                newHeight = std::max(minSizeY, std::min(maxSizeY, newHeight));
                
                // 以鼠标位置为中心进行缩放（保持鼠标位置不变）
                viewport.left = graphX - ratioX * newWidth;
                viewport.right = viewport.left + newWidth;
                viewport.top = graphY + ratioY * newHeight;
                viewport.bottom = viewport.top - newHeight;
                
                // 确保视口边界不超出图的范围
                viewport.left = std::max(-static_cast<double>(window.getSize().x / 2), viewport.left);
                viewport.right = std::min(static_cast<double>(mapWidth), viewport.right);
                viewport.bottom = std::max(-static_cast<double>(window.getSize().y / 2), viewport.bottom);
                viewport.top = std::min(static_cast<double>(mapHeight), viewport.top);
            }
            
            // 窗口大小改变事件
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));

                double newWidth = viewport.getWidth() * (window.getSize().x / static_cast<double>(oldWidth));
                double newHeight = viewport.getHeight() * (window.getSize().y / static_cast<double>(oldHeight));
                auto nleft = viewport.left - (newWidth - viewport.getWidth()) / 2;
                auto nright = viewport.right + (newWidth - viewport.getWidth()) / 2;
                auto ntop = viewport.top + (newHeight - viewport.getHeight()) / 2;
                auto nbottom = viewport.bottom - (newHeight - viewport.getHeight()) / 2;
                viewport.left = nleft;
                viewport.right = nright;
                viewport.top = ntop;
                viewport.bottom = nbottom;
                oldWidth = window.getSize().x;
                oldHeight = window.getSize().y;
            }
            
            // 鼠标按下事件
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isDragging = true;
                    lastMousePos = sf::Mouse::getPosition(window);
                }
            }
            
            // 鼠标移动事件（拖拽平移）
            if (event.type == sf::Event::MouseMoved && isDragging) {
                sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);
                
                // 计算鼠标移动的偏移量
                int deltaX = currentMousePos.x - lastMousePos.x;
                int deltaY = currentMousePos.y - lastMousePos.y;
                
                // 将像素偏移转换为图坐标偏移
                double graphDeltaX = -deltaX * (viewport.getWidth() / window.getSize().x);
                double graphDeltaY = deltaY * (viewport.getHeight() / window.getSize().y);
                
                // 更新视口位置
                viewport.left += graphDeltaX;
                viewport.right += graphDeltaX;
                viewport.top += graphDeltaY;
                viewport.bottom += graphDeltaY;
                
                // 更新鼠标位置
                lastMousePos = currentMousePos;
            }
            
            // 鼠标释放事件
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isDragging = false;
                }
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
        // 计算线条和节点的尺寸（根据缩放比例自适应）
        float lineThickness = std::max(1.5f, static_cast<float>(scaleX * 2));
        float nodeRadius = std::max(6.0f, static_cast<float>(scaleX * 10));
        float outlineThickness = std::max(2.0f, static_cast<float>(scaleX * 3));

        // 绘制边（使用矩形条带模拟粗线）
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

            // 计算边的方向和垂直方向
            sf::Vector2f direction = end - start;
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            
            // 计算边的起点和终点（缩进节点半径，避免边进入节点内部）
            sf::Vector2f normalizedDir = direction / length;
            sf::Vector2f adjustedStart = start + normalizedDir * nodeRadius;
            sf::Vector2f adjustedEnd = end - normalizedDir * nodeRadius;
            sf::Vector2f adjustedDirection = adjustedEnd - adjustedStart;
            float adjustedLength = std::max(0.0f, std::sqrt(
                adjustedDirection.x * adjustedDirection.x + 
                adjustedDirection.y * adjustedDirection.y
            ));

            // 创建粗线条（使用矩形）
            sf::RectangleShape lineShape(sf::Vector2f(adjustedLength, lineThickness));
            lineShape.setFillColor(sf::Color(100, 100, 100)); // 灰色边
            lineShape.setPosition(adjustedStart);
            lineShape.setRotation(std::atan2(adjustedDirection.y, adjustedDirection.x) * 180 / PI);
            lineShape.setOrigin(0, lineThickness / 2);

            window.draw(lineShape);
        }

        // 绘制节点（空心圆球，后绘制以覆盖边）
        for (const Node* node : visibleGraph.first) {
            // 将图坐标转换为窗口坐标
            sf::Vector2f position(
                (node->x - viewport.left) * scaleX,
                (viewport.top - node->y) * scaleY
            );

            // 创建空心节点圆
            sf::CircleShape circle(nodeRadius);
            circle.setFillColor(sf::Color::Transparent); // 空心，透明填充
            circle.setOutlineColor(sf::Color(30, 80, 180)); // 深蓝色边框
            circle.setOutlineThickness(outlineThickness); // 边框厚度（自适应缩放）
            circle.setPosition(position - sf::Vector2f(nodeRadius, nodeRadius));

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
            screen.Close();
        }
    }
    
    showMap(mapWidthInt, mapHeightInt, nodeCountInt, edgeCountInt);
    
    return 0;
}