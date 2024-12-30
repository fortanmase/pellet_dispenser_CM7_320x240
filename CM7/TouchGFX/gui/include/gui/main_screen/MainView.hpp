#ifndef MAINVIEW_HPP
#define MAINVIEW_HPP

#include <gui_generated/main_screen/MainViewBase.hpp>
#include <gui/main_screen/MainPresenter.hpp>

#include <map>

class MainView : public MainViewBase
{
public:
    MainView();
    virtual ~MainView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

private:
    struct TextureScaleData
    {
        float textureScale = 1.0F;
        float textureScaleFactor = -0.005F;
    };
    std::map<TextureMapper *, TextureScaleData> textureScales;
    std::map<TextureMapper *, float> textureZAngle;
    static constexpr float M_PI = 3.14159265358979323846F;
    static constexpr float FLOAT_EPSILON = 0.005F;
    static constexpr float MAX_SCALE = 1.0F;
    static constexpr float MIN_SCALE = 0.8F;

    bool transitionTrigger = false;

protected:
    void updateDisplayState();
    void updateStartUpScreen();
    void updateMainScreen();
    void updateSettingsScreen();
    void updateDisplaySettingsScreen();
    void updateSoundSettingsScreen();
    void updateDispenserSettingsScreen();
    void updateInfoScreen();
    void containerVisibilityOff();
    void updateTextureScale(TextureMapper &texture);
    void updateTextureZAngle(TextureMapper &texture, float increment);
    void updateAlertSlideMenu();
    virtual void settingsButtonPressed();
    virtual void infoButtonPressed();
    virtual void backButtonPressed();
    virtual void homeButtonPressed();
    virtual void displaySettingsButtonPressed();
    virtual void soundSettingsButtonPressed();
    virtual void dispenserSettingsButtonPressed();
    virtual void autoDispenseButtonPressed();
    virtual void manualDispenseButtonPressed();
    virtual void decScreenTimeout();
    virtual void incScreenTimeout();
    virtual void toggleDarkTheme();
    virtual void incPelletQuantity();
    virtual void decPelletQuantity();
    virtual void incPelletInterval();
    virtual void decPelletInterval();
    virtual void incPelletSpeed();
    virtual void decPelletSpeed();
    virtual void alertSlideMenuTriggered();
};

#endif // MAINVIEW_HPP
