#ifndef MAINVIEW_HPP
#define MAINVIEW_HPP

#include <gui_generated/main_screen/MainViewBase.hpp>
#include <gui/main_screen/MainPresenter.hpp>

class MainView : public MainViewBase
{
public:
    MainView();
    virtual ~MainView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

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
    virtual void settingsButtonPressed();
    virtual void infoButtonPressed();
    virtual void backButtonPressed();
    virtual void homeButtonPressed();
    virtual void displaySettingsButtonPressed();
    virtual void soundSettingsButtonPressed();
    virtual void dispenserSettingsButtonPressed();
    virtual void decScreenTimeout();
    virtual void incScreenTimeout();
    virtual void toggleDarkTheme();
    virtual void incPelletQuantity();
    virtual void decPelletQuantity();
    virtual void incPelletInterval();
    virtual void decPelletInterval();
    virtual void incPelletSpeed();
    virtual void decPelletSpeed();
};

#endif // MAINVIEW_HPP
