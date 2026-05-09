#ifndef __SHOWER_HPP__
#define __SHOWER_HPP__

// Shower类 - 封装地图显示功能
class Shower {
public:
    Shower(int mapWidth, int mapHeight, int nodeCount, int edgeCount) : mapWidth(mapWidth), mapHeight(mapHeight), isDragging(false) 
    {
        // 创建数据对象
        dataMaker = new DataMaker(0, mapWidth, 0, mapHeight, nodeCount, edgeCount);
        dataManager = new DataManager(dataMaker->getGraph());
        shortestPathAlgorithm = new ShortestPathAlgorithm(dataMaker->getGraph());
        shortestPathAlgorithm->setFlowQueryInterface([this](const Edge *edge) {
            return dataMaker->queryCurrentFlowInEdge(edge);
        });
        
        // 创建SFML窗口
        window = new sf::RenderWindow(sf::VideoMode(1200, 800), L"地图可视化");
        window->setFramerateLimit(60);
        
        // 初始化视口（显示整个图区域）
        viewport.left = 0;
        viewport.right = mapWidth;
        viewport.top = mapHeight;
        viewport.bottom = 0;
        viewport.level = 0;
        
        oldWindowWidth = window->getSize().x;
        oldWindowHeight = window->getSize().y;
    }
    
    ~Shower() 
    {
        // 释放资源
        delete shortestPathAlgorithm;
        delete dataManager;
        delete dataMaker;
        delete window;
    }
    
    // 执行一次主循环
    bool Tick() 
    {
        if (!window->isOpen()) {
            return false;
        }
        
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
                return false;
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
                }
            }
        }
        
        // 绘制地图
        drawMap();
        
        return true;
    }
    
    bool IsOpen() const { return window->isOpen(); }
    
private:
    // 视口结构体
    struct Viewport {
        double left = 0;      // 左边界
        double right = 10000; // 右边界
        double top = 10000;   // 上边界
        double bottom = 0;    // 下边界
        int level = 0;        // 当前层级
        
        double getWidth() const { return right - left; }
        double getHeight() const { return top - bottom; }
    };
    
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
    
    // 拖拽相关
    bool isDragging;
    sf::Vector2i lastMousePos;
    
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
        
        if (newWidth > 50 && newHeight > 50 && newWidth < mapWidth * 3.0 && newHeight < mapHeight * 3.0) {
            viewport.left = graphX - ratioX * newWidth;
            viewport.right = viewport.left + newWidth;
            viewport.top = graphY + ratioY * newHeight;
            viewport.bottom = viewport.top - newHeight;
        }
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
    
    // 绘制地图
    void drawMap() 
    {
        window->clear(sf::Color::White);
        
        Graph visibleGraph = dataManager->queryDataInViewport(
            static_cast<int>(viewport.left),
            static_cast<int>(viewport.right),
            static_cast<int>(viewport.top),
            static_cast<int>(viewport.bottom),
            viewport.level
        );
        
        double scaleX = window->getSize().x / viewport.getWidth();
        double scaleY = window->getSize().y / viewport.getHeight();
        
        float lineThickness = std::max(1.5f, static_cast<float>(scaleX * 2));
        float nodeRadius = std::max(6.0f, static_cast<float>(scaleX * 10));
        float outlineThickness = std::max(2.0f, static_cast<float>(scaleX * 3));

        // 绘制边
        for (const Edge* edge : visibleGraph.second) {
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
            sf::Vector2f adjustedStart = start + normalizedDir * nodeRadius;
            sf::Vector2f adjustedEnd = end - normalizedDir * nodeRadius;
            sf::Vector2f adjustedDirection = adjustedEnd - adjustedStart;
            float adjustedLength = std::max(0.0f, std::sqrt(
                adjustedDirection.x * adjustedDirection.x + 
                adjustedDirection.y * adjustedDirection.y
            ));

            sf::RectangleShape lineShape(sf::Vector2f(adjustedLength, lineThickness));
            lineShape.setFillColor(sf::Color(100, 100, 100));
            lineShape.setPosition(adjustedStart);
            lineShape.setRotation(std::atan2(adjustedDirection.y, adjustedDirection.x) * 180 / PI);
            lineShape.setOrigin(0, lineThickness / 2);

            window->draw(lineShape);
        }

        // 绘制节点
        for (const Node* node : visibleGraph.first) {
            sf::Vector2f position(
                (node->x - viewport.left) * scaleX,
                (viewport.top - node->y) * scaleY
            );

            sf::CircleShape circle(nodeRadius);
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineColor(sf::Color(30, 80, 180));
            circle.setOutlineThickness(outlineThickness);
            circle.setPosition(position - sf::Vector2f(nodeRadius, nodeRadius));

            window->draw(circle);
        }
        
        window->display();
    }
};

#endif
