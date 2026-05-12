#ifndef __SHOWER_HPP__
#define __SHOWER_HPP__

#define __FF(F, G, B) F {B} G {B}
#define ___FF(F, G, H, B) F {B} G {B} H {B}
#define ____FF(F, G, H, I, B) F {B} G {B} H {B} I {B}

#include <functional>
#include <algorithm>

// 视口结构体
struct Viewport {
    double left;      // 左边界
    double right;     // 右边界
    double top;       // 上边界
    double bottom;    // 下边界
    int level;        // 当前层级
    int maxLevel;     // 最大层级数
    
    double getWidth() const { return right - left; }
    double getHeight() const { return top - bottom; }
    void updateLevel(double t) { 
        const auto &b = 1.4;
        for (int i = maxLevel - 1; i > 0; --i) {
            if (t > std::pow(std::pow(b, i), 2)) {
                level = i;
                return;
            }
        }
        level = 0;
    }
};
// Shower类 - 封装地图显示功能
class Shower {
public:
    Shower(int mapWidth, int mapHeight, int nodeCount, int edgeCount, int levelNum = 5, int levelVolume = 50) : mapWidth(mapWidth), mapHeight(mapHeight), isDragging(false) 
    {
        // 创建数据对象
        dataMaker = new DataMaker(0, mapWidth, 0, mapHeight, nodeCount, edgeCount, levelNum, levelVolume);
        dataManager = new DataManager(dataMaker->getGraph());
        shortestPathAlgorithm = new ShortestPathAlgorithm(dataMaker->getGraph());
        shortestPathAlgorithm->setFlowQueryInterface([this](const Edge *edge) {
            return dataMaker->queryCurrentFlowInEdge(edge);
        });
        
        // 创建SFML窗口
        window = new sf::RenderWindow(sf::VideoMode(1200, 800), L"地图可视化");
        window->setFramerateLimit(60);
        
        // 初始化视口
        viewport.left = 0;
        viewport.right = window->getSize().x;
        viewport.top = window->getSize().y;
        viewport.bottom = 0;
        viewport.level = 0;
        viewport.maxLevel = levelNum;
        
        oldWindowWidth = window->getSize().x;
        oldWindowHeight = window->getSize().y;
    }

    Graph queryShortestPath(const Node *start, const Node *end) {
        return shortestPathAlgorithm->queryShortestPath(start, end);
    }
    Graph queryShortestTimePath(const Node *start, const Node *end) {
        return shortestPathAlgorithm->queryShortestTimePath(start, end);
    }
    
    // 获取viewport的引用
    const Viewport& getViewport() const {
        return viewport;
    }
    
    // 移动viewport
    void moveViewport(double offsetX, double offsetY) {
        viewport.left += offsetX;
        viewport.right += offsetX;
        viewport.top += offsetY;
        viewport.bottom += offsetY;
    }
    
    // 强制设置level
    void setLevel(int level) {
        if (level >= 0 && level < viewport.maxLevel) {
            viewport.level = level;
        }
    }
    
    // 获取levelNum
    int getLevelNum() const {
        return viewport.maxLevel;
    }
    
    ~Shower() 
    {
        // 释放资源
        delete shortestPathAlgorithm;
        delete dataManager;
        delete dataMaker;
        delete window;
    }
    
    /**
     * @brief 执行一次主循环
     * @return 如果点击了节点返回节点指针；如果窗口关闭返回 nullptr；否则返回 nullptr
     */
    const Node* Tick(const Graph& path = Graph(), const Node* startNode = nullptr, const Node* endNode = nullptr) 
    {
        if (!window->isOpen()) {
            return nullptr;
        }
        
        // 重置点击结果
        const Node* result = nullptr;
        
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
                return nullptr;
            }
            // 滚轮缩放事件
            if (event.type == sf::Event::MouseWheelScrolled) {
                handleZoom(event);
            }
            // 窗口大小改变事件
            if (event.type == sf::Event::Resized) {
                handleResize(event);
            }
            // 鼠标按下事件
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isDragging = true;
                    lastMousePos = sf::Mouse::getPosition(*window);
                    // 记录按下时的节点
                    pressedNode = getNodeAtPosition(lastMousePos, startNode, endNode);
                }
            }
            // 鼠标移动事件（拖拽平移）
            if (event.type == sf::Event::MouseMoved && isDragging) {
                handleDrag(event);
            }
            // 鼠标释放事件
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isDragging = false;
                    // 检测释放时是否在同一个节点上
                    sf::Vector2i releasePos = sf::Mouse::getPosition(*window);
                    const Node* releasedNode = getNodeAtPosition(releasePos, startNode, endNode);
                    // 如果按下和释放都在同一个节点上，调用回调函数
                    if (pressedNode != nullptr && releasedNode != nullptr && pressedNode == releasedNode) {
                        if (nodeClickCallback) {
                            nodeClickCallback(pressedNode);
                        }
                    }
                    pressedNode = nullptr;
                }
            }
        }
        
        // 绘制地图
        drawMap(path, startNode, endNode);
        
        return result;
    }
    
    bool IsOpen() const { return window->isOpen(); }
    
    /**
     * @brief 设置节点点击回调函数
     * @param callback 回调函数，参数为被点击的节点指针
     */
    void SetNodeClickCallback(std::function<void(const Node*)> callback) {
        nodeClickCallback = std::move(callback);
    }
    
private:
    
    // 数据成员
    DataMaker* dataMaker = nullptr;
    DataManager* dataManager = nullptr;
    ShortestPathAlgorithm* shortestPathAlgorithm = nullptr;
    sf::RenderWindow* window = nullptr;
    
    Viewport viewport;
    int mapWidth;
    int mapHeight;
    double oldWindowWidth;
    double oldWindowHeight;
    
    Graph currentGraph;  // 当前画面中的图
    
    // 拖拽相关
    bool isDragging;
    sf::Vector2i lastMousePos;
    const Node* pressedNode = nullptr;
    std::function<void(const Node*)> nodeClickCallback;  // 节点点击回调函数
    
    // 处理滚轮缩放
    void handleZoom(const sf::Event& event) 
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
        
        double ratioX = static_cast<double>(mousePos.x) / window->getSize().x;
        double ratioY = static_cast<double>(mousePos.y) / window->getSize().y;
        double graphX = viewport.left + ratioX * viewport.getWidth();
        double graphY = viewport.top - ratioY * viewport.getHeight();
        
        const double zoomDelta = 0.1;
        double zoomFactor = (event.mouseWheelScroll.delta > 0) ? (1.0 - zoomDelta) : (1.0 + zoomDelta);
        
        double newWidth = viewport.getWidth() * zoomFactor;
        double newHeight = viewport.getHeight() * zoomFactor;
        
        auto oldWidth = viewport.getWidth();
        auto oldHeight = viewport.getHeight();
        if (newWidth > 50 && newHeight > 50 && newWidth < mapWidth * 3.0 && newHeight < mapHeight * 3.0 ||
            !(oldWidth > 50 && oldHeight > 50 && oldWidth < mapWidth * 3.0 && oldHeight < mapHeight * 3.0)) {
            viewport.left = graphX - ratioX * newWidth;
            viewport.right = viewport.left + newWidth;
            viewport.top = graphY + ratioY * newHeight;
            viewport.bottom = viewport.top - newHeight;
        }

        auto t = viewport.getWidth() / window->getSize().x;
        viewport.updateLevel(t);
    }
    
    // 处理窗口大小改变
    void handleResize(const sf::Event& event) 
    {
        sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
        window->setView(sf::View(visibleArea));

        double newWidth = viewport.getWidth() * (window->getSize().x / oldWindowWidth);
        double newHeight = viewport.getHeight() * (window->getSize().y / oldWindowHeight);
        auto nleft = viewport.left - (newWidth - viewport.getWidth()) / 2;
        auto nright = viewport.right + (newWidth - viewport.getWidth()) / 2;
        auto ntop = viewport.top + (newHeight - viewport.getHeight()) / 2;
        auto nbottom = viewport.bottom - (newHeight - viewport.getHeight()) / 2;
        viewport.left = nleft;
        viewport.right = nright;
        viewport.top = ntop;
        viewport.bottom = nbottom;
        oldWindowWidth = window->getSize().x;
        oldWindowHeight = window->getSize().y;
    }
    
    // 处理拖拽平移
    void handleDrag(const sf::Event& event) 
    {
        sf::Vector2i currentMousePos = sf::Mouse::getPosition(*window);
        
        int deltaX = currentMousePos.x - lastMousePos.x;
        int deltaY = currentMousePos.y - lastMousePos.y;
        
        double graphDeltaX = -deltaX * (viewport.getWidth() / window->getSize().x);
        double graphDeltaY = deltaY * (viewport.getHeight() / window->getSize().y);
        
        viewport.left += graphDeltaX;
        viewport.right += graphDeltaX;
        viewport.top += graphDeltaY;
        viewport.bottom += graphDeltaY;
        
        lastMousePos = currentMousePos;
    }
    
    // 获取鼠标位置对应的节点
    const Node* getNodeAtPosition(const sf::Vector2i& mousePos, const Node *startNode, const Node *endNode)
    {
        double scaleX = window->getSize().x / viewport.getWidth();
        double scaleY = window->getSize().y / viewport.getHeight();
        double graphX = viewport.left + static_cast<double>(mousePos.x) / scaleX;
        double graphY = viewport.top - static_cast<double>(mousePos.y) / scaleY;
        
        // 检查是否有节点在鼠标位置附近
        double detectRadius = std::max(12.0, 12.0 / scaleX);
        ___FF(for (const Node* node : currentGraph.first),
            if (startNode && viewport.level != 0) for (const Node* node : {startNode}),
            if (endNode && viewport.level != 0) for (const Node* node : {endNode}),
            {
            double dx = node->x - graphX;
            double dy = node->y - graphY;
            double distance = std::sqrt(dx * dx + dy * dy);
            if (distance < detectRadius) {  // 节点检测半径，随level缩放
                return node;
            }
        })
        return nullptr;
    }
    
    // 绘制地图
    void drawMap(const Graph& path, const Node* startNode, const Node* endNode) 
    {
        window->clear(sf::Color::White);
        
        // 查询并存储当前画面中的图
        currentGraph = dataManager->queryDataInViewport(
            static_cast<int>(viewport.left),
            static_cast<int>(viewport.right),
            static_cast<int>(viewport.top),
            static_cast<int>(viewport.bottom),
            viewport.level
        );

        std::clog << "level: " << viewport.level << std::endl;
        
        double scaleX = window->getSize().x / viewport.getWidth();
        double scaleY = window->getSize().y / viewport.getHeight();
        
        float lineThickness = std::max(1.5f, static_cast<float>(scaleX * 2));
        float nodeRadius = std::max(10.0f, static_cast<float>(scaleX * 10));
        float outlineThickness = std::max(2.0f, static_cast<float>(scaleX * 3));
        float edgeOffset = std::max(5.0, 5.0f * scaleX);  // 边的偏移量，避免双向边重合

        // 绘制边
        __FF(for (const Edge* edge : currentGraph.second), 
             if (viewport.level != 0) for (const Edge* edge : path.second), 
             {
            sf::Vector2f start(
                (edge->from->x - viewport.left) * scaleX,
                (viewport.top - edge->from->y) * scaleY
            );
            sf::Vector2f end(
                (edge->to->x - viewport.left) * scaleX,
                (viewport.top - edge->to->y) * scaleY
            );

            sf::Vector2f direction = end - start;
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            
            sf::Vector2f normalizedDir = direction / length;
            // 计算右侧法线方向 (-dy, dx)
            sf::Vector2f rightNormal(-normalizedDir.y, normalizedDir.x);
            
            sf::Vector2f adjustedStart = start + normalizedDir * nodeRadius + rightNormal * edgeOffset;
            sf::Vector2f adjustedEnd = end - normalizedDir * nodeRadius + rightNormal * edgeOffset;
            sf::Vector2f adjustedDirection = adjustedEnd - adjustedStart;
            float adjustedLength = std::max(0.0f, std::sqrt(
                adjustedDirection.x * adjustedDirection.x + 
                adjustedDirection.y * adjustedDirection.y
            ));

            // 检查是否在路径上，路径边线更粗
            float currentThickness = lineThickness;
            if (Tag::instance()[edge]["onpath"] == "1") {
                currentThickness = lineThickness * 3.5f;  // 路径边线粗3.5倍
            }
            
            sf::RectangleShape lineShape(sf::Vector2f(adjustedLength, currentThickness));
            // 检查是否在路径上
            if (0 && Tag::instance()[edge]["onpath"] == "1") {
                lineShape.setFillColor(sf::Color(255, 0, 0));  // 红色表示路径
            } else {
                // 根据流量计算颜色：流量少->绿色，流量多->红色，中间黄橙过渡
                int currentFlow = dataMaker->queryCurrentFlowInEdge(edge);
                double ratio = static_cast<double>(currentFlow) / edge->volume;
                
                // 平滑颜色插值：绿(0,255,0) -> 黄(255,255,0) -> 橙(255,165,0) -> 红(255,0,0)
                sf::Color color;
                if (ratio <= 0.33) {
                    // 绿色到黄色的过渡 (0-0.33)
                    double t = ratio / 0.33;
                    color.r = static_cast<sf::Uint8>(0 + PerlinNoise::lerp(t) * 255);
                    color.g = 255;
                    color.b = 0;
                } else if (ratio <= 0.67) {
                    // 黄色到橙色的过渡 (0.33-0.67)
                    double t = (ratio - 0.33) / 0.34;
                    color.r = 255;
                    color.g = static_cast<sf::Uint8>(255 - PerlinNoise::lerp(t) * 90);
                    color.b = 0;
                } else {
                    // 橙色到红色的过渡 (0.67-1.0)
                    double t = (ratio - 0.67) / 0.33;
                    color.r = 255;
                    color.g = static_cast<sf::Uint8>(165 - PerlinNoise::lerp(t) * 165);
                    color.b = 0;
                }
                lineShape.setFillColor(color);
            }
            lineShape.setPosition(adjustedStart);
            lineShape.setRotation(std::atan2(adjustedDirection.y, adjustedDirection.x) * 180 / PI);
            lineShape.setOrigin(0, currentThickness / 2);

            window->draw(lineShape);
            
            // 绘制箭头（在终点一侧，指向终点）
            sf::Color arrowColor = lineShape.getFillColor();
            float arrowSize = lineThickness * 4;  // 箭头大小
            float arrowWidth = lineThickness * 2;  // 箭头宽度
            
            // 箭头尖端在终点位置，底边在终点后方
            sf::Vector2f arrowTip = adjustedEnd;  // 箭头尖端指向终点
            sf::Vector2f arrowBack = adjustedEnd - normalizedDir * arrowSize;  // 箭头底边
            
            // 绘制箭头（三角形）
            sf::ConvexShape arrow(3);
            arrow.setPoint(0, arrowTip);
            arrow.setPoint(1, arrowBack + rightNormal * arrowWidth);
            arrow.setPoint(2, arrowBack - rightNormal * arrowWidth);
            arrow.setFillColor(arrowColor);
            window->draw(arrow);
        })
        
        // 绘制节点
        ____FF(for (const Node* node : currentGraph.first), 
             if (viewport.level != 0) for (const Node* node : path.first), 
             if (startNode && viewport.level != 0) for (const Node* node : {startNode}),
             if (endNode && viewport.level != 0) for (const Node* node : {endNode}),
            {
            sf::Vector2f position(
                (node->x - viewport.left) * scaleX,
                (viewport.top - node->y) * scaleY
            );

            sf::CircleShape circle(nodeRadius);
            
            // 检查是否是起点
            if (Tag::instance()[node]["s"] == "1") {
                circle.setFillColor(sf::Color(0, 100, 0));  // 深绿色实心
                circle.setOutlineColor(sf::Color(0, 60, 0));  // 深绿色边框
                circle.setOutlineThickness(2);
            }
            // 检查是否是终点
            else if (Tag::instance()[node]["e"] == "1") {
                circle.setFillColor(sf::Color(139, 0, 0));  // 深红色实心
                circle.setOutlineColor(sf::Color(90, 0, 0));  // 深红色边框
                circle.setOutlineThickness(2);
            }
            // 检查是否在路径上
            else if (Tag::instance()[node]["onpath"] == "1") {
                circle.setFillColor(sf::Color(255, 215, 0));  // 金色表示路径上的节点
                circle.setOutlineColor(sf::Color(218, 165, 32));
                circle.setOutlineThickness(2);
            }
            // 普通节点
            else {
                circle.setFillColor(sf::Color::Transparent);
                circle.setOutlineColor(sf::Color(30, 80, 180));
                circle.setOutlineThickness(outlineThickness);
            }
            
            circle.setPosition(position - sf::Vector2f(nodeRadius, nodeRadius));
            window->draw(circle);
        })
        
        window->display();
    }
};

#endif