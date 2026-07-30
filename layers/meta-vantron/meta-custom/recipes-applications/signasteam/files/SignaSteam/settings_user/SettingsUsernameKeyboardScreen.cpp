#include "SettingsUsernameKeyboardScreen.h"
#include "ui_SettingsUsernameKeyboardScreen-landscape.h"
#include "ui_SettingsUsernameKeyboardScreen-portrait.h"

#include "settings/SettingsScreenModel.h"

#include <QMessageBox>
#include <QToolButton>
#include <QSettings>

#include <QDebug>

SettingsUsernameKeyboardScreen::SettingsUsernameKeyboardScreen(QWidget *parent, SettingsScreenModel *settingsScreenModel)
    : QWidget(parent)
{
    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsUsernameKeyboardScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsUsernameKeyboardScreenPortrait;
        uiPortrait->setupUi(this);
    }

    usernameValueLabel = findChild<QLabel*>("usernameValueLabel");
    usernameValueLabel->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QButtonGroup *keyBtnGroup = findChild<QButtonGroup*>("keyBtnGroup");
    connect(keyBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &SettingsUsernameKeyboardScreen::onKeyBtnClicked);

    QToolButton *aKeyBtn = findChild<QToolButton*>("aKeyBtn");
    keyBtnGroup->setId(aKeyBtn, 65);
    aKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *bKeyBtn = findChild<QToolButton*>("bKeyBtn");
    keyBtnGroup->setId(bKeyBtn, 66);
    bKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *cKeyBtn = findChild<QToolButton*>("cKeyBtn");
    keyBtnGroup->setId(cKeyBtn, 67);
    cKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *dKeyBtn = findChild<QToolButton*>("dKeyBtn");
    keyBtnGroup->setId(dKeyBtn, 68);
    dKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *eKeyBtn = findChild<QToolButton*>("eKeyBtn");
    keyBtnGroup->setId(eKeyBtn, 69);
    eKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *fKeyBtn = findChild<QToolButton*>("fKeyBtn");
    keyBtnGroup->setId(fKeyBtn, 70);
    fKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *gKeyBtn = findChild<QToolButton*>("gKeyBtn");
    keyBtnGroup->setId(gKeyBtn, 71);
    gKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *hKeyBtn = findChild<QToolButton*>("hKeyBtn");
    keyBtnGroup->setId(hKeyBtn, 72);
    hKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *iKeyBtn = findChild<QToolButton*>("iKeyBtn");
    keyBtnGroup->setId(iKeyBtn, 73);
    iKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *jKeyBtn = findChild<QToolButton*>("jKeyBtn");
    keyBtnGroup->setId(jKeyBtn, 74);
    jKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *kKeyBtn = findChild<QToolButton*>("kKeyBtn");
    keyBtnGroup->setId(kKeyBtn, 75);
    kKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *lKeyBtn = findChild<QToolButton*>("lKeyBtn");
    keyBtnGroup->setId(lKeyBtn, 76);
    lKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *mKeyBtn = findChild<QToolButton*>("mKeyBtn");
    keyBtnGroup->setId(mKeyBtn, 77);
    mKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *nKeyBtn = findChild<QToolButton*>("nKeyBtn");
    keyBtnGroup->setId(nKeyBtn, 78);
    nKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *oKeyBtn = findChild<QToolButton*>("oKeyBtn");
    keyBtnGroup->setId(oKeyBtn, 79);
    oKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *pKeyBtn = findChild<QToolButton*>("pKeyBtn");
    keyBtnGroup->setId(pKeyBtn, 80);
    pKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *qKeyBtn = findChild<QToolButton*>("qKeyBtn");
    keyBtnGroup->setId(qKeyBtn, 81);
    qKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *rKeyBtn = findChild<QToolButton*>("rKeyBtn");
    keyBtnGroup->setId(rKeyBtn, 82);
    rKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *sKeyBtn = findChild<QToolButton*>("sKeyBtn");
    keyBtnGroup->setId(sKeyBtn, 83);
    sKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *tKeyBtn = findChild<QToolButton*>("tKeyBtn");
    keyBtnGroup->setId(tKeyBtn, 84);
    tKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *uKeyBtn = findChild<QToolButton*>("uKeyBtn");
    keyBtnGroup->setId(uKeyBtn, 85);
    uKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *vKeyBtn = findChild<QToolButton*>("vKeyBtn");
    keyBtnGroup->setId(vKeyBtn, 86);
    vKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *wKeyBtn = findChild<QToolButton*>("wKeyBtn");
    keyBtnGroup->setId(wKeyBtn, 87);
    wKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *xKeyBtn = findChild<QToolButton*>("xKeyBtn");
    keyBtnGroup->setId(xKeyBtn, 88);
    xKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *yKeyBtn = findChild<QToolButton*>("yKeyBtn");
    keyBtnGroup->setId(yKeyBtn, 89);
    yKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *zKeyBtn = findChild<QToolButton*>("zKeyBtn");
    keyBtnGroup->setId(zKeyBtn, 90);
    zKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton *spaceKeyBtn = findChild<QToolButton*>("spaceKeyBtn");
    keyBtnGroup->setId(spaceKeyBtn, 32);
    spaceKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(MEDIUM_FONT_SIZE));

    QToolButton *deleteKeyBtn = findChild<QToolButton*>("deleteKeyBtn");
    deleteKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(MEDIUM_FONT_SIZE));
    connect(deleteKeyBtn, &QToolButton::clicked, this, &SettingsUsernameKeyboardScreen::onDeleteKeyBtnPressed);

    QToolButton *okayKeyBtn = findChild<QToolButton*>("okayKeyBtn");
    okayKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(MEDIUM_FONT_SIZE));
    connect(okayKeyBtn, &QToolButton::clicked, this, &SettingsUsernameKeyboardScreen::onOkayKeyBtnPressed);

    QToolButton *cancelKeyBtn = findChild<QToolButton*>("cancelKeyBtn");
    cancelKeyBtn->setFont(fontStyle::getQuicksandBoldWithSize(MEDIUM_FONT_SIZE));
    connect(cancelKeyBtn, &QToolButton::clicked, this, &SettingsUsernameKeyboardScreen::onCancelKeyBtnPressed);

    connect(settingsScreenModel, &SettingsScreenModel::usernameChanged, this, &SettingsUsernameKeyboardScreen::onUsernameChanged);
}

SettingsUsernameKeyboardScreen::~SettingsUsernameKeyboardScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsUsernameKeyboardScreen::onUsernameChanged(QString username)
{
    usernameValueLabel->setText(username);
}

void SettingsUsernameKeyboardScreen::onKeyBtnClicked(int keyPressedAsInt)
{
    QString temp = usernameValueLabel->text();
    if(temp.size() < 8) {
        temp.append(keyPressedAsInt);
    } else {
        showDialog(USERNAME_TOO_LONG_MSG);
    }
    usernameValueLabel->setText(temp);
}

void SettingsUsernameKeyboardScreen::onDeleteKeyBtnPressed()
{
    QString temp = usernameValueLabel->text();
    temp = temp.left(temp.size() - 1);
    usernameValueLabel->setText(temp);
}

void SettingsUsernameKeyboardScreen::onOkayKeyBtnPressed()
{
    if(usernameValueLabel->text().size() > 0) {
        emit usernameEntered(usernameValueLabel->text());
        emit settingsScreenChanged(SettingsScreenAll::USERNAMES);
    } else {
        showDialog(USERNAME_TOO_SHORT_MSG);
    }
}

void SettingsUsernameKeyboardScreen::onCancelKeyBtnPressed()
{
    emit settingsScreenChanged(SettingsScreenAll::USERNAMES);
}

void SettingsUsernameKeyboardScreen::showDialog(QString message)
{
    QToolButton *okayBtn = new QToolButton();
    okayBtn->setStyleSheet(dialogStyle::BUTTON_STYLESHEET);

    QMessageBox dialog;
    dialog.addButton(okayBtn, QMessageBox::ButtonRole::AcceptRole);
    dialog.setStyleSheet(DIALOG_STYLESHEET);
    dialog.setFont(fontStyle::getQuicksandBoldWithSize(MEDIUM_FONT_SIZE));
    dialog.setText(message);
    dialog.exec();
}
