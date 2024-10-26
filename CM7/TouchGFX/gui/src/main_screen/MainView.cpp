#include <gui/main_screen/MainView.hpp>
#include <touchgfx/Color.hpp>

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

void MainView::handleTickEvent()
{
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
