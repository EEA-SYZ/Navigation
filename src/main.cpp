/**
 * @brief 主函数
 * @note 使用ui.hpp模块创建GUI界面
 */
#include "ui.hpp"
#include <iostream>

int main() {
    // 创建窗口
    ui::Screen screen(500, 400, "地图参数设置");

    // 创建主布局容器
    ui::VerticalBox* mainLayout = new ui::VerticalBox();
    mainLayout->SetPreset(ui::Control::Preset::PLACE_AT_CENTER);
    mainLayout->SetVSizeValueType(ui::Control::ValueType::PERCENTAGE);
    mainLayout->SetVSize(100);
    mainLayout->SetHSizeValueType(ui::Control::ValueType::PERCENTAGE);
    mainLayout->SetHSize(80);
    mainLayout->SetGap(15);
    mainLayout->AddTo(&screen);

    // 创建标题
    ui::Label* title = new ui::Label("地图参数设置");
    title->SetFontSize(40);
    title->SetFontColor(sf::Color::Black);
    title->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
    title->AddTo(mainLayout);

    // 创建输入框标签和输入框的水平布局
    // 地图长
    ui::HorizontalBox* widthBox = new ui::HorizontalBox();
    widthBox->SetHSizeWrap(true);
    widthBox->SetVSizeWrap(true);
    widthBox->SetGap(10);
    widthBox->AddTo(mainLayout);

    ui::Label* widthLabel = new ui::Label("地图长:");
    widthLabel->SetFontSize(25);
    widthLabel->SetFontColor(sf::Color::Black);
    widthLabel->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
    widthLabel->AddTo(widthBox);

    ui::InputBox* widthInput = new ui::InputBox();
    widthInput->SetHSize(200);
    widthInput->SetVSize(40);
    widthInput->SetFontSize(25);
    widthInput->SetFontColor(sf::Color::Black);
    widthInput->SetInputtingFontColor(sf::Color::Black);
    widthInput->SetBackColor(sf::Color(200, 200, 200));
    widthInput->SetInputtingBackColor(sf::Color::White);
    widthInput->SetContentLimit(ui::InputBox::ContentLimit::BAN_SPECIAL_CHARACTERS);
    widthInput->SetSpecialCharacters(ui::InputBox::ASCII);
    widthInput->AddTo(widthBox);

    // 地图高
    ui::HorizontalBox* heightBox = new ui::HorizontalBox();
    heightBox->SetHSizeWrap(true);
    heightBox->SetVSizeWrap(true);
    heightBox->SetGap(10);
    heightBox->AddTo(mainLayout);

    ui::Label* heightLabel = new ui::Label("地图高:");
    heightLabel->SetFontSize(25);
    heightLabel->SetFontColor(sf::Color::Black);
    heightLabel->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
    heightLabel->AddTo(heightBox);

    ui::InputBox* heightInput = new ui::InputBox();
    heightInput->SetHSize(200);
    heightInput->SetVSize(40);
    heightInput->SetFontSize(25);
    heightInput->SetFontColor(sf::Color::Black);
    heightInput->SetInputtingFontColor(sf::Color::Black);
    heightInput->SetBackColor(sf::Color(200, 200, 200));
    heightInput->SetInputtingBackColor(sf::Color::White);
    heightInput->SetContentLimit(ui::InputBox::ContentLimit::BAN_SPECIAL_CHARACTERS);
    heightInput->SetSpecialCharacters(ui::InputBox::ASCII);
    heightInput->AddTo(heightBox);

    // 节点数
    ui::HorizontalBox* nodeBox = new ui::HorizontalBox();
    nodeBox->SetHSizeWrap(true);
    nodeBox->SetVSizeWrap(true);
    nodeBox->SetGap(10);
    nodeBox->AddTo(mainLayout);

    ui::Label* nodeLabel = new ui::Label("节点数:");
    nodeLabel->SetFontSize(25);
    nodeLabel->SetFontColor(sf::Color::Black);
    nodeLabel->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
    nodeLabel->AddTo(nodeBox);

    ui::InputBox* nodeInput = new ui::InputBox();
    nodeInput->SetHSize(200);
    nodeInput->SetVSize(40);
    nodeInput->SetFontSize(25);
    nodeInput->SetFontColor(sf::Color::Black);
    nodeInput->SetInputtingFontColor(sf::Color::Black);
    nodeInput->SetBackColor(sf::Color(200, 200, 200));
    nodeInput->SetInputtingBackColor(sf::Color::White);
    nodeInput->SetContentLimit(ui::InputBox::ContentLimit::BAN_SPECIAL_CHARACTERS);
    nodeInput->SetSpecialCharacters(ui::InputBox::ASCII);
    nodeInput->AddTo(nodeBox);

    // 边数
    ui::HorizontalBox* edgeBox = new ui::HorizontalBox();
    edgeBox->SetHSizeWrap(true);
    edgeBox->SetVSizeWrap(true);
    edgeBox->SetGap(10);
    edgeBox->AddTo(mainLayout);

    ui::Label* edgeLabel = new ui::Label("边数:");
    edgeLabel->SetFontSize(25);
    edgeLabel->SetFontColor(sf::Color::Black);
    edgeLabel->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
    edgeLabel->AddTo(edgeBox);

    ui::InputBox* edgeInput = new ui::InputBox();
    edgeInput->SetHSize(200);
    edgeInput->SetVSize(40);
    edgeInput->SetFontSize(25);
    edgeInput->SetFontColor(sf::Color::Black);
    edgeInput->SetInputtingFontColor(sf::Color::Black);
    edgeInput->SetBackColor(sf::Color(200, 200, 200));
    edgeInput->SetInputtingBackColor(sf::Color::White);
    edgeInput->SetContentLimit(ui::InputBox::ContentLimit::BAN_SPECIAL_CHARACTERS);
    edgeInput->SetSpecialCharacters(ui::InputBox::ASCII);
    edgeInput->AddTo(edgeBox);

    // 创建确认按钮
    ui::Button* confirmBtn = new ui::Button("确认", [&](const std::string&, const sf::Event&) {
        std::cout << "地图长: " << widthInput->GetText() << std::endl;
        std::cout << "地图高: " << heightInput->GetText() << std::endl;
        std::cout << "节点数: " << nodeInput->GetText() << std::endl;
        std::cout << "边数: " << edgeInput->GetText() << std::endl;
    });
    confirmBtn->SetHSize(150);
    confirmBtn->SetVSize(50);
    confirmBtn->SetFontSize(25);
    confirmBtn->SetFontColor(sf::Color::White);
    confirmBtn->SetFlatOutlineColor(sf::Color(100, 150, 255));
    confirmBtn->SetFocusOutlineColor(sf::Color(150, 200, 255));
    confirmBtn->SetFlatBackColor(sf::Color(100, 150, 255));
    confirmBtn->SetFocusBackColor(sf::Color(150, 200, 255));
    confirmBtn->SetPreset(ui::Control::Preset::WRAP_AT_CENTER);
    confirmBtn->AddTo(mainLayout);

    // 主循环
    while (screen.IsOpen()) {
        screen.Tick();
        screen.Draw();
    }

    return 0;
}