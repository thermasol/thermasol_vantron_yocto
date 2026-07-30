#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

enum class LightMode {
    VIOLET = 0,
    INDIGO,
    BLUE,
    GREEN,
    YELLOW,
    ORANGE,
    RED,
    MOOD,
    WHITE,
    CHROMA,
    COLOR
};


class LightHelper
{
public:
    LightHelper();
    ~LightHelper();
    void turnlightOn();
    void turnlightOff();
    bool getLightOn();
    void setIntensity(int);
    int getIntensity();
    void setLightMode(LightMode);
    LightMode getLightMode();
    void setRGB(int rgb);
    void setRGB(int red, int green, int blue);
    int getRGB();
private:
    bool lightOn;
    int intensity;
    LightMode lightMode;
    int RGB;

};


#endif //LIGHTHELPER_H
