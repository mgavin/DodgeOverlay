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
#pragma region dodgeoverlayShowLastDodgeMarker
    if((tempCvar = cvarManager->getCvar("dodgeoverlayShowLastDodgeMarker")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayShowLastDodgeMarker", "1");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_fShowLastDodgeMarker = now.getBoolValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayShowLastDoubleJumpMarker
    if((tempCvar = cvarManager->getCvar("dodgeoverlayShowLastDoubleJumpMarker")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayShowLastDoubleJumpMarker", "1");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_fShowLastDoubleJumpMarker = now.getBoolValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayClearLastDodgeMarkerAfterJumping
    if((tempCvar = cvarManager->getCvar("dodgeoverlayClearLastDodgeMarkerAfterJumping")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayClearLastDodgeMarkerAfterJumping", "1");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_fClearLastDodgeMarkerAfterJumping = now.getBoolValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayLastDodgeMarkerShape
    if((tempCvar = cvarManager->getCvar("dodgeoverlayLastDodgeMarkerShape")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayLastDodgeMarkerShape", "1");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_lastDodgeMarkerShapeSelection = std::clamp(now.getIntValue(), 0, 4);
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayLastDodgeMarkerScale
    if((tempCvar = cvarManager->getCvar("dodgeoverlayLastDodgeMarkerScale")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayLastDodgeMarkerScale", "5");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_lastDodgeMarkerScale = now.getIntValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayLastDodgeMarkerThickness
    if((tempCvar = cvarManager->getCvar("dodgeoverlayLastDodgeMarkerThickness")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayLastDodgeMarkerThickness", "1.0");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_lastDodgeMarkerThickness = now.getFloatValue();
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion
#pragma region dodgeoverlayFadeLastDodgeMarker
    if((tempCvar = cvarManager->getCvar("dodgeoverlayFadeLastDodgeMarker")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayFadeLastDodgeMarker", "0");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_fFadeLastDodgeMarker = now.getBoolValue();
            m_lastDodgeMarkerFadeFactor = 1.0f / m_lastDodgeMarkerFadeTicks;
            if (!m_fFadeLastDodgeMarker) {
                  m_lastDodgeMarkerColor.Value.w = 1.0f;
            }
            writeCfg();
        });
    m_localCvars.insert({tempCvar.getCVarName(), tempCvar});
#pragma endregion 
#pragma region dodgeoverlayLastDodgeMarkerFadeTicks
    if((tempCvar = cvarManager->getCvar("dodgeoverlayLastDodgeMarkerFadeTicks")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayLastDodgeMarkerFadeTicks", "200");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_lastDodgeMarkerFadeTicks = std::clamp(now.getIntValue(), 100, 900);
            m_lastDodgeMarkerFadeFactor = 1.0f / m_lastDodgeMarkerFadeTicks;
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
#pragma region dodgeoverlayLastDodgeMarkerColor
    if((tempCvar = cvarManager->getCvar("dodgeoverlayLastDodgeMarkerColor")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayLastDodgeMarkerColor", "(1.0, 1.0, 1.0, 1.0)");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            LinearColor color = now.getColorValue();
            m_lastDodgeMarkerColor = ImColor(color.R, color.G, color.B, color.A);
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
            }

            if (m_fGrabMarkerInputs) {
                m_lastDodgeMarker = m_stickLocation;
                m_fGrabMarkerInputs = false;
            }
        });        
    gameWrapper->HookEvent("Function CarComponent_Dodge_TA.Active.BeginState",
        [this](std::string) {
            if (!m_fShowLastDodgeMarker) {
                return;
            }

            CarWrapper car = gameWrapper->GetLocalCar();
            if (car && car.GetInput().Jumped) {
                m_lastDodgeMarkerColor.Value.w = 1.0f;
                m_fGrabMarkerInputs = true;
                m_fClearDodgeMarker = false;
            }
    });
    gameWrapper->HookEvent("Function CarComponent_DoubleJump_TA.Active.BeginState",
        [this](std::string) {
            if (!m_fShowLastDoubleJumpMarker) {
                return;
            }

            CarWrapper car = gameWrapper->GetLocalCar();
            if (car && car.GetInput().Jumped) {
                m_lastDodgeMarkerColor.Value.w = 1.0f;
                m_fGrabMarkerInputs = true;
                m_fClearDodgeMarker = false;
            }
    });
    gameWrapper->HookEvent("Function CarComponent_Jump_TA.Active.BeginState",
        [this](std::string) {
            CarWrapper car = gameWrapper->GetLocalCar();
            if (car && car.GetInput().Jumped) {
                if (m_fClearLastDodgeMarkerAfterJumping && !m_fIsInGameReplay) {
                    m_fClearDodgeMarker = true;
                }
            }
        });
    gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState",
        [this](std::string) {
            m_fIsInGameReplay = true;
        });
    gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.EndState",
        [this](std::string) {
            m_fIsInGameReplay = false;
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
    if (Checkbox("Show outputs nums", &m_fShowNums)) {
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

    if (Checkbox("Mark on overlay where last dodge happened", &m_fShowLastDodgeMarker)) {
        m_localCvars.at("dodgeoverlayShowLastDodgeMarker").setValue(m_fShowLastDodgeMarker);
    }
    SameLine();
    if (Checkbox("Mark double jumps", &m_fShowLastDoubleJumpMarker)) {
        m_localCvars.at("dodgeoverlayShowLastDoubleJumpMarker").setValue(m_fShowLastDoubleJumpMarker);
    }
    if (m_fShowLastDodgeMarker || m_fShowLastDoubleJumpMarker) {
        if (Checkbox("Clear mark after jumping", &m_fClearLastDodgeMarkerAfterJumping)) {
            m_localCvars.at("dodgeoverlayClearLastDodgeMarkerAfterJumping").setValue(m_fClearLastDodgeMarkerAfterJumping);
        }
        if (Combo("Select the shape of the marker", &m_lastDodgeMarkerShapeSelection, m_lastDodgeMarkerShapeChoices, IM_ARRAYSIZE(m_lastDodgeMarkerShapeChoices))) {
            m_localCvars.at("dodgeoverlayLastDodgeMarkerShape").setValue(m_lastDodgeMarkerShapeSelection);
        }
        if (DragInt("Last dodge marker scale", &m_lastDodgeMarkerScale, 1, 1, 10, "%d")) {
            m_localCvars.at("dodgeoverlayLastDodgeMarkerScale").setValue(m_lastDodgeMarkerScale);
        }
        if(DragFloat("Last dodge marker thickness", &m_lastDodgeMarkerThickness, 0.1f, 0.1f, 10.0f, "%.1f")) {
            m_localCvars.at("dodgeoverlayLastDodgeMarkerThickness").setValue(m_lastDodgeMarkerThickness);
        }
    }
    if (Checkbox("Fade the last dodge marker", &m_fFadeLastDodgeMarker)) {
        m_localCvars.at("dodgeoverlayFadeLastDodgeMarker").setValue(m_fFadeLastDodgeMarker);
    }
    if (m_fFadeLastDodgeMarker) {
        if (DragInt("Amount of time before the dodge marker completely fades", &m_lastDodgeMarkerFadeTicks, 1, 100, 900, "%d")) {
            m_localCvars.at("dodgeoverlayLastDodgeMarkerFadeTicks").setValue(m_lastDodgeMarkerFadeTicks);
        }
    }
    {
        float* colors[4] = { &m_lastDodgeMarkerColor.Value.x, &m_lastDodgeMarkerColor.Value.y, &m_lastDodgeMarkerColor.Value.z, &m_lastDodgeMarkerColor.Value.w };
        if (ColorEdit4("Last dodge marker color", *colors, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
            LinearColor color = LinearColor();
            color.R = m_lastDodgeMarkerColor.Value.x;
            color.G = m_lastDodgeMarkerColor.Value.y;
            color.B = m_lastDodgeMarkerColor.Value.z;
            color.A = m_lastDodgeMarkerColor.Value.w;
            m_localCvars.at("dodgeoverlayLastDodgeMarkerColor").setValue(color);
        };
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

        if ((m_fShowLastDodgeMarker || m_fShowLastDoubleJumpMarker) && !m_fClearDodgeMarker) {
              if (m_fFadeLastDodgeMarker) {
                    m_lastDodgeMarkerColor.Value.w -= m_lastDodgeMarkerFadeFactor;
                    if (m_lastDodgeMarkerColor.Value.w - 0.f <= 10e-6) {
                          m_fClearDodgeMarker = true;
                    }
              }

              switch(m_lastDodgeMarkerShapeSelection) {
                    case LASTDODGEMARKERSHAPE::CROSS:
                        // X shape
                        drawList->AddLine(stickCenter + ImVec2(m_lastDodgeMarker.x * m_radius - m_lastDodgeMarkerScale, -m_lastDodgeMarker.y * m_radius - m_lastDodgeMarkerScale), stickCenter + ImVec2(m_lastDodgeMarker.x * m_radius + m_lastDodgeMarkerScale, -m_lastDodgeMarker.y * m_radius + m_lastDodgeMarkerScale), m_lastDodgeMarkerColor, m_lastDodgeMarkerThickness * m_finalScale);
                        drawList->AddLine(stickCenter + ImVec2(m_lastDodgeMarker.x * m_radius - m_lastDodgeMarkerScale, -m_lastDodgeMarker.y * m_radius + m_lastDodgeMarkerScale), stickCenter + ImVec2(m_lastDodgeMarker.x * m_radius + m_lastDodgeMarkerScale, -m_lastDodgeMarker.y * m_radius - m_lastDodgeMarkerScale), m_lastDodgeMarkerColor, m_lastDodgeMarkerThickness * m_finalScale);
                        break;

                    case LASTDODGEMARKERSHAPE::CIRCLE:
                        drawList->AddCircle(stickCenter + ImVec2(m_lastDodgeMarker.x * m_radius, -m_lastDodgeMarker.y * m_radius), m_lastDodgeMarkerScale * 1.0f, m_lastDodgeMarkerColor, 12, m_lastDodgeMarkerThickness * m_finalScale);
                        break;

                    case LASTDODGEMARKERSHAPE::SQUARE:
                        drawList->AddRect(stickCenter + ImVec2(m_lastDodgeMarker.x * m_radius - m_lastDodgeMarkerScale, -m_lastDodgeMarker.y * m_radius - m_lastDodgeMarkerScale), stickCenter + ImVec2(m_lastDodgeMarker.x * m_radius + m_lastDodgeMarkerScale, -m_lastDodgeMarker.y * m_radius + m_lastDodgeMarkerScale), m_lastDodgeMarkerColor, 0.0f, 15, m_lastDodgeMarkerThickness * m_finalScale);
                        break;

                    case LASTDODGEMARKERSHAPE::FILLEDCIRCLE:
                        drawList->AddCircleFilled(stickCenter + ImVec2(m_lastDodgeMarker.x * m_radius, -m_lastDodgeMarker.y * m_radius), m_lastDodgeMarkerScale * 1.0f, m_lastDodgeMarkerColor);
                        break;

                    case LASTDODGEMARKERSHAPE::FILLEDSQUARE:
                        drawList->AddRectFilled(stickCenter + ImVec2(m_lastDodgeMarker.x * m_radius - m_lastDodgeMarkerScale, -m_lastDodgeMarker.y * m_radius - m_lastDodgeMarkerScale), stickCenter + ImVec2(m_lastDodgeMarker.x * m_radius + m_lastDodgeMarkerScale, -m_lastDodgeMarker.y * m_radius + m_lastDodgeMarkerScale), m_lastDodgeMarkerColor, 0.0f, 15);
                        break;
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
