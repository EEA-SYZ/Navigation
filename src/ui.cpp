#include "ui.hpp"

const ui::Button::Callback ui::Control::DO_NOTHING = [](const std::string &name, const sf::Event &event){};

const float ui::ScrollingBox::DEFAULT_SENSITIVITY = 15;

const unsigned int ui::Label::REVISION_X = 15;

const unsigned int ui::Label::REVISION_Y = 20;

const std::string ui::Label::FONT_FILE_PATH = _ASSETS_"simfang.ttf";

const float ui::LoadingRing::PI = 3.14159265358979323846;

const sf::String ui::InputBox::NUMBER = "0123456789";
const sf::String ui::InputBox::LOWER_LETTER = "abcdefghijklmnopqrstuvwxyz";
const sf::String ui::InputBox::UPPER_LETTER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const sf::String ui::InputBox::ASCII = (sf::String)
        "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F" + 
    "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F" + 
    "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F" + 
    "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F" + 
    "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4A\x4B\x4C\x4D\x4E\x4F" + 
    "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5A\x5B\x5C\x5D\x5E\x5F" + 
    "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6A\x6B\x6C\x6D\x6E\x6F" + 
    "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7A\x7B\x7C\x7D\x7E\x7F" + 
    "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F" + 
    "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F" + 
    "\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF" + 
    "\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7\xB8\xB9\xBA\xBB\xBC\xBD\xBE\xBF" + 
    "\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF" + 
    "\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF" + 
    "\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF" + 
    "\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF";

void ui::Control::SetCenter(Direction direction, int percentage) noexcept
{
    center[direction] = percentage;
    if (parent) parent->UpdateInQueue();
    else if (nominalParent) nominalParent->UpdateInQueue();
}

void ui::Control::Fresh() noexcept
{
    if (inQueue) {
        Update(resetMinSize); 
        inQueue = false; 
        resetMinSize = false; 
    }
}

void ui::Container::FreshUp() noexcept
{
    for (auto child : *children) {
        child->FreshUp();
    }
    Control::FreshUp();
}

void ui::Container::FreshDown() noexcept
{
    Control::FreshDown();
    for (auto child : *children) {
        child->FreshDown();
    }
}

void ui::Control::SetAnchor(Direction direction, int percentage) noexcept
{
    anchor[direction] = percentage;
    if (parent) parent->UpdateInQueue();
    else if (nominalParent) nominalParent->UpdateInQueue();
}

void ui::Control::SetPosition(Direction direction, int absolute) noexcept
{
    position[direction] = absolute;
    if (parent) parent->UpdateInQueue();
    else if (nominalParent) nominalParent->UpdateInQueue();
}

void ui::Control::SetVisible(bool flag) noexcept
{
    visible = flag;
    if (parent) parent->UpdateInQueue();
    else if (nominalParent) nominalParent->UpdateInQueue();
}

void ui::Control::AddTo(Container *container) noexcept
{
    container->Add(this);
}

void ui::Control::SetPreset(Direction direction, Preset preset) noexcept
{
    switch (preset) {
        case Preset::WRAP_AT_FRONT:
        case Preset::WRAP_AT_END:
        case Preset::WRAP_AT_CENTER:
            SetSizeValueType(direction, ValueType::ABSOLUTE);
            SetSizeWrap(direction, true);
            break;
        case Preset::FILL_FROM_FRONT:
        case Preset::FILL_FROM_END:
        case Preset::FILL_FROM_CENTER:
            SetSizeValueType(direction, ValueType::PERCENTAGE);
            SetSize(direction, 100);
            break;
        case Preset::PLACE_AT_FRONT:
        case Preset::PLACE_AT_END:
        case Preset::PLACE_AT_CENTER:
            break;
        default:
            assert(false); // Impossible preset.
            break;
    }

    switch (preset) {
        case Preset::PLACE_AT_FRONT:
        case Preset::WRAP_AT_FRONT:
        case Preset::FILL_FROM_FRONT:
            SetCenter(direction, 0);
            SetAnchor(direction, 0);
            break;
        case Preset::PLACE_AT_END:
        case Preset::WRAP_AT_END:
        case Preset::FILL_FROM_END:
            SetCenter(direction, 100);
            SetAnchor(direction, 100);
            break;
        case Preset::PLACE_AT_CENTER:
        case Preset::WRAP_AT_CENTER:
        case Preset::FILL_FROM_CENTER:
            SetCenter(direction, 50);
            SetAnchor(direction, 50);
            break;
        default:
            assert(false); // Impossible preset.
            break;
    }
    SetPosition(direction, 0);
}

void ui::Control::SetMinSize(Direction direction, unsigned int absolute) noexcept
{
    if (absolute == minSize[direction]) return;
    minSize[direction] = absolute;
    if (parent) parent->UpdateInQueue();
    else if (nominalParent) nominalParent->UpdateInQueue();
}

void ui::Control::SetSizeWrap(Direction direction, bool flag) noexcept
{
    sizeWrap[direction] = flag;
    if (parent) parent->UpdateInQueue();
    else if (nominalParent) nominalParent->UpdateInQueue();
}

void ui::Control::SetSizeValueType(Direction direction, ValueType valueType) noexcept
{
    sizeValueType[direction] = valueType;
    if (parent) parent->UpdateInQueue();
    else if (nominalParent) nominalParent->UpdateInQueue();
}

void ui::Control::SetSize(Direction direction, unsigned int value) noexcept
{
    size[direction] = value;
    if (parent) parent->UpdateInQueue();
    else if (nominalParent) nominalParent->UpdateInQueue();
}

void ui::Control::SetParent(Container *container) noexcept
{
    parent = container;
}

void ui::Control::SetNominalParent(Control *container) noexcept
{
    nominalParent = container;
}

void ui::Control::SetGlobalPosition(Direction direction, int absolute) noexcept
{
    globalPosition[direction] = absolute;
    UpdateInQueue(false);
}

void ui::Control::SetGlobalSize(Direction direction, unsigned int absolute) noexcept
{
    globalSize[direction] = absolute;
    UpdateInQueue(false);
}

void ui::Container::Add(Control *control) noexcept
{
    children->push_back(control);
    control->SetParent(this);
    UpdateInQueue();
}

void ui::Container::Remove(Control *control) noexcept
{
    auto toDelete = children->end();
    for (auto it = children->begin(); it < children->end(); ++it) {
        if (*it == control) {
            toDelete = it;
            break;
        }
    }
    if (toDelete != children->end()) children->erase(toDelete);
    control->SetParent(nullptr);
    UpdateInQueue();
}

void ui::Container::FreeAll() noexcept
{
    auto tmp = *children;
    for (auto each : tmp) {
        auto toDelete = children->end();
        for (auto it = children->begin(); it < children->end(); ++it) {
            if (*it == each) {
                toDelete = it;
                break;
            }
        }
        if (toDelete != children->end()) children->erase(toDelete);
        each->SetParent(nullptr);
        delete each;
    }
    UpdateInQueue();
}

void ui::Container::HideAll() noexcept
{
    for (auto child : *children) {
        child->Hide();
    }
    UpdateInQueue();
}

void ui::Container::ShowAll() noexcept
{
    for (auto child : *children) {
        child->Show();
    }
    UpdateInQueue();
}

void ui::Container::ToggleVisible() noexcept
{
    for (auto child : *children) {
        child->SetVisible(!child->GetVisible());
    }
    UpdateInQueue();
}

void ui::Container::FreeAllVisible() noexcept
{
    auto tmp = *children;
    for (auto each : tmp) {
        if (each->GetVisible()) {
            auto toDelete = children->end();
            for (auto it = children->begin(); it < children->end(); ++it) {
                if (*it == each) {
                    toDelete = it;
                    break;
                }
            }
            if (toDelete != children->end()) children->erase(toDelete);
            each->SetParent(nullptr);
            delete each;
        }
    }
    UpdateInQueue();
}

void ui::Container::SyncChildren(Children *pointer) noexcept
{
    FreeAll();
    delete children;
    children = pointer;
    UpdateInQueue();
}

void ui::Container::UnsyncChildren() noexcept
{
    children = new Children{};
}

void ui::Container::ForEachChildren(std::function<void(Control *)> func) noexcept
{
    for (auto child : *children) {
        func(child);
    }
}

void ui::Container::SetIgnoreOutside(bool flag) noexcept
{
    ignoreOutside = flag;
}

void ui::Container::FreeAllHiden() noexcept
{
    auto tmp = *children;
    for (auto each : tmp) {
        if (!each->GetVisible()) {
            auto toDelete = children->end();
            for (auto it = children->begin(); it < children->end(); ++it) {
                if (*it == each) {
                    toDelete = it;
                    break;
                }
            }
            if (toDelete != children->end()) children->erase(toDelete);
            each->SetParent(nullptr);
            delete each;
        }
    }
}

void ui::Container::Process(const sf::Event &event, const sf::RenderWindow &screen) noexcept
{
    for (auto child : *children) {
        if (!child->GetVisible()) continue;

        if (ignoreOutside) {
            if (!IsThisInside(child)) continue;
        }

        child->Process(event, screen);
    }
}

void ui::Container::Draw(sf::RenderWindow &screen, float delta) noexcept
{
    for (auto child : *children) {
        if (!child->GetVisible()) continue;

        if (ignoreOutside) {
            if (!IsThisInside(child)) continue;
        }

        child->Draw(screen, delta);
    }

    _DRAW_DEBUG_RECT_;
}

ui::Container::~Container() noexcept
{
    for (auto each : *children) {
        each->SetParent(nullptr);
        delete each;
    }
    delete children;
}

void ui::Screen::SetRange(unsigned int width, unsigned int height) noexcept
{
    screen.setSize({width, height});
}

sf::String ui::Control::convert(const std::string &str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

void ui::Screen::SetCaption(const std::string &caption) noexcept
{
    screen.setTitle(convert(caption));
}

bool ui::Screen::IsOpen() const noexcept
{
    return screen.isOpen();
}

void ui::Screen::Tick() noexcept
{
    if (IsOpen()) {
        sf::Event event;
        if (screen.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                screen.close();
            } else if (event.type == sf::Event::Resized) {
                SetGlobalSize(Direction::HORIZONTAL, event.size.width);
                SetGlobalSize(Direction::VERTICAL, event.size.height);
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                screen.setView(sf::View(visibleArea));
            } else {
                Process(event, screen);
            }
        }
        Container::FreshUp();
        Container::FreshDown();
    }
}

void ui::Screen::Draw() noexcept
{
    screen.clear();
    Container::Draw(screen, clock.getElapsedTime().asSeconds());
    clock.restart();
    screen.display();
}

void ui::Flat::Update(bool resetMinSize) noexcept
{
    for (auto direction : {Direction::HORIZONTAL, Direction::VERTICAL}) {
        for (auto child : *children) {
            if (!child->GetVisible()) continue;
            UpdateByDefault(direction, child);
        }
    }
}

void ui::Container::UpdateByDefault(Direction direction, Control *child) noexcept
{
    UpdateSizeByDefault(direction, child);
    UpdatePositionByDefault(direction, child);
}

void ui::Container::UpdateSizeByDefault(Direction direction, Control *child) noexcept
{
    child->SetGlobalSize(direction, ObtainDefaultGlobalSize(direction, child, globalSize[direction]));
}

void ui::Container::UpdatePositionByDefault(Direction direction, Control *child) noexcept
{
    child->SetGlobalPosition(direction, ObtainDefaultGlobalPosition(direction, child, globalSize[direction], globalPosition[direction]));
}

unsigned int ui::Container::ObtainDefaultGlobalSize(Direction direction, const Control *child, unsigned int parentSize) noexcept
{
    return ObtainDefaultGlobalSize(child->GetSizeValueType(direction), child->GetSizeWrap(direction), child->GetSize(direction), child->GetMinSize(direction), parentSize);
}

int ui::Container::ObtainDefaultGlobalPosition(Direction direction, const Control *child, unsigned int parentSize, int parentPosition) noexcept
{
    return ObtainDefaultGlobalPosition(child->GetCenter(direction), child->GetAnchor(direction), child->GetPosition(direction), child->GetGlobalSize(direction), parentSize, parentPosition);
}

unsigned int ui::Container::ObtainDefaultGlobalSize(ValueType sizeValueType, bool wrap, unsigned int size, unsigned int minSize, unsigned int parentSize) noexcept
{
    unsigned int tmpSize;
    switch (sizeValueType) {
        case ValueType::ABSOLUTE:
            if (wrap) tmpSize = minSize;
            else      tmpSize = size;
            break;
        case ValueType::PERCENTAGE:
            tmpSize = size * parentSize / 100;
            break;
        default:
            assert(false); // Impossible value type.
            break;
    }
    return std::max(minSize, tmpSize);
}

int ui::Container::ObtainDefaultGlobalPosition(int center, int anchor, int position, unsigned int thisSize, unsigned int parentSize, int parentPosition) noexcept
{
    int thisPosition = parentPosition;
    thisPosition -= center * thisSize / 100;
    thisPosition += anchor * parentSize / 100;
    thisPosition += position;
    return thisPosition;
}

bool ui::Container::IsThisInside(const Control *child) noexcept
{
    auto bx0 = globalPosition[Direction::HORIZONTAL];
    auto bxx = bx0 + (int)globalSize[Direction::HORIZONTAL];
    auto x0 = child->GetGlobalPosition(Direction::HORIZONTAL);
    auto xx = x0 + (int)child->GetGlobalSize(Direction::HORIZONTAL);
    if (xx < bx0 || x0 > bxx) return false;
    
    auto by0 = globalPosition[Direction::VERTICAL];
    auto byy = by0 + (int)globalSize[Direction::VERTICAL];
    auto y0 = child->GetGlobalPosition(Direction::VERTICAL);
    auto yy = y0 + (int)child->GetGlobalSize(Direction::VERTICAL);
    if (yy < by0 || y0 > byy) return false;
    
    return true;
}

void ui::Center::Update(bool resetMinSize) noexcept
{
    for (auto direction : {Direction::HORIZONTAL, Direction::VERTICAL}) {
        unsigned int myMinSize = 0;
        for (auto child : *children) {
            if (!child->GetVisible()) continue;
            UpdateSizeByDefault(direction, child);
            child->SetGlobalPosition(direction, globalPosition[direction] + (globalSize[direction] - child->GetGlobalSize(direction)) / 2);
            switch (child->GetSizeValueType(direction)) {
                case ValueType::ABSOLUTE:
                    myMinSize = std::max(myMinSize, child->GetGlobalSize(direction));
                    break;
                case ValueType::PERCENTAGE:
                    myMinSize = std::max(myMinSize, child->GetMinSize(direction));
                    break;
                default:
                    assert(false); // Impossible value type.
                    break;
            }
        }
        if (resetMinSize) Set_minSize(direction, myMinSize);
    }
}

void ui::ScrollingBox::FreshUp() noexcept
{
    layer.FreshUp();
    Control::FreshUp();
}

void ui::ScrollingBox::FreshDown() noexcept
{
    Control::FreshDown();
    layer.FreshDown();
}

void ui::Button::FreshUp() noexcept
{
    layer.FreshUp();
    Control::FreshUp();
}

void ui::Button::FreshDown() noexcept
{
    Control::FreshDown();
    layer.FreshDown();
}

void ui::InputBox::FreshUp() noexcept
{
    layer.FreshUp();
    Control::FreshUp();
}

void ui::InputBox::FreshDown() noexcept
{
    Control::FreshDown();
    layer.FreshDown();
}

void ui::ScrollBar::FreshUp() noexcept
{
    layer.FreshUp();
    Control::FreshUp();
}

void ui::ScrollBar::FreshDown() noexcept
{
    Control::FreshDown();
    layer.FreshDown();
}

void ui::LoadingRingWithText::FreshUp() noexcept
{
    layer.FreshUp();
    Control::FreshUp();
}

void ui::LoadingRingWithText::FreshDown() noexcept
{
    Control::FreshDown();
    layer.FreshDown();
}

void ui::LinearBox::SetAllChildrenWrap(Direction direction, bool flag) noexcept
{
    for (auto child : *children) {
        child->SetSizeValueType(direction, ValueType::ABSOLUTE);
        child->SetSizeWrap(direction, flag);
    }
}

void ui::LinearBox::SetGap(int absolute) noexcept
{
    gap = absolute;
    UpdateInQueue();
}

void ui::LinearBox::SetProportionMode(bool flag) noexcept
{
    proportionMode = flag;
    UpdateInQueue();
}

void ui::LinearBox::SetDelta(int absolute) noexcept
{
    delta = absolute;
    UpdateInQueue();
}

void ui::LinearBox::UpdateLinear(Direction direction, bool resetMinSize) noexcept
{
    auto another = GetAnotherDirection(direction);
    unsigned int anotherMinSize = 0;
    for (auto child : *children) {
        if (!child->GetVisible()) continue;
        UpdateByDefault(another, child);
        switch (child->GetSizeValueType(another)) {
            case ValueType::ABSOLUTE:
                anotherMinSize = std::max(anotherMinSize, child->GetGlobalSize(another));
                break;
            case ValueType::PERCENTAGE:
                anotherMinSize = std::max(anotherMinSize, child->GetMinSize(another));
                break;
            default:
                assert(false); // Impossible value type.
                break;
        }
    }
    if (resetMinSize) Set_minSize(another, anotherMinSize);

    if (proportionMode) {
        unsigned int directionMinSize = 0;
        unsigned int sizeOccupied     = 0;
        unsigned int ratioSum         = 0;
        int          childCount       = 0;
        for (auto child : *children) {
            if (!child->GetVisible()) continue;
            switch (child->GetSizeValueType(direction)) {
                case ValueType::ABSOLUTE:
                    UpdateSizeByDefault(direction, child);
                    directionMinSize += child->GetGlobalSize(direction);
                    sizeOccupied     += child->GetGlobalSize(direction);
                    break;
                case ValueType::PERCENTAGE:
                    directionMinSize += child->GetMinSize(direction);
                    ratioSum         += child->GetSize(direction);
                    break;
                default:
                    assert(false); // Impossible value type.
                    break;
            }
            ++childCount;
        }
        if (resetMinSize) Set_minSize(direction, directionMinSize + std::max(0, childCount - 1) * gap);
        unsigned int sizeLeft    = globalSize[direction] - std::max(0, childCount - 1) * gap - sizeOccupied;
        bool fixing = true;
        std::unordered_map<Control *, bool> mark;
        while (fixing) {
            fixing = false;
            unsigned int tmpPosition = 0;
            for (auto child : *children) {
                if (!child->GetVisible()) continue;
                if (child->GetSizeValueType(direction) == ValueType::PERCENTAGE && !mark[child]) {
                    if (child->GetSize(direction) * sizeLeft / std::max(1u, ratioSum) < child->GetMinSize(direction)) {
                        child->SetGlobalSize(direction, child->GetMinSize(direction));
                        ratioSum -= child->GetSize(direction);
                        sizeLeft -= child->GetMinSize(direction);
                        fixing = true;
                        mark[child] = true;
                        break;
                    }
                    child->SetGlobalSize(direction, child->GetSize(direction) * sizeLeft / std::max(1u, ratioSum));
                }
                child->SetGlobalPosition(direction, globalPosition[direction] + tmpPosition);
                tmpPosition += child->GetGlobalSize(direction) + gap;
            }
        }
    } else {
        unsigned int tmpPosition = 0;
        for (auto child : *children) {
            if (!child->GetVisible()) continue;
            UpdateSizeByDefault(direction, child);
            child->SetGlobalPosition(direction, globalPosition[direction] + tmpPosition + delta);
            tmpPosition += child->GetGlobalSize(direction) + gap;
        }
        totalSize = tmpPosition != 0 ? tmpPosition - gap : 0;
    }
}

void ui::HorizontalBox::Update(bool resetMinSize) noexcept
{
    UpdateLinear(Direction::HORIZONTAL, resetMinSize);
}

void ui::VerticalBox::Update(bool resetMinSize) noexcept
{
    UpdateLinear(Direction::VERTICAL, resetMinSize);
}

void ui::Label::SetContent(const std::string &newContent) noexcept
{
    content = newContent;
    UpdateInQueue();
}

void ui::Label::SetFont(const std::string &fontFile) noexcept
{
    if (!font.loadFromFile(convert(fontFile))) {
        error = true;
    }
    UpdateInQueue();
}

void ui::Button::SetMaxCount(unsigned int count) noexcept
{
    label->SetMaxCount(count);
    UpdateInQueue();
}

void ui::InputBox::SetMaxCount(unsigned int count) noexcept
{
    label->SetMaxCount(count);
    UpdateInQueue();
}

void ui::Label::SetMaxCount(unsigned int count) noexcept
{
    maxCount = count;
    UpdateInQueue();
}

void ui::Label::Update(bool resetMinSize) noexcept
{
    sf::String tmpText;
    if (maxCount) {
        unsigned int cur = 0;
        for (auto c : convert(content)) {
            tmpText += c;
            ++cur;
            if (c == '\n') {
                cur = 0;
            }
            if (cur >= maxCount) {
                tmpText += '\n';
                cur = 0;
            }
        }
    } else {
        tmpText = convert(content);
    }
    text.setString(tmpText);
    text.setCharacterSize(fontSize);
    text.setFont(font);
    text.setPosition(sf::Vector2f(globalPosition[Direction::HORIZONTAL], globalPosition[Direction::VERTICAL]));
    if (resetMinSize) {
        Set_minSize(Direction::HORIZONTAL, text.getGlobalBounds().getSize().x + REVISION_X);
        Set_minSize(Direction::VERTICAL, std::max((unsigned int)text.getGlobalBounds().getSize().y, fontSize) + REVISION_Y);
    }
}

void ui::PageTurner::SetSingleMinHeight(unsigned int absolute) noexcept
{
    singleHeight = absolute;
    UpdateInQueue();
}

void ui::PageTurner::SetSingleMinWidth(unsigned int absolute) noexcept
{
    singleWidth = absolute;
    UpdateInQueue();
}

void ui::PageTurner::SetVerticalGap(unsigned int absolute) noexcept
{
    verticalGap = absolute;
    UpdateInQueue();
}

void ui::PageTurner::SetHorizontalGap(unsigned int absolute) noexcept
{
    horizontalGap = absolute;
    UpdateInQueue();
}

void ui::PageTurner::SetSingle(unsigned int absolute) noexcept
{
    SetSingleMinHeight(absolute);
    SetSingleMinWidth(absolute);
}

void ui::PageTurner::SetGap(unsigned int absolute) noexcept
{
    SetVerticalGap(absolute);
    SetHorizontalGap(absolute);
}

void ui::PageTurner::SetFontSize(unsigned int absolute) noexcept
{
    fontSize = absolute;
    UpdateInQueue();
}

void ui::PageTurner::SetMaxPage(unsigned int absolute) noexcept
{
    maxPage = std::max(1u, absolute);
    if (currentPage > maxPage) currentPage = maxPage;
    UpdateInQueue();
}

void ui::PageTurner::SetCurrentPage(unsigned int absolute) noexcept
{
    if (absolute < 1) absolute = 1;
    if (absolute > maxPage) absolute = maxPage;
    if (currentPage == absolute) return;
    currentPage = absolute;
    turnCallback(name, {});
    UpdateInQueue();
}

void ui::PageTurner::Update(bool resetMinSize) noexcept
{
    if (resetMinSize) {
        btnBox->SetVMinSize(singleHeight);
        gotoBox->SetVMinSize(singleHeight);

        for (int i = -DELTA_NUM - 1; i <= DELTA_NUM + 1; ++i) {
            auto c = center[i];
            c->SetHMinSize(singleWidth);
        }
        input->SetHMinSize(singleWidth);

        layer.SetGap(verticalGap);

        btnBox->SetGap(horizontalGap);
        gotoBox->SetGap(horizontalGap);

        for (auto l : labels) {
            l->SetFontSize(fontSize);
        }
        for (auto b : buttons) {
            b->SetFontSize(fontSize);
        }
        input->SetFontSize(fontSize);

        labels[0]->SetContent(ToStr(currentPage));
        for (int i = 0; i < DELTA_NUM * 2; ++i) {
            auto con = currentPage + ToNum(buttons[i]->GetName());
            if (con < 1 || con > maxPage) {
                buttons[i]->SetCaption("-");
                buttons[i]->Disable();
            } else {
                buttons[i]->SetCaption(ToStr(con));
                buttons[i]->Enable();
            }
        }

        maxNum->SetContent("/ " + ToStr(maxPage));

        Set_minSize(Direction::HORIZONTAL, layer.GetMinSize(Direction::HORIZONTAL));
        Set_minSize(Direction::VERTICAL, layer.GetMinSize(Direction::VERTICAL));
    } else {
        layer.SetGlobalPosition(Direction::HORIZONTAL, globalPosition[Direction::HORIZONTAL]);
        layer.SetGlobalPosition(Direction::VERTICAL, globalPosition[Direction::VERTICAL]);
        layer.SetGlobalSize(Direction::HORIZONTAL, globalSize[Direction::HORIZONTAL]);
        layer.SetGlobalSize(Direction::VERTICAL, globalSize[Direction::VERTICAL]);
    }
}

void ui::PageTurner::Process(const sf::Event &event, const sf::RenderWindow &screen) noexcept
{
    layer.Process(event, screen);
}

void ui::PageTurner::Draw(sf::RenderWindow &screen, float delta) noexcept
{
    layer.Draw(screen, delta);

    _DRAW_DEBUG_RECT_;
}

void ui::PageTurner::FreshUp() noexcept
{
    layer.FreshUp();
    Control::FreshUp();
}

void ui::PageTurner::FreshDown() noexcept
{
    Control::FreshDown();
    layer.FreshDown();
}

void ui::Label::SetFontSize(unsigned int absolute) noexcept
{
    fontSize = absolute;
    UpdateInQueue();
}

void ui::Label::SetFontColor(const sf::Color &color) noexcept
{
    fontColor = color;
    text.setFillColor(fontColor);
}

void ui::Label::Draw(sf::RenderWindow &screen, float delta) noexcept
{
    if (!error) {
        screen.draw(text);
    } else {
        sf::RectangleShape rect(sf::Vector2f(globalSize[Direction::HORIZONTAL], globalSize[Direction::VERTICAL]));
        rect.setPosition(sf::Vector2f(globalPosition[Direction::HORIZONTAL], globalPosition[Direction::VERTICAL]));
        rect.setFillColor(sf::Color::Red);
        screen.draw(rect);
    }

    _DRAW_DEBUG_RECT_;
}

void ui::Button::SetCaption(const std::string &caption) noexcept
{
    label->SetContent(caption);
    UpdateInQueue();
}

void ui::Button::SetFontSize(unsigned int size) noexcept
{
    label->SetFontSize(size);
    UpdateInQueue();
}

void ui::Button::SetFontColor(const sf::Color &color) noexcept
{
    label->SetFontColor(color);
}

void ui::Button::SetFlatOutlineColor(const sf::Color &color) noexcept
{
    flatOutlineColor = color;
}

void ui::Button::SetFocusOutlineColor(const sf::Color &color) noexcept
{
    focusOutlineColor = color;
}

void ui::Button::SetFlatBackColor(const sf::Color &color) noexcept
{
    flatBackColor = color;
}

void ui::Button::SetFocusBackColor(const sf::Color &color) noexcept
{
    focusBackColor = color;
}

void ui::Button::SetDisabledOutlineColor(const sf::Color &color) noexcept
{
    disabledOutlineColor = color;
}

void ui::Button::SetDisabledBackColor(const sf::Color &color) noexcept
{
    disabledBackColor = color;
}

void ui::Button::Enable(bool flag) noexcept
{
    enabled = flag;
}

void ui::PageTurner::Enable(bool flag) noexcept
{
    enabled = flag;
}

void ui::PageTurner::Disable() noexcept
{
    Enable(false);
}

void ui::Button::Disable() noexcept
{
    Enable(false);
}

void ui::Button::SetFont(const std::string &fontFile) noexcept
{
    label->SetFont(fontFile);
    UpdateInQueue();
}

void ui::Button::Update(bool resetMinSize) noexcept
{
    layer.SetGlobalSize(Direction::HORIZONTAL, globalSize[Direction::HORIZONTAL]);
    layer.SetGlobalSize(Direction::VERTICAL, globalSize[Direction::VERTICAL]);
    layer.SetGlobalPosition(Direction::HORIZONTAL, globalPosition[Direction::HORIZONTAL]);
    layer.SetGlobalPosition(Direction::VERTICAL, globalPosition[Direction::VERTICAL]);
    if (resetMinSize) {
        Set_minSize(Direction::HORIZONTAL, label->GetMinSize(Direction::HORIZONTAL));
        Set_minSize(Direction::VERTICAL, label->GetMinSize(Direction::VERTICAL));
    }
}

bool ui::Control::IsInside(int x, int y) const noexcept
{
    auto x0 = globalPosition[Direction::HORIZONTAL];
    auto xx = x0 + (int)globalSize[Direction::HORIZONTAL];
    auto y0 = globalPosition[Direction::VERTICAL];
    auto yy = y0 + (int)globalSize[Direction::VERTICAL];
    if (x0 <= x && x <= xx && y0 <= y && y <= yy) {
        return true;
    }
    return false;
}

void ui::Control::Set_minSize(Direction direction, unsigned int absolute) noexcept
{
    if (absolute == _minSize[direction]) return;
    _minSize[direction] = absolute;
    if (parent) parent->UpdateInQueue();
    else if (nominalParent) nominalParent->UpdateInQueue();
}

ui::Control::~Control() noexcept
{
    if (parent) parent->Remove(this);
}

ui::Control::Direction ui::Control::GetAnotherDirection(Direction direction) noexcept
{
    switch (direction) {
        case Direction::HORIZONTAL:
            return Direction::VERTICAL;
            break;
        case Direction::VERTICAL:
            return Direction::HORIZONTAL;
            break;
        default:
            assert(false); // Impossible direction.
            return Direction::VERTICAL;
            break;
    }
}

std::string ui::Control::convert(const sf::String &str)
{
    std::string result;
    for (auto c : str.toUtf8()) {
        result.push_back(c);
    }
    return std::move(result);
}

void ui::Button::Process(const sf::Event &event, const sf::RenderWindow &screen) noexcept
{
    if (!enabled) return;
    if (event.type == sf::Event::MouseMoved) {
        if (IsInside(event.mouseMove.x, event.mouseMove.y)) {
            if (!entered) {
                SetEntered(true);
                enterCallback(name, event);
            }
        } else {
            if (entered) {
                SetEntered(false);
                leaveCallback(name, event);
            }
        }
    } else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (IsInside(event.mouseButton.x, event.mouseButton.y)) {
                if (!pressed) {
                    SetPressed(true);
                    pressDownCallback(name, event);
                }
            }
        }
    } else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (pressed) {
                SetPressed(false);
                pressUpCallback(name, event);
                if (IsInside(event.mouseButton.x, event.mouseButton.y)) {    
                    clickCallback(name, event);
                }
            }
        }
    }
}

void ui::ToggleButton::SetOn(bool flag) noexcept
{
    on = flag;
}

void ui::ToggleButton::Toggle() noexcept
{
    SetOn(!GetOn());
}

void ui::ToggleButton::SetOnColor(const sf::Color &color) noexcept
{
    onColor = color;
}

void ui::ToggleButton::Draw(sf::RenderWindow &screen, float delta) noexcept
{
    if (enabled) {
        if (entered) {
            rect->SetOutlineColor(focusOutlineColor);
        } else {
            rect->SetOutlineColor(flatOutlineColor);
        }
        if (pressed) {
            rect->SetFillColor(focusBackColor);
        } else {
            rect->SetFillColor(flatBackColor);
        }
    } else {
        rect->SetFillColor(disabledBackColor);
        rect->SetOutlineColor(disabledOutlineColor);
    }
    if (on) {
        rect->SetFillColor(onColor);
    }
    layer.Draw(screen, delta);

    _DRAW_DEBUG_RECT_;
}

void ui::Button::Draw(sf::RenderWindow &screen, float delta) noexcept
{   
    if (enabled) {
        if (entered) {
            rect->SetOutlineColor(focusOutlineColor);
        } else {
            rect->SetOutlineColor(flatOutlineColor);
        }
        if (pressed) {
            rect->SetFillColor(focusBackColor);
        } else {
            rect->SetFillColor(flatBackColor);
        }
    } else {
        rect->SetFillColor(disabledBackColor);
        rect->SetOutlineColor(disabledOutlineColor);
    }
    layer.Draw(screen, delta);

    _DRAW_DEBUG_RECT_;
}

void ui::Button::SetEntered(bool flag) noexcept
{
    entered = flag;
}

void ui::Button::SetPressed(bool flag) noexcept
{
    pressed = flag;
}

void ui::Margin::SetMargin(unsigned int top, unsigned int bottom, unsigned int left, unsigned int right) noexcept
{
    margin = {top, bottom, left, right};
    UpdateInQueue();
}

void ui::Margin::SetMarginTop(unsigned int top) noexcept
{
    margin.top = top;
    UpdateInQueue();
}

void ui::Margin::SetMarginBottom(unsigned int bottom) noexcept
{
    margin.bottom = bottom;
    UpdateInQueue();
}

void ui::Margin::SetMarginLeft(unsigned int left) noexcept
{
    margin.left = left;
    UpdateInQueue();
}

void ui::Margin::SetMarginRight(unsigned int right) noexcept
{
    margin.right = right;
    UpdateInQueue();
}

void ui::Margin::Update(bool resetMinSize) noexcept
{
    unsigned int minSizeH = 0;
    unsigned int minSizeV = 0;
    for (auto child : *children) {
        if (!child->GetVisible()) continue;
        child->SetGlobalSize(Direction::HORIZONTAL, globalSize[Direction::HORIZONTAL] - margin.left - margin.right);
        child->SetGlobalSize(Direction::VERTICAL, globalSize[Direction::VERTICAL] - margin.top - margin.bottom);
        child->SetGlobalPosition(Direction::HORIZONTAL, globalPosition[Direction::HORIZONTAL] + margin.left);
        child->SetGlobalPosition(Direction::VERTICAL, globalPosition[Direction::VERTICAL] + margin.top);
        minSizeH = std::max(minSizeH, child->GetMinSize(Direction::HORIZONTAL));
        minSizeV = std::max(minSizeV, child->GetMinSize(Direction::VERTICAL));
    }
    if (resetMinSize) {
        Set_minSize(Direction::HORIZONTAL, minSizeH + margin.left + margin.right);
        Set_minSize(Direction::VERTICAL, minSizeV + margin.top + margin.bottom);
    }
}

void ui::InputBox::SetText(const std::string &text) noexcept
{
    sf::String textSatisfiysLimits = GetTextSatisfysLimits(convert(text));
    textCopy = textSatisfiysLimits;
    UpdateInQueue();
}

void ui::InputBox::SetFontSize(unsigned int size) noexcept
{
    label->SetFontSize(size);
    UpdateInQueue();
}

void ui::InputBox::SetFontColor(const sf::Color &color) noexcept
{
    label->SetFontColor(color);
    fontColor = color;
}

void ui::InputBox::SetInputtingFontColor(const sf::Color &color) noexcept
{
    inputtingFontColor = color;
}

void ui::InputBox::SetBackColor(const sf::Color &color) noexcept
{
    backColor = color;
}

void ui::InputBox::SetInputtingBackColor(const sf::Color &color) noexcept
{
    inputtingBackColor = color;
}

void ui::InputBox::SetFlickerInterval(unsigned int interval) noexcept
{
    flickerInterval = interval;
}

void ui::InputBox::SetCursorThickness(float thickness) noexcept
{
    cursorThickness = thickness;
}

void ui::InputBox::SetLengthLimit(unsigned int maxLength) noexcept
{
    lengthLimited = maxLength;
}

void ui::InputBox::SetContentLimit(ContentLimit limit) noexcept
{
    contentLimit = limit;
}

void ui::InputBox::SetSpecialCharacters(const sf::String &list) noexcept
{
    specialCharacters = list;
}

void ui::InputBox::SetFont(const std::string &fontFile) noexcept
{
    label->SetFont(fontFile);
    UpdateInQueue();
}

void ui::InputBox::SetProtectText(bool flag) noexcept
{
    protectText = flag;
    UpdateInQueue();
}

void ui::InputBox::Update(bool resetMinSize) noexcept
{
    if (protectText) {
        label->SetContent(std::string(textCopy.getSize(), '*'));
    } else {
        label->SetContent(convert(textCopy));
    }
    layer.SetGlobalSize(Direction::HORIZONTAL, globalSize[Direction::HORIZONTAL]);
    layer.SetGlobalSize(Direction::VERTICAL, globalSize[Direction::VERTICAL]);
    layer.SetGlobalPosition(Direction::HORIZONTAL, globalPosition[Direction::HORIZONTAL]);
    layer.SetGlobalPosition(Direction::VERTICAL, globalPosition[Direction::VERTICAL]);
    if (resetMinSize) {
        Set_minSize(Direction::HORIZONTAL, label->GetMinSize(Direction::HORIZONTAL));
        Set_minSize(Direction::VERTICAL, label->GetMinSize(Direction::VERTICAL));
    }
}

void ui::InputBox::Process(const sf::Event &event, const sf::RenderWindow &screen) noexcept
{
    button->Process(event, screen);

    auto input = [this](sf::Uint32 c){
        switch (c) {
            case '\b':
                SetText(convert(textCopy.substring(0, textCopy.getSize() - 1)));
                break;
            default:
                SetText(convert(textCopy + c));
                break;
        }
        cursorVisible = true;
        flickerTimer.restart();
    };
    if (event.type == sf::Event::TextEntered) {
        if (inputting) {
            input(event.text.unicode);
            inputCallback(name, event);
        }
    } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter) {
            if (inputting) {
                input('\n');
                inputCallback(name, event);
            }
        }
    } else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (!IsInside(event.mouseButton.x, event.mouseButton.y)) {
                if (inputting) {
                    SetInputting(false);
                    endCallback(name, event);
                }
            }
        }
    }
}

void ui::InputBox::Draw(sf::RenderWindow &screen, float delta) noexcept
{
    if (inputting) {
        rect->SetFillColor(inputtingBackColor);
        label->SetFontColor(inputtingFontColor);
    } else {
        rect->SetFillColor(backColor);
        label->SetFontColor(fontColor);
    }
    layer.Draw(screen, delta);

    if (inputting) {
        if (flickerTimer.getElapsedTime().asMilliseconds() > flickerInterval) {
            flickerTimer.restart();
            cursorVisible = !cursorVisible;
        }
        if (cursorVisible) {
            auto x = label->GetMinSize(Direction::HORIZONTAL);
            auto h = label->GetMinSize(Direction::VERTICAL);
            sf::RectangleShape line(sf::Vector2f(cursorThickness, h));
            line.setPosition(globalPosition[Direction::HORIZONTAL] + x, globalPosition[Direction::VERTICAL]);
            line.setFillColor(inputtingFontColor);
            screen.draw(line);
        }
    }

    _DRAW_DEBUG_RECT_;
}

void ui::InputBox::SetInputting(bool flag) noexcept
{
    inputting = flag;
}

bool ui::InputBox::IsSpecialCharacter(wchar_t c) noexcept
{
    return specialCharacters.find(c) != sf::String::InvalidPos;
}

sf::String ui::InputBox::GetTextSatisfysLimits(const sf::String &s) noexcept
{
    sf::String textSatisfiysLimits = "";
    switch (contentLimit) {
        case ContentLimit::BAN_SPECIAL_CHARACTERS:
            for (auto c : s) {
                if (!IsSpecialCharacter(c)) {
                    textSatisfiysLimits += c;
                } else {
                    exceedLimitCallback(name, {});
                }
            }
            break;
        case ContentLimit::ALLOW_SPECIAL_CHARACTERS_ONLY:
            for (auto c : s) {
                if (IsSpecialCharacter(c)) {
                    textSatisfiysLimits += c;
                } else {
                    exceedLimitCallback(name, {});
                }
            }
            break;
        default:
            textSatisfiysLimits = s;
            break;
    }
    if (lengthLimited) {
        if (s.getSize() > lengthLimited) {
            textSatisfiysLimits = textSatisfiysLimits.substring(0, lengthLimited);
            exceedLimitCallback(name, {});
        }
    }
    return textSatisfiysLimits;
}

void ui::Spacer::SetOutlineThickness(float thickness) noexcept
{
    rect.setOutlineThickness(thickness);
}

void ui::Spacer::SetOutlineColor(const sf::Color &color) noexcept
{
    rect.setOutlineColor(color);
}

void ui::Spacer::SetFillColor(const sf::Color &color) noexcept
{
    rect.setFillColor(color);
}

void ui::Spacer::Update(bool resetMinSize) noexcept
{
    rect.setSize(sf::Vector2f(globalSize[Direction::HORIZONTAL], globalSize[Direction::VERTICAL]));
    rect.setPosition(sf::Vector2f(globalPosition[Direction::HORIZONTAL], globalPosition[Direction::VERTICAL]));
}

void ui::Spacer::Draw(sf::RenderWindow &screen, float delta) noexcept
{
    screen.draw(rect);

    _DRAW_DEBUG_RECT_;
}

void ui::ScrollBar::SetPort(unsigned int absolute) noexcept
{
    port = absolute;
    if (rate + port > sum) {
        if (port <= sum) {
            rate = sum - port;
        } else {
            rate = 0;
            sum = port;
        }
    }
    UpdateInQueue();
}

void ui::ScrollBar::SetSum(unsigned int absolute) noexcept
{
    sum = absolute;
    if (sum < rate + port) {
        if (sum >= port) {
            rate = sum - port;
        } else {
            rate = 0;
            sum = port;
        }
    }
    UpdateInQueue();
}

void ui::ScrollBar::SetRate(unsigned int absolute) noexcept
{
    rate = absolute;
    if (rate + port > sum) {
        rate = sum - port;
    }
    UpdateInQueue();
}

void ui::ScrollBar::AddRate(int absolute) noexcept
{
    SetRate(std::max(0, (int)rate + absolute));
}

void ui::ScrollBar::Scroll(float delta) noexcept
{
    AddRate((reserve ? -1 : 1) * -sensitivity * delta);
}

void ui::ScrollBar::SetSensitivity(float real) noexcept
{
    sensitivity = real;
}

void ui::ScrollBar::SetReserve(bool flag) noexcept
{
    reserve = flag;
}

void ui::ScrollBar::SetBackColor(const sf::Color &color) noexcept
{
    backColor = color;
}

void ui::ScrollBar::SetFrontColor(const sf::Color &color) noexcept
{
    frontColor = color;
}

void ui::ScrollBar::UpdateTo(Direction direction) noexcept
{
    layer.SetGlobalSize(Direction::HORIZONTAL, globalSize[Direction::HORIZONTAL]);
    layer.SetGlobalSize(Direction::VERTICAL, globalSize[Direction::VERTICAL]);
    layer.SetGlobalPosition(Direction::HORIZONTAL, globalPosition[Direction::HORIZONTAL]);
    layer.SetGlobalPosition(Direction::VERTICAL, globalPosition[Direction::VERTICAL]);

    front->SetPreset(GetAnotherDirection(direction), Preset::FILL_FROM_CENTER);
    front->SetSize(direction, port * globalSize[direction] / std::max(1u, sum));
    front->SetPosition(direction, rate * globalSize[direction] / std::max(1u, sum));
}

void ui::ScrollBar::Process(const sf::Event &event, const sf::RenderWindow &screen) noexcept
{
    if (event.type == sf::Event::MouseMoved) {
        if (IsInside(event.mouseMove.x, event.mouseMove.y)) {
            background->SetFillColor(backColor);
            front->SetFillColor(frontColor);
            entered = true;
            enteredCallback(name, event);
        } else {
            background->SetFillColor(sf::Color::Transparent);
            front->SetFillColor(backColor);
            entered = false;
            leaveCallback(name, event);
        }
    } else if (event.type == sf::Event::MouseWheelScrolled) {
        if (entered) {
            Scroll(event.mouseWheelScroll.delta);
            scrollCallback(name, event);
        }
    } else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (entered) {
                dragging = true;
            }
        }
    } else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            dragging = false;
        }
    }
    
    if (dragging) {
        auto mousePosition = sf::Mouse::getPosition(screen);
        DragTo(mousePosition.x - globalPosition[Direction::HORIZONTAL], mousePosition.y - globalPosition[Direction::VERTICAL]);
        dragCallback(name, event);
        scrollCallback(name, event);
    }
}

void ui::ScrollBar::Draw(sf::RenderWindow &screen, float delta) noexcept
{
    layer.Draw(screen, delta);

    _DRAW_DEBUG_RECT_;
}

void ui::HorizontalScrollBar::Update(bool resetMinSize) noexcept
{
    UpdateTo(Direction::HORIZONTAL);
}

void ui::HorizontalScrollBar::DragTo(int x, int y) noexcept
{
    auto frontSize = front->GetGlobalSize(Direction::HORIZONTAL);
    float expected = x - (int)frontSize / 2;
    expected /= globalSize[Direction::HORIZONTAL] - frontSize;
    if (expected < 0) expected = 0;
    if (expected > 1) expected = 1;
    SetRate(expected * (sum - port));
}

void ui::VerticalScrollBar::Update(bool resetMinSize) noexcept
{
    UpdateTo(Direction::VERTICAL);
}

void ui::VerticalScrollBar::DragTo(int x, int y) noexcept
{
    auto frontSize = front->GetGlobalSize(Direction::VERTICAL);
    float expected = y - (int)frontSize / 2;
    expected /= globalSize[Direction::VERTICAL] - frontSize;
    if (expected < 0) expected = 0;
    if (expected > 1) expected = 1;
    SetRate(expected * (sum - port));
}

void ui::ScrollingBox::SetBarSize(unsigned int size) noexcept
{
    barSize = size;
    UpdateInQueue();
}

void ui::ScrollingBox::SetBarAtFront(bool flag) noexcept
{
    barAtFront = flag;
    UpdateInQueue();
}

void ui::ScrollingBox::SetGap(int absolute) noexcept
{
    GetBox()->SetGap(absolute);
}

void ui::ScrollingBox::SetRate(unsigned int absolute) noexcept
{
    GetBar()->SetRate(absolute);
    GetBox()->SetDelta(-GetBar()->GetRate());
}

void ui::ScrollingBox::SetSensitivity(float real) noexcept
{
    GetBar()->SetSensitivity(real);
}

void ui::ScrollingBox::SetReserve(bool flag) noexcept
{
    GetBar()->SetReserve(flag);
}

void ui::ScrollingBox::SetInsideBoxScrollable(bool flag) noexcept
{
    insideScrollable = flag;
}

void ui::HorizontalScrollingBox::Update(bool resetMinSize) noexcept
{
    SharedUpdate(resetMinSize, Direction::HORIZONTAL);
}

ui::HorizontalScrollingBox::~HorizontalScrollingBox() noexcept
{
    box->UnsyncChildren();
}

void ui::ScrollingBox::Process(const sf::Event &event, const sf::RenderWindow &screen) noexcept
{
    if (insideScrollable) {
        if (event.type == sf::Event::MouseMoved) {
            if (GetBox()->IsInside(event.mouseMove.x, event.mouseMove.y)) {
                rect->SetOutlineColor(sf::Color::Blue);
                enteredBox = true;
            } else {
                rect->SetOutlineColor(sf::Color::White);
                enteredBox = false;
            }
        } else if (event.type == sf::Event::MouseWheelScrolled) {
            if (enteredBox) {
                GetBar()->Scroll(event.mouseWheelScroll.delta);
                GetBox()->SetDelta(-GetBar()->GetRate());
            }
        }
    }
    GetBar()->Process(event, screen);

    Container::Process(event, screen);
}

void ui::ScrollingBox::Draw(sf::RenderWindow &screen, float delta) noexcept
{
    rect->Draw(screen, delta);

    Container::Draw(screen, delta);

    GetBar()->Draw(screen, delta);

    _DRAW_DEBUG_RECT_;
}

void ui::ScrollingBox::UpdateInQueue(bool argv) noexcept
{
    Container::UpdateInQueue(argv);
    GetBox()->UpdateInQueue(argv);
}

void ui::ScrollingBox::SharedUpdate(bool resetMinSize, Direction direction) noexcept
{
    layer.SetGlobalSize(Direction::HORIZONTAL, globalSize[Direction::HORIZONTAL]);
    layer.SetGlobalSize(Direction::VERTICAL, globalSize[Direction::VERTICAL]);
    layer.SetGlobalPosition(Direction::HORIZONTAL, globalPosition[Direction::HORIZONTAL]);
    layer.SetGlobalPosition(Direction::VERTICAL, globalPosition[Direction::VERTICAL]);

    auto another = GetAnotherDirection(direction);

    if (resetMinSize) {
        Set_minSize(another, GetBox()->GetMinSize(another) + barSize);
    }

    rect->SetPreset(direction, Preset::FILL_FROM_CENTER);
    if (barAtFront) {
        GetBar()->SetPreset(another, Preset::PLACE_AT_FRONT);
        GetBox()->SetPreset(another, Preset::PLACE_AT_END);
        rect->SetPreset(another, Preset::PLACE_AT_END);
    } else {
        GetBar()->SetPreset(another, Preset::PLACE_AT_END);
        GetBox()->SetPreset(another, Preset::PLACE_AT_FRONT);
        rect->SetPreset(another, Preset::PLACE_AT_FRONT);
    }

    GetBox()->SetSize(another, globalSize[another] - barSize);
    rect->SetSize(another, globalSize[another] - barSize);
    GetBar()->SetSize(another, barSize);

    GetBar()->SetPort(globalSize[direction]);
    GetBar()->SetSum(GetBox()->GetTotalSize());
    GetBox()->SetDelta(-GetBar()->GetRate());
}

void ui::VerticalScrollingBox::Update(bool resetMinSize) noexcept
{
    SharedUpdate(resetMinSize, Direction::VERTICAL);
}

ui::VerticalScrollingBox::~VerticalScrollingBox() noexcept
{
    box->UnsyncChildren();
}

void ui::LoadingRing::SetColor(const sf::Color &color) noexcept
{
    circle.setOutlineColor(color);
}

void ui::LoadingRing::SetThickness(float thickness) noexcept
{
    circle.setOutlineThickness(thickness);
}

void ui::LoadingRing::SetSpeed(float speed) noexcept
{
    speed = speed;
}

void ui::Timer::SetInterval(unsigned int newInterval) noexcept
{
    interval = newInterval;
    clock.restart();
}

void ui::Timer::Start() noexcept
{
    started = true;
    clock.restart();
}

void ui::Timer::Stop() noexcept
{
    started = false;
}

void ui::Timer::Draw(sf::RenderWindow &screen, float delta) noexcept {
    if (started && clock.getElapsedTime().asMilliseconds() > interval) { 
        callback(name, {});
        clock.restart();
    }

    _DRAW_DEBUG_RECT_;
}

void ui::LoadingRing::Draw(sf::RenderWindow &screen, float delta) noexcept
{
    Timer::Draw(screen, delta);

    if (globalSize[Direction::HORIZONTAL] > globalSize[Direction::VERTICAL]) {
        circle.setRadius(globalSize[Direction::VERTICAL] / 2 * (sin(rate) + 1) / 2);
        circle.setPosition(globalPosition[Direction::HORIZONTAL] + globalSize[Direction::HORIZONTAL] / 2 - circle.getRadius(), 
            globalPosition[Direction::VERTICAL] + globalSize[Direction::VERTICAL] / 2 - circle.getRadius());
    } else {
        circle.setRadius(globalSize[Direction::HORIZONTAL] / 2 * (sin(rate) + 1) / 2);
        circle.setPosition(globalPosition[Direction::HORIZONTAL] + globalSize[Direction::HORIZONTAL] / 2 - circle.getRadius(), 
            globalPosition[Direction::VERTICAL] + globalSize[Direction::VERTICAL] / 2 - circle.getRadius());
    }
    if (started) {
        rate += speed * delta;
    }
    if (rate > 2 * PI) rate = 0;

    screen.draw(circle);

    _DRAW_DEBUG_RECT_;
}

void ui::LoadingRingWithText::SetFontSize(unsigned int size) noexcept
{
    label->SetFontSize(size);
    UpdateInQueue();
}

void ui::LoadingRingWithText::SetFont(const std::string &fontFile) noexcept
{
    label->SetFont(fontFile);
    UpdateInQueue();
}

void ui::LoadingRingWithText::SetTextHeight(unsigned int height) noexcept
{
    textHeight = height;
    UpdateInQueue();
}

void ui::LoadingRingWithText::SetRingHeight(unsigned int height) noexcept
{
    ringHeight = height;
    UpdateInQueue();
}

void ui::LoadingRingWithText::SetText(const std::string &newText) noexcept
{
    text = newText;
    UpdateInQueue();
}

void ui::LoadingRingWithText::SetCount(int newCount) noexcept
{
    count = newCount;
}

void ui::LoadingRingWithText::SetFontColor(const sf::Color &color) noexcept
{
    label->SetFontColor(color);
}

void ui::LoadingRingWithText::SetInterval(unsigned int interval) noexcept
{
    ring->SetInterval(interval);
}

void ui::LoadingRingWithText::Start() noexcept
{
    ring->Start();
}

void ui::LoadingRingWithText::Stop() noexcept
{
    ring->Stop();
}

void ui::LoadingRingWithText::Update(bool resetMinSize) noexcept
{
    label->SetSize(Direction::VERTICAL, textHeight);
    label->SetContent(text + "..." + ToStr(count));
    ring->SetSize(Direction::VERTICAL, ringHeight);
    layer.SetGlobalSize(Direction::HORIZONTAL, globalSize[Direction::HORIZONTAL]);
    layer.SetGlobalSize(Direction::VERTICAL, globalSize[Direction::VERTICAL]);
    layer.SetGlobalPosition(Direction::HORIZONTAL, globalPosition[Direction::HORIZONTAL]);
    layer.SetGlobalPosition(Direction::VERTICAL, globalPosition[Direction::VERTICAL]);
    if (resetMinSize) {
        Set_minSize(Direction::HORIZONTAL, layer.GetMinSize(Direction::HORIZONTAL));
        Set_minSize(Direction::VERTICAL, layer.GetMinSize(Direction::VERTICAL));
    }
}

void ui::LoadingRingWithText::Process(const sf::Event &event, const sf::RenderWindow &screen) noexcept
{
    layer.Process(event, screen);
}

void ui::LoadingRingWithText::Draw(sf::RenderWindow &screen, float delta) noexcept
{
    layer.Draw(screen, delta);

    _DRAW_DEBUG_RECT_;
}

void ui::LoadingRingWithText::Count() noexcept
{
    --count;
    UpdateInQueue();
    countCallback(name, {});
    if (count == 0) {
        finishedCallback(name, {});
    }
}

void ui::PictureBox::SetPicture(const std::string &filename) noexcept
{
    if (!texture.loadFromFile(convert(filename))) {
        error = true;
    }
    originSize = {texture.getSize().x, texture.getSize().y};
    if (originSize.width == 0) {
        originSize.width = 200;
    }
    if (originSize.height == 0) {
        originSize.height = 100;
    }
    sprite.setTexture(texture);
    UpdateInQueue();
}

void ui::PictureBox::SetScale(unsigned int percentage) noexcept
{
    SetSize(originSize.width * percentage / 100, originSize.height * percentage / 100);
}

void ui::PictureBox::KeepWidth(unsigned int absolute) noexcept
{
    SetSize(absolute, absolute * originSize.height / std::max(1u, originSize.width));
}

void ui::PictureBox::KeepHeight(unsigned int absolute) noexcept
{
    SetSize(absolute * originSize.width / std::max(1u, originSize.height), absolute);
}

void ui::PictureBox::Draw(sf::RenderWindow &screen, float delta) noexcept
{
    if (!error) {
        screen.draw(sprite);
    } else {
        sf::RectangleShape rect(sf::Vector2f(globalSize[Direction::HORIZONTAL], globalSize[Direction::VERTICAL]));
        rect.setPosition(sf::Vector2f(globalPosition[Direction::HORIZONTAL], globalPosition[Direction::VERTICAL]));
        rect.setFillColor(sf::Color::Red);
        screen.draw(rect);
    }

    _DRAW_DEBUG_RECT_;
}

void ui::PictureBox::Update(bool resetMinSize) noexcept
{
    sprite.setPosition(globalPosition[Direction::HORIZONTAL], globalPosition[Direction::VERTICAL]);
    sprite.setScale((float)globalSize[Direction::HORIZONTAL] / std::max(1u, originSize.width), (float)globalSize[Direction::VERTICAL] / std::max(1u, originSize.height));
}
