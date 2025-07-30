#ifndef DODGEOVERLAY_H
#define DODGEOVERLAY_H

// #define IMGUI_DEFINE_MATH_OPERATORS

namespace DodgeOverlay {
    class DodgeOverlayPlugin : 
        public BakkesMod::Plugin::BakkesModPlugin,
        public BakkesMod::Plugin::PluginSettingsWindow,
        public BakkesMod::Plugin::PluginWindow 
    {
    private:
        bool m_fWindowOpen = false;
        ImVec2 m_windowPosition = ImVec2();
        ImVec2 m_windowSize = ImVec2();
        ImVec2 m_displaySize = ImVec2(100.0f, 100.0f);
        float m_scale = 1.0f;
        float m_finalScale = 1.0f;

        float m_radius = 1.0f;
        int m_circleSegments = 4;
        bool m_fShowNums = true;
        ImColor m_stickBorderColor = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
        ImColor m_stickLocationColor = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
        ImColor m_dodgeDeadzoneColor = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec2 m_stickLocation = ImVec2();
        float m_stickLocationSize = 5.0f;
        float m_dodgeDeadzone = 0.0f;
        float m_dodgeDeadzoneRoll = 0.0f;
        bool m_fShowDodgeDeadzoneBorder = true;
        float m_dodgeDeadzoneBorderThickness = 1.0f;
        float m_dodgeDeadzoneCrossedAlpha = 0.1f;
        bool m_fShowFlipCancelMeter = true;
        bool m_fFlipFlipCancelMeter = false;
        int m_flipCancelMeterPosition = 0;
        enum FLIPCANCELMETERPOSITION { LEFT, TOP, RIGHT, INLAID };
        const char* m_flipCancelMeterPositionOptions[4] = {"Left", "Top", "Right", "Inlaid"};
        int m_flipCancelMeterWidth = 10;
        int m_flipCancelMeterDistanceAway = 10;
        ImColor m_flipCancelMeterAngMomPosSide = ImColor(0.0f, 1.0f, 0.0f, 0.5f);
        ImColor m_flipCancelMeterAngMomNegSide = ImColor(1.0f, 0.0f, 0.0f, 0.5f);
        ImColor m_flipCancelMeterTimerColor = ImColor(1.0f, 0.0f, 1.0f, 0.5f);
        bool m_fShowFlipCancelMeterTimer = true;
        bool m_fStartFlipCancelMeterTimer = false;
        bool m_amDodging = false;
        bool m_isDodgePositive = false;
        float m_timeDodged;
        float m_theTime;
        float MIN_DODGE_TORQUE_TIME = 0.041f, DODGE_TORQUE_TIME = 0.65f; // defaults

        std::filesystem::path m_configurationFilePath = "cfg/dodgeoverlay.cfg";

        std::unordered_map<std::string, CVarWrapper> m_localCvars;

    public:
        virtual void onLoad();
        virtual void RenderSettings() override;
        virtual std::string GetPluginName() override;
        virtual void SetImGuiContext(uintptr_t ctx) override;
        virtual void Render() override;
        void RenderImGui();
        virtual std::string GetMenuName() override;
        virtual std::string GetMenuTitle() override;
        virtual bool ShouldBlockInput() override;
        virtual bool IsActiveOverlay() override;
        virtual void OnOpen() override;
        virtual void OnClose() override;
        void writeCfg();
        virtual void onUnload();
    };
}

#endif // DODGEOVERLAY_H