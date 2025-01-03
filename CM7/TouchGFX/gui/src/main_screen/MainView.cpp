#include <gui/main_screen/MainView.hpp>
#include <touchgfx/Color.hpp>
#include "main.h"

typedef enum
{
    STARTUP_SCREEN = 1U,
    MAIN_SCREEN,
    SETTINGS_SCREEN,
    DISPLAY_SETTINGS_SCREEN,
    SOUND_SETTINGS_SCREEN,
    DISPENSER_SETTINGS_SCREEN,
    INFO_SCREEN,
} display_state_t;

static bool logoFinished = false; // variable indicating that the logo drawing was finished
static display_state_t screenState = STARTUP_SCREEN;

static uint8_t screenTimeoutValue = 100U;
static uint16_t pelletQuantity = 100U;
static uint8_t pelletInterval = 5U;
static float pelletSpeed = 1.5F;

static bool dispenseTrigger = false;
static bool autoDispenseTrigger = false;

MainView::MainView()
{
    headerContainer.setVisible(false);
    containerVisibilityOff();
}

void MainView::setupScreen()
{
    MainViewBase::setupScreen();
}

void MainView::tearDownScreen()
{
    MainViewBase::tearDownScreen();
}

static uint32_t lastResetTime = 0;

void MainView::handleTickEvent()
{
    if (dispenseTrigger)
    {
        dispenseTrigger = false;
        set_dispense_trigger();
    }

    if (autoDispenseTrigger)
    {
        uint32_t currentTime = HAL_GetTick();
        if (currentTime - lastResetTime >= 3000U)
        {
            dispenseTrigger = true;
            reset_dispense_trigger();
            lastResetTime = currentTime;
        }
    }

    updateAlertSlideMenu();
    if (screenState == MAIN_SCREEN)
    {
        updateTextureZAngle(dispenseInProgressTexture, 0.100F);
        updateTextureScale(statusReadyTexture);
        updateTextureScale(statusStalledTexture);
        updateTextureScale(statusEmptyTexture);
    }
    updateDisplayState();
}

void MainView::updateDisplayState()
{
    static display_state_t prevDisplayState = STARTUP_SCREEN;
    static Container *containerToInvalidate = NULL;
    static bool stateChanged = false;

    containerVisibilityOff();

    switch (screenState)
    {
    case STARTUP_SCREEN:
        updateStartUpScreen();
        break;
    case MAIN_SCREEN:
        updateMainScreen();
        containerToInvalidate = &mainContainer;
        break;
    case SETTINGS_SCREEN:
        updateSettingsScreen();
        containerToInvalidate = &settingsContainer;
        break;
    case DISPLAY_SETTINGS_SCREEN:
        updateDisplaySettingsScreen();
        containerToInvalidate = &displaySettingsContainer;
        break;
    case SOUND_SETTINGS_SCREEN:
        updateSoundSettingsScreen();
        containerToInvalidate = &displaySettingsContainer;
        break;
    case DISPENSER_SETTINGS_SCREEN:
        updateDispenserSettingsScreen();
        containerToInvalidate = &displaySettingsContainer;
        break;
    case INFO_SCREEN:
        updateInfoScreen();
        containerToInvalidate = &infoContainer;
        break;
    default:
        break;
    }

    if (prevDisplayState != screenState && containerToInvalidate != NULL)
    {
        containerToInvalidate->invalidate();
    }

    prevDisplayState = screenState; /* update the previous state with the current state */

    if (logoFinished && !stateChanged) /* check if logo drawing has finished, stateChanged ensures only one execution */
    {
        screenState = MAIN_SCREEN;
        stateChanged = true;
    }
}

void MainView::updateStartUpScreen()
{
    static uint8_t logoAlphaVal = 0;

    headerContainer.setVisible(false);
    startupContainer.setVisible(true);

    if (logoAlphaVal < 255)
    {
        logoAlphaVal += 3;
        metrisLogo.setAlpha(logoAlphaVal);
        metrisLogo.invalidate();
    }
    else
    {
        logoFinished = true;
    }
}

void MainView::updateMainScreen()
{
    headerContainer.setVisible(true);
    mainContainer.setVisible(true);
}

void MainView::updateSettingsScreen()
{
    functionContainer.setVisible(true);
    settingsContainer.setVisible(true);
}

void MainView::updateDisplaySettingsScreen()
{
    functionContainer.setVisible(true);
    displaySettingsContainer.setVisible(true);
}

void MainView::updateSoundSettingsScreen()
{
    functionContainer.setVisible(true);
    soundSettingsContainer.setVisible(true);
}

void MainView::updateDispenserSettingsScreen()
{
    functionContainer.setVisible(true);
    dispenserSettingsContainer.setVisible(true);
}

void MainView::updateInfoScreen()
{
    functionContainer.setVisible(true);
    infoContainer.setVisible(true);
}
void MainView::settingsButtonPressed()
{
    screenState = SETTINGS_SCREEN;
}

void MainView::infoButtonPressed()
{
    screenState = INFO_SCREEN;
}

void MainView::displaySettingsButtonPressed()
{
    screenState = DISPLAY_SETTINGS_SCREEN;
}

void MainView::soundSettingsButtonPressed()
{
    screenState = SOUND_SETTINGS_SCREEN;
}

void MainView::dispenserSettingsButtonPressed()
{
    screenState = DISPENSER_SETTINGS_SCREEN;
}

void MainView::backButtonPressed()
{
    if (screenState == DISPLAY_SETTINGS_SCREEN || screenState == SOUND_SETTINGS_SCREEN || screenState == DISPENSER_SETTINGS_SCREEN)
    {
        screenState = SETTINGS_SCREEN;
    }
    else
    {
        screenState = MAIN_SCREEN;
    }
}

void MainView::homeButtonPressed()
{
    screenState = MAIN_SCREEN;
}

void MainView::autoDispenseButtonPressed()
{
    autoDispenseTrigger ^= true;
    circleAutoDispenseStatusPainter.setColor(autoDispenseTrigger ? touchgfx::Color::getColorFromRGB(0, 255, 0) : touchgfx::Color::getColorFromRGB(255, 0, 0));
}

void MainView::manualDispenseButtonPressed()
{
    dispenseTrigger = true;
    reset_dispense_trigger();
}

void MainView::decScreenTimeout()
{
    if (screenTimeoutValue > 0)
    {
        screenTimeoutValue--;
        Unicode::snprintf(screenToutValueBuffer, SCREENTOUTVALUE_SIZE, "%d", screenTimeoutValue);
        screenToutValue.setWildcard(screenToutValueBuffer);
        screenToutValue.resizeToCurrentTextWithAlignment();
        screenToutValue.invalidate();
        displaySettingsContainer.invalidate(); // temporary solution, invalidate areas as small as possible
    }
}

void MainView::incScreenTimeout()
{
    if (screenTimeoutValue < 100)
    {
        screenTimeoutValue++;
        Unicode::snprintf(screenToutValueBuffer, SCREENTOUTVALUE_SIZE, "%d", screenTimeoutValue);
        screenToutValue.setWildcard(screenToutValueBuffer);
        screenToutValue.resizeToCurrentTextWithAlignment();
        screenToutValue.invalidate();
        displaySettingsContainer.invalidate(); // temporary solution, invalidate areas as small as possible
    }
}

void MainView::incPelletQuantity()
{
    if (pelletQuantity < 2000)
    {
        pelletQuantity++;
        Unicode::snprintf(pelQuantValueBuffer, PELQUANTVALUE_SIZE, "%d", pelletQuantity);
        pelQuantValue.setWildcard(pelQuantValueBuffer);
        pelQuantValue.resizeToCurrentTextWithAlignment();
        pelQuantValue.invalidate();
        dispenserSettingsContainer.invalidate(); // temporary solution, invalidate areas as small as possible
    }
}

void MainView::decPelletQuantity()
{
    if (pelletQuantity > 1)
    {
        pelletQuantity--;
        Unicode::snprintf(pelQuantValueBuffer, PELQUANTVALUE_SIZE, "%d", pelletQuantity);
        pelQuantValue.setWildcard(pelQuantValueBuffer);
        pelQuantValue.resizeToCurrentTextWithAlignment();
        pelQuantValue.invalidate();
        dispenserSettingsContainer.invalidate(); // temporary solution, invalidate areas as small as possible
    }
}

void MainView::incPelletInterval()
{
    if (pelletInterval < 100)
    {
        pelletInterval++;
        Unicode::snprintf(pelIntervValueBuffer, PELINTERVVALUE_SIZE, "%d", pelletInterval);
        pelIntervValue.setWildcard(pelIntervValueBuffer);
        pelIntervValue.resizeToCurrentTextWithAlignment();
        pelIntervValue.invalidate();
        dispenserSettingsContainer.invalidate(); // temporary solution, invalidate areas as small as possible
    }
}

void MainView::decPelletInterval()
{
    if (pelletInterval > 1)
    {
        pelletInterval--;
        Unicode::snprintf(pelIntervValueBuffer, PELINTERVVALUE_SIZE, "%d", pelletInterval);
        pelIntervValue.setWildcard(pelIntervValueBuffer);
        pelIntervValue.resizeToCurrentTextWithAlignment();
        pelIntervValue.invalidate();
        dispenserSettingsContainer.invalidate(); // temporary solution, invalidate areas as small as possible
    }
}

void MainView::incPelletSpeed()
{
    if (pelletSpeed < 10.0F)
    {
        pelletSpeed += 0.5F;
        Unicode::snprintfFloat(pelSpeedValueBuffer, PELSPEEDVALUE_SIZE, "%.1f", pelletSpeed);
        pelSpeedValue.setWildcard(pelSpeedValueBuffer);
        pelSpeedValue.resizeToCurrentTextWithAlignment();
        pelSpeedValue.invalidate();
        dispenserSettingsContainer.invalidate(); // temporary solution, invalidate areas as small as possible
    }
}

void MainView::decPelletSpeed()
{
    if (pelletSpeed > 0.5F)
    {
        pelletSpeed -= 0.5F;
        Unicode::snprintfFloat(pelSpeedValueBuffer, PELSPEEDVALUE_SIZE, "%.1f", pelletSpeed);
        pelSpeedValue.setWildcard(pelSpeedValueBuffer);
        pelSpeedValue.resizeToCurrentTextWithAlignment();
        pelSpeedValue.invalidate();
        dispenserSettingsContainer.invalidate(); // temporary solution, invalidate areas as small as possible
    }
}

void MainView::toggleDarkTheme()
{
    if (darkThemeButton.getState())
    {
        backgndBox.setColor((touchgfx::Color::getColorFromRGB(18, 18, 18)));
    }
    else
    {
        backgndBox.setColor((touchgfx::Color::getColorFromRGB(245, 245, 245)));
    }
    backgndBox.invalidate();
}

void MainView::containerVisibilityOff()
{
    // headerContainer.setVisible(false);
    startupContainer.setVisible(false);
    mainContainer.setVisible(false);
    functionContainer.setVisible(false);
    settingsContainer.setVisible(false);
    displaySettingsContainer.setVisible(false);
    infoContainer.setVisible(false);
    displaySettingsContainer.setVisible(false);
    soundSettingsContainer.setVisible(false);
    dispenserSettingsContainer.setVisible(false);
}

void MainView::updateTextureScale(TextureMapper &texture)
{
    TextureScaleData &scaleData = textureScales[&texture];

    if (fabs(scaleData.textureScale - MAX_SCALE) < FLOAT_EPSILON)
    {
        scaleData.textureScaleFactor = -0.005F;
    }
    else if (fabs(scaleData.textureScale - MIN_SCALE) < FLOAT_EPSILON)
    {
        scaleData.textureScaleFactor = 0.005F;
    }

    scaleData.textureScale += scaleData.textureScaleFactor;
    texture.updateScale(scaleData.textureScale);
}

void MainView::alertSlideMenuTriggered()
{
    if (alertSlideMenu.getState() == SlideMenu::COLLAPSED)
    {
        alertSlideMenu.animateToState(SlideMenu::EXPANDED);
        transitionTrigger = true;
    }
    else
    {
        alertSlideMenu.animateToState(SlideMenu::COLLAPSED);
        transitionTrigger = false;
    }
}

void MainView::updateAlertSlideMenu()
{
    if (alertSlideMenu.getState() == SlideMenu::EXPANDED)
    {
        if (alertSlideMenu.getExpandedStateTimer() == alertSlideMenu.getExpandedStateTimeout())
        {
            transitionTrigger = false; /* Reset transition trigger after timeout */
        }
    }

    static float updateAngle = 0.0F;
    const float incAngle = 0.1309F;

    if (transitionTrigger && updateAngle < 1.5708F)
    {
        updateAngle += incAngle;
        if (updateAngle > 1.5708F)
        {
            updateAngle = 1.5708F;
        }
        textureAlertsButton.updateAngles(0.0F, 0.0F, updateAngle);
    }
    else if (!transitionTrigger && updateAngle > 0.0F)
    {
        updateAngle -= incAngle;
        if (updateAngle < 0.0F)
        {
            updateAngle = 0.0F;
        }
        textureAlertsButton.updateAngles(0.0F, 0.0F, updateAngle);
    }
}

void MainView::updateTextureZAngle(TextureMapper &texture, float increment)
{
    float &zAngle = textureZAngle[&texture];
    zAngle += increment;
    if (zAngle >= 2.0F * M_PI)
    {
        zAngle -= 2.0F * M_PI;
    }
    texture.updateAngles(0.0F, 0.0F, zAngle);
}
