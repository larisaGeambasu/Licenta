#ifndef OV2640_H_
#define OV2640_H_

#include <stdio.h>
#include <Arduino.h>
#include <pgmspace.h>
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_camera.h"

//Declararea variabilelor externe pentru configuraiile camerei
extern camera_config_t esp32cam_config, esp32cam_aithinker_config;

class OV2640
{
public:
    //Constructorul
    OV2640(){
        fb = NULL; 
    };
    //Deconstructorul
    ~OV2640(){};

    esp_err_t init(camera_config_t config);
    void run(void); // captureaza o imagine folosind camera
    size_t getSize(void); 
    uint8_t *getfb(void);
    int getWidth(void);
    int getHeight(void);
    framesize_t getFrameSize(void);// returneaza dimensiunea cadrului imaginii capturate cum ar fi QVGA
    pixformat_t getPixelFormat(void);//returneaza formatul pixelilor cum ar fi JPEG

    void setFrameSize(framesize_t size); //seteaza dimensiunea cadrului pentru imaginea care va fi capturat
    void setPixelFormat(pixformat_t format);  //seteaza formatul pixelilor 

private:
    void capturareCadru(); //daca nu avem deja unul , aceasta functie va captura un cadru

    camera_config_t _cam_config; //stocheaza configuratia camerei
    camera_fb_t *fb;//stocheaza datele imaginii capturate de la camera
};

#endif 
