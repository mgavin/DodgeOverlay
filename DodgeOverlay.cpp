#include "pch.h"
#include "DodgeOverlay.h"
#include <fstream>

using std::string;
using namespace DodgeOverlay;

const string MENU_NAME = "dodgeoverlay";
const string PLUGIN_NAME = "Dodge Overlay";


BAKKESMOD_PLUGIN(DodgeOverlayPlugin, PLUGIN_NAME.c_str(), "0.0.3", 0)

inline ImVec2 operator+(const ImVec2 v1, const ImVec2 v2) {
    return ImVec2{ v1.x + v2.x, v1.y + v2.y };
}

inline ImVec2 operator-(const ImVec2 v1, const ImVec2 v2) {
    return ImVec2{ v1.x - v2.x, v1.y - v2.y };
}

inline ImVec2 operator*(const ImVec2 v1, const ImVec2 v2) {
    return ImVec2{ v1.x * v2.x, v1.y * v2.y };
}

inline ImVec2 operator/(const ImVec2 v1, const int i) {
    return ImVec2{ v1.x / i, v1.y / i };
}

void DodgeOverlayPlugin::onLoad() {
    m_dodgeDeadzone = gameWrapper->GetSettings().GetGamepadSettings().DodgeInputThreshold;
    m_configurationFilePath = gameWrapper->GetBakkesModPath() / m_configurationFilePath;

#pragma region register cvars
#pragma region dodgeoverlayWinXPos
    CVarWrapper tempCvar = cvarManager->getCvar("dodgeoverlayWinXPos");
    if(tempCvar.IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayWinXPos", "0.0");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_windowPosition.x = now.getFloatValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayWinYPos
    if((tempCvar = cvarManager->getCvar("dodgeoverlayWinYPos")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayWinYPos", "0.0");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_windowPosition.y = now.getFloatValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayScale
    if((tempCvar = cvarManager->getCvar("dodgeoverlayScale")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayScale", "1.0");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_scale = now.getFloatValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayShowNums
    if((tempCvar = cvarManager->getCvar("dodgeoverlayShowNums")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayShowNums", "1");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_fShowNums = now.getBoolValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayStickBorderColor
    if((tempCvar = cvarManager->getCvar("dodgeoverlayStickBorderColor")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayStickBorderColor", "(1.0, 1.0, 1.0, 1.0)");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            LinearColor color = now.getColorValue();
            m_stickBorderColor = ImColor(color.R, color.G, color.B, color.A);
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayStickLocationColor
    if((tempCvar = cvarManager->getCvar("dodgeoverlayStickLocationColor")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayStickLocationColor", "(1.0, 1.0, 1.0, 1.0)");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            LinearColor color = now.getColorValue();
            m_stickLocationColor = ImColor(color.R, color.G, color.B, color.A);
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion 
#pragma region dodgeoverlayDodgeDeadzoneColor
    if((tempCvar = cvarManager->getCvar("dodgeoverlayDodgeDeadzoneColor")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayDodgeDeadzoneColor", "(1.0, 1.0, 1.0, 1.0)");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            LinearColor color = now.getColorValue();
            m_dodgeDeadzoneColor = ImColor(color.R, color.G, color.B, color.A);
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayStickLocationSize
    if((tempCvar = cvarManager->getCvar("dodgeoverlayStickLocationSize")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayStickLocationSize", "5.0");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_stickLocationSize = now.getFloatValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayShowDodgeDeadzoneBorder
    if((tempCvar = cvarManager->getCvar("dodgeoverlayShowDodgeDeadzoneBorder")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayShowDodgeDeadzoneBorder", "1");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_fShowDodgeDeadzoneBorder = now.getBoolValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayDodgeDeadzoneBorderThickness
    if((tempCvar = cvarManager->getCvar("dodgeoverlayDodgeDeadzoneBorderThickness")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayDodgeDeadzoneBorderThickness", "1.0");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_dodgeDeadzoneBorderThickness = now.getFloatValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayDodgeDeadzoneCrossedAlpha
    if((tempCvar = cvarManager->getCvar("dodgeoverlayDodgeDeadzoneCrossedAlpha")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayDodgeDeadzoneCrossedAlpha", "0.1");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_dodgeDeadzoneCrossedAlpha = now.getFloatValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayShowFlipCancelMeter
    if((tempCvar = cvarManager->getCvar("dodgeoverlayShowFlipCancelMeter")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayShowFlipCancelMeter", "1");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_fShowFlipCancelMeter = now.getBoolValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayFlipFlipCancelMeter
    if((tempCvar = cvarManager->getCvar("dodgeoverlayFlipFlipCancelMeter")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayFlipFlipCancelMeter", "0");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_fFlipFlipCancelMeter = now.getBoolValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayShowFlipCancelMeterPosition
    if((tempCvar = cvarManager->getCvar("dodgeoverlayShowFlipCancelMeterPosition")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayShowFlipCancelMeterPosition", "0");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_flipCancelMeterPosition = now.getIntValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma endregion

    if (std::ifstream(m_configurationFilePath)) {
        cvarManager->loadCfg(m_configurationFilePath.string());
    }

    //TODO: try HookEvent<T*>
    gameWrapper->HookEvent("Function TAGame.PlayerInput_TA.PlayerInput",
        [this](std::string) {
            CarWrapper car = gameWrapper->GetLocalCar();
            if(car) {
                    ControllerInput inputs = car.GetInput();

                    m_stickLocation.x = inputs.DodgeStrafe + inputs.Roll;
                    m_stickLocation.y = inputs.DodgeForward;
                    m_dodgeDeadzoneRoll = 0.0f;
                    if(fabs(inputs.Roll) >= 1.0f) {
                        m_stickLocation.x = std::max(std::min(m_stickLocation.x, 1.0f), -1.0f);
                        m_dodgeDeadzoneRoll = m_dodgeDeadzone;
                    }

                    m_amDodging = car.IsDodging();
                    m_theTime = car.GetWorldInfo().GetTimeSeconds(); 
                    DodgeComponentWrapper dcw = car.GetDodgeComponent();
                    if (dcw) {
                        DODGE_TORQUE_TIME = dcw.GetDodgeTorqueTime();
                        MIN_DODGE_TORQUE_TIME = dcw.GetMinDodgeTorqueTime();
                    }
            }
        });

    gameWrapper->HookEvent("Function CarComponent_Dodge_TA.Active.BeginState",
        [this](std::string) {
            CarWrapper car = gameWrapper->GetLocalCar();
            // somehow car.GetbJumped() is not good enough for
            // checking if the local car has jumped at this point
            if (car && car.GetInput().Jumped) {
                if (m_fShowFlipCancelMeterTimer) {
                    if (!(fabs(m_stickLocation.y - 0.0f) <= 10e-6)) {
                        m_fStartFlipCancelMeterTimer = true;
                        m_isDodgePositive = m_stickLocation.y > 0.0f ? true : false;
                    }
                    m_amDodging = true;
                    m_timeDodged = car.GetWorldInfo().GetTimeSeconds();
                }
            }
        });

    gameWrapper->HookEvent("Function TAGame.GFxData_Settings_TA.SetDodgeInputThreshold",
        [this](std::string) {
            m_dodgeDeadzone = gameWrapper->GetSettings().GetGamepadSettings().DodgeInputThreshold;
        });
    gameWrapper->Execute(
        [this](GameWrapper* gameWrapper) {
            cvarManager->executeCommand("openmenu dodgeoverlay;");
        });
}

void DodgeOverlayPlugin::RenderSettings() {
    using namespace ImGui;
    if(DragFloat("Window position x", &m_windowPosition.x, 1.0f, 50.0f * m_finalScale, m_displaySize.x - m_windowSize.x / 4, "%.1f")) {
        m_localCvars.at("dodgeoverlayWinXPos").setValue(m_windowPosition.x);
    }
    if(DragFloat("Window position y", &m_windowPosition.y, 1.0f, (50.0f) * m_finalScale, m_displaySize.y - m_windowSize.x / 4 - m_fShowNums * 12.0f * m_finalScale, "%.1f")) {
        m_localCvars.at("dodgeoverlayWinYPos").setValue(m_windowPosition.y);
    }

    if(DragFloat("Scale", &m_scale, 0.1f, 1.0f, 10.0f, "%.1f")) {
        m_localCvars.at("dodgeoverlayScale").setValue(m_scale);
    }
    Text(("Final scale: " + std::to_string(m_finalScale)).c_str());
    if(DragFloat("Stick location size", &m_stickLocationSize, 0.1f, 1.0f, 100.0f, "%.1f")) {
        m_localCvars.at("dodgeoverlayStickLocationSize").setValue(m_stickLocationSize);
    }
    if(Checkbox("Show only dodge deadzone border", &m_fShowDodgeDeadzoneBorder)) {
        m_localCvars.at("dodgeoverlayShowDodgeDeadzoneBorder").setValue(m_fShowDodgeDeadzoneBorder);
    }
    if(m_fShowDodgeDeadzoneBorder) {
        if(DragFloat("Dodge deadzone border thickness", &m_dodgeDeadzoneBorderThickness, 0.1f, 0.1f, 10.0f, "%.1f")) {
            m_localCvars.at("dodgeoverlayDodgeDeadzoneBorderThickness").setValue(m_dodgeDeadzoneBorderThickness);
        };
    }
    if(Checkbox("Show outputs nums", &m_fShowNums)) {
        m_localCvars.at("dodgeoverlayShowNums").setValue(m_fShowNums);
    }
    if (DragFloat("Background color alpha when deadzone has been crossed", &m_dodgeDeadzoneCrossedAlpha, 0.01f, 0.0f, 1.0f, "%.2f")) {
        m_localCvars.at("dodgeoverlayDodgeDeadzoneCrossedAlpha").setValue(m_dodgeDeadzoneCrossedAlpha);
    }
    {
        float* colors[4] = { &m_stickBorderColor.Value.x, &m_stickBorderColor.Value.y, &m_stickBorderColor.Value.z, &m_stickBorderColor.Value.w };
        if (ColorEdit4("Stick border color", *colors, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
            LinearColor color = LinearColor();
            color.R = m_stickBorderColor.Value.x;
            color.G = m_stickBorderColor.Value.y;
            color.B = m_stickBorderColor.Value.z;
            color.A = m_stickBorderColor.Value.w;
            m_localCvars.at("dodgeoverlayStickBorderColor").setValue(color);
        };
    }
    {
        float* colors[4] = { &m_stickLocationColor.Value.x, &m_stickLocationColor.Value.y, &m_stickLocationColor.Value.z, &m_stickLocationColor.Value.w };
        if (ColorEdit4("Stick location color", *colors, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
            LinearColor color = LinearColor();
            color.R = m_stickLocationColor.Value.x;
            color.G = m_stickLocationColor.Value.y;
            color.B = m_stickLocationColor.Value.z;
            color.A = m_stickLocationColor.Value.w;
            m_localCvars.at("dodgeoverlayStickLocationColor").setValue(color);
        };
    }
    {
        float* colors[4] = { &m_dodgeDeadzoneColor.Value.x, &m_dodgeDeadzoneColor.Value.y, &m_dodgeDeadzoneColor.Value.z, &m_dodgeDeadzoneColor.Value.w };
        if (ColorEdit4("Dodge deadzone color", *colors, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
            LinearColor color = LinearColor();
            color.R = m_dodgeDeadzoneColor.Value.x;
            color.G = m_dodgeDeadzoneColor.Value.y;
            color.B = m_dodgeDeadzoneColor.Value.z;
            color.A = m_dodgeDeadzoneColor.Value.w;
            m_localCvars.at("dodgeoverlayDodgeDeadzoneColor").setValue(color);
        };
    }

    if (Checkbox("Show flip cancel momentum meter", &m_fShowFlipCancelMeter)) {
        m_localCvars.at("dodgeoverlayShowFlipCancelMeter").setValue(m_fShowFlipCancelMeter);
    }
    if (m_fShowFlipCancelMeter) {
        Combo("Which side should the meter go?", &m_flipCancelMeterPosition, m_flipCancelMeterPositionOptions, IM_ARRAYSIZE(m_flipCancelMeterPositionOptions));


        if (Checkbox("Flip flip cancel momentum meter ends?", &m_fFlipFlipCancelMeter)) {
            m_localCvars.at("dodgeoverlayFlipFlipCancelMeter").setValue(m_fFlipFlipCancelMeter);
        }
    }

    TextUnformatted("Dodge Overlay plugin settings");
}

string DodgeOverlayPlugin::GetPluginName() {
    return PLUGIN_NAME;
}

void DodgeOverlayPlugin::SetImGuiContext(uintptr_t ctx) {
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

void DodgeOverlayPlugin::Render() {
    ServerWrapper server = (gameWrapper->IsInOnlineGame()) ? (gameWrapper->GetOnlineGame()) : (gameWrapper->GetGameEventAsServer());
    if (!server.IsNull() && !server.GetbMatchEnded()) {
        RenderImGui();
    }
}

void DodgeOverlayPlugin::RenderImGui() {
    using namespace ImGui;
    SetNextWindowBgAlpha(0.0f);
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs;
    if (Begin(GetMenuTitle().c_str(), &m_fWindowOpen, windowFlags)) {
        m_displaySize = GetIO().DisplaySize;
        m_finalScale = m_scale * m_displaySize.y / 1000;
        m_windowSize = ImVec2(200.0f * m_finalScale, 200.0f * m_finalScale);
        m_radius = 50.0f * m_finalScale;
        ImVec2 finalWindowPosition = m_windowPosition - ImVec2(50.0f * m_finalScale, 50.0f * m_finalScale) - m_windowSize / 4;
        SetWindowSize(m_windowSize);
        SetWindowFontScale(m_finalScale);
        SetWindowPos(finalWindowPosition);

        ImDrawList* drawList = GetWindowDrawList();
        ImVec2 p = GetCursorScreenPos();

        ImVec2 stickCenter = p + ImVec2(m_windowSize.x / 2, m_windowSize.x / 2);    // Because I need only square center position excluding outputs nums height
        drawList->AddQuad(stickCenter + ImVec2(-m_radius, m_radius), stickCenter + ImVec2(m_radius, m_radius), stickCenter + ImVec2(m_radius, -m_radius), stickCenter + ImVec2(-m_radius, -m_radius), m_stickBorderColor);
        drawList->AddCircleFilled(stickCenter + ImVec2(m_stickLocation.x * m_radius, -m_stickLocation.y * m_radius), m_radius * m_stickLocationSize / 100, m_stickLocationColor, 0);
        float tempDodgeDeadzone = m_dodgeDeadzone - m_dodgeDeadzoneRoll;
        if (m_fShowDodgeDeadzoneBorder) {
            drawList->AddQuad(stickCenter + ImVec2(-tempDodgeDeadzone * m_radius, 0.0f), stickCenter + ImVec2(0.0f, tempDodgeDeadzone * m_radius), stickCenter + ImVec2(tempDodgeDeadzone * m_radius, 0.0f), stickCenter + ImVec2(0.0f, -tempDodgeDeadzone * m_radius), m_dodgeDeadzoneColor, m_dodgeDeadzoneBorderThickness * m_finalScale);
        }
        else {
            drawList->AddQuadFilled(stickCenter + ImVec2(-tempDodgeDeadzone * m_radius, 0.0f), stickCenter + ImVec2(0.0f, tempDodgeDeadzone * m_radius), stickCenter + ImVec2(tempDodgeDeadzone * m_radius, 0.0f), stickCenter + ImVec2(0.0f, -tempDodgeDeadzone * m_radius), m_dodgeDeadzoneColor);
        }
        ImColor color = (std::abs(m_stickLocation.x) + std::abs(m_stickLocation.y) < tempDodgeDeadzone) ? ImColor(1.0f, 0.0f, 0.0f, m_dodgeDeadzoneCrossedAlpha) : ImColor(0.0f, 1.0f, 0.0f, m_dodgeDeadzoneCrossedAlpha);
        drawList->AddQuadFilled(stickCenter + ImVec2(-m_radius + 1.0f, m_radius - 1.0f), stickCenter + ImVec2(m_radius - 1.0f, m_radius - 1.0f), stickCenter + ImVec2(m_radius - 1.0f, -m_radius + 1.0f), stickCenter + ImVec2(-m_radius + 1.0f, -m_radius + 1.0f), color);
        if (m_fShowNums) {
            drawList->AddText(stickCenter + ImVec2(-m_radius, m_radius), m_stickLocationColor, ("X: " + std::format("{:.2f}", m_stickLocation.x)).c_str());
            drawList->AddText(stickCenter + ImVec2(0, m_radius), m_stickLocationColor, ("Y: " + std::format("{:.2f}", m_stickLocation.y)).c_str());
        }

        // Flip Momentum Meter Section
        if (m_fShowFlipCancelMeter) {
            const float time_diff = m_theTime - m_timeDodged;
            const bool IN_MIN_DODGE_TIME = (time_diff - MIN_DODGE_TORQUE_TIME) < 0.0f;
            const float percSize = IN_MIN_DODGE_TIME ? 1.0f : (1.0f - (fmin(time_diff, DODGE_TORQUE_TIME) / DODGE_TORQUE_TIME));
            const float cancel_factor = 0.16f * std::clamp((static_cast<int>(fabs(m_stickLocation.y) * 100) - 94), 0, 6);
            // draw the meter
            switch (m_flipCancelMeterPosition) {
            case FLIPCANCELMETERPOSITION::LEFT:
            {
                // border
                ImVec2 leftBorderTopLeft = stickCenter + ImVec2{ -m_radius - 50.0f, -m_radius};
                ImVec2 leftBorderBotRight = stickCenter + ImVec2{ -m_radius - 5.0f, m_radius};
                drawList->AddRect(leftBorderTopLeft, leftBorderBotRight, ImColor{ 1.0f, 1.0f, 1.0f, 1.0f });

                // inside
                ImVec2 leftInnerTopLeft = stickCenter + ImVec2{ -m_radius - 49.0f, -m_radius + 1.0f };
                ImVec2 leftInnerBotRight = stickCenter + ImVec2{ -m_radius - 6.0f, m_radius - 1.0f };
                drawList->AddRectFilled(leftInnerTopLeft, leftInnerBotRight, ImColor{ 1.0f, 1.0f, 1.0f, m_dodgeDeadzoneCrossedAlpha });

                // draw line in middle
                ImVec2 leftLineLeft = stickCenter + ImVec2{ -m_radius - 50.0f, 0.0f };
                ImVec2 leftLineRight = stickCenter + ImVec2{ -m_radius - 5.0f, 0.0f };
                drawList->AddLine(leftLineLeft, leftLineRight, ImColor{ 1.0f, 1.0f, 1.0f, 1.0f });

                if (m_fStartFlipCancelMeterTimer || true) {
                    if (m_isDodgePositive) {
                        // draw upward box
                        ImVec2 rectTopLeft = stickCenter + (ImVec2{ -m_radius - 49.0f, -m_radius + 1.0f } * ImVec2{ 1.0f, percSize });
                        ImVec2 rectBotRight = stickCenter + ImVec2{ -m_radius - 6.0f, 0.0f };
                        drawList->AddRectFilled(rectTopLeft, rectBotRight, m_flipCancelMeterAngMomPosSide);

                        // draw downward arrow
                        if (m_stickLocation.y < -0.8f && !IN_MIN_DODGE_TIME) { // -0.8 is enough resolution to catch this
                            ImVec2 triLeft = stickCenter + ImVec2{ -m_radius - 49.0f, 0.0f };
                            ImVec2 triMiddle = stickCenter + ImVec2{ -m_radius - 27.5f, m_radius * cancel_factor };
                            ImVec2 triRight = stickCenter + ImVec2{ -m_radius - 6.0f, 0.0f };
                            drawList->AddTriangleFilled(triLeft, triMiddle, triRight, m_flipCancelMeterAngMomNegSide);
                        }
                    } else {
                        // draw downward box
                        ImVec2 rectTopLeft = stickCenter + ImVec2{ -m_radius - 49.0f, 0.0f };
                        ImVec2 rectBotRight = stickCenter + (ImVec2{ -m_radius - 6.0f, m_radius - 1.0f } * ImVec2{ 1.0f, percSize });
                        drawList->AddRectFilled(rectTopLeft, rectBotRight, m_flipCancelMeterAngMomNegSide);

                        // draw upward arrow                        
                        if (m_stickLocation.y > 0.8f && !IN_MIN_DODGE_TIME) { // 0.8 is enough resolution to catch this
                            ImVec2 left = stickCenter + ImVec2{ -m_radius - 49.0f, 0.0f };
                            ImVec2 middle = stickCenter + ImVec2{ -m_radius - 27.5f, -m_radius * cancel_factor };
                            ImVec2 right = stickCenter + ImVec2{ -m_radius - 6.0f, 0.0f };
                            drawList->AddTriangleFilled(left, middle, right, m_flipCancelMeterAngMomPosSide);
                        }
                    }
                }
            }
            break;
            case FLIPCANCELMETERPOSITION::TOP:
                // determine color
                std::swap(m_flipCancelMeterAngMomNegSide, m_flipCancelMeterAngMomPosSide);

                // border
                drawList->AddRect(stickCenter + ImVec2{ -m_radius, -m_radius - 50.0f}, stickCenter + ImVec2{ m_radius, -m_radius - 5.0f }, ImColor{ 1.0f, 1.0f, 1.0f, 1.0f });

                // inside
                drawList->AddRectFilled(stickCenter + ImVec2{ -m_radius, -m_radius - 49.0f }, stickCenter + ImVec2{ m_radius - 1.0f, -m_radius - 6.0f }, ImColor{ 1.0f, 1.0f, 1.0f, m_dodgeDeadzoneCrossedAlpha });

                // draw line in middle
                drawList->AddLine(stickCenter + ImVec2{ 0.0f , -m_radius - 50.0f }, stickCenter + ImVec2{ 0.0f, -m_radius - 5.0f }, ImColor{ 1.0f, 1.0f, 1.0f, 1.0f });
                break;
            case FLIPCANCELMETERPOSITION::RIGHT:
                // border
                drawList->AddRect(stickCenter + ImVec2{ m_radius + 5.0f, -m_radius}, stickCenter + ImVec2{ m_radius + 50.0f, m_radius}, ImColor{ 1.0f, 1.0f, 1.0f, 1.0f });

                // inside
                drawList->AddRectFilled(stickCenter + ImVec2{ m_radius + 6.0f, -m_radius + 1.0f }, stickCenter + ImVec2{ m_radius + 49.0f, m_radius - 1.0f }, ImColor{ 1.0f, 1.0f, 1.0f, m_dodgeDeadzoneCrossedAlpha });

                // draw line in middle
                drawList->AddLine(stickCenter + ImVec2{ m_radius + 5.0f, 0.0f }, stickCenter + ImVec2{ m_radius + 50.0f, 0.0f }, ImColor{ 1.0f, 1.0f, 1.0f, 1.0f });
                break;
            case FLIPCANCELMETERPOSITION::INLAID:
                // inside
                drawList->AddRectFilled(stickCenter + ImVec2{ -m_radius + 1.0f, -m_radius + 1.0f }, stickCenter + ImVec2{ m_radius + 1.0f, m_radius + 1.0f }, ImColor{ 1.0f, 1.0f, 1.0f, m_dodgeDeadzoneCrossedAlpha });
                
                // draw line in middle
                drawList->AddLine(stickCenter + ImVec2{ -m_radius, 0.0f }, stickCenter + ImVec2{ m_radius, 0.0f }, ImColor{ 1.0f, 1.0f, 1.0f, 1.0f });
                break;
            };

            if (m_fStartFlipCancelMeterTimer) {
                if (!m_amDodging) {
                    cvarManager->log(std::format("DONE WITH THE DODGE. TIME IT TOOK: {}", m_theTime - m_timeDodged));
                    m_fStartFlipCancelMeterTimer = false;
                }
            }           
        }

        PopStyleVar(2);
    }
    End();
}

string DodgeOverlayPlugin::GetMenuName() {
    return MENU_NAME;
}

string DodgeOverlayPlugin::GetMenuTitle() {
    return PLUGIN_NAME;
}

bool DodgeOverlayPlugin::ShouldBlockInput() {
    return false;
}

bool DodgeOverlayPlugin::IsActiveOverlay() {
    return false;
}

void DodgeOverlayPlugin::OnOpen() {
    m_fWindowOpen = true;
}

void DodgeOverlayPlugin::OnClose() {
    m_fWindowOpen = false;
}

void DodgeOverlayPlugin::writeCfg() {
    std::fstream configurationFile;

    configurationFile.open(m_configurationFilePath, std::ios::out);

    for(auto cvar : m_localCvars) {
        configurationFile << cvar.second.getCVarName() + " \"" + cvar.second.getStringValue() + "\"\n";
    }

    configurationFile.close();
}

void DodgeOverlayPlugin::onUnload() {
    writeCfg();
}
