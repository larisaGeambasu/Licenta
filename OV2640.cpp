#include "OV2640.h"

#define LOG "OV2640" //definirea unei constante necesare pentru logging

//definirea pinilor pentru modulul ESP32-CAM si pentru ESP32-CAM AI THINKER
//aceste date se pot gasi in datasheetul fiecarui modul si sunt fixe pentru fiecare modul
camera_config_t esp32cam_config{

    .pin_pwdn = -1, 
    .pin_reset = 15,
    .pin_xclk = 27,
    .pin_sscb_sda = 25,
    .pin_sscb_scl = 23,
    .pin_d7 = 19,
    .pin_d6 = 36,
    .pin_d5 = 18,
    .pin_d4 = 39,
    .pin_d3 = 5,
    .pin_d2 = 34,
    .pin_d1 = 35,
    .pin_d0 = 17,
    .pin_vsync = 22,
    .pin_href = 26,
    .pin_pclk = 21,
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_JPEG, // formatul principal de imagine este JPEG
    .frame_size = FRAMESIZE_SVGA,
    .jpeg_quality = 12, 
    .fb_count = 2       
};

camera_config_t esp32cam_aithinker_config{

    .pin_pwdn = 32,
    .pin_reset = -1,
    .pin_xclk = 0,
    .pin_sscb_sda = 26,
    .pin_sscb_scl = 27,
    .pin_d7 = 35,
    .pin_d6 = 34,
    .pin_d5 = 39,
    .pin_d4 = 36,
    .pin_d3 = 21,
    .pin_d2 = 19,
    .pin_d1 = 18,
    .pin_d0 = 5,
    .pin_vsync = 25,
    .pin_href = 23,
    .pin_pclk = 22,
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_1,
    .ledc_channel = LEDC_CHANNEL_1,
    .pixel_format = PIXFORMAT_JPEG, //formatul principal de imagine este JPEG
    .frame_size = FRAMESIZE_SVGA, 
    .jpeg_quality = 12, 
    .fb_count = 2  
};

//Functia folosita pentru a capta o noua imagine folosind camera
void OV2640::run(void)
{
    if(fb) //se verifica daca exista deja un frame buffer 
    esp_camera_fb_return(fb); // si se returneaza pentru reutilizare
    fb = esp_camera_fb_get(); //apoi se obtine altul nou de la camera
}

//Verifica daca exista un frame buffer , daca nu atunci apeleaza la functia run 
void OV2640::capturareCadru(void)
{
    if (!fb)
        run();
}
//returneaza latimea imaginii capturate
int OV2640::getWidth(void)
{
    capturareCadru(); //se apleaza aceasta functie pentru a se asigura ca exista un cadru disponibil
    return fb->width;
}
//returneaza inaltimea imaginii capturate 
int OV2640::getHeight(void)
{
    capturareCadru();
    return fb->height;
}
//returneaza dimensiunea imgainii in octeti
size_t OV2640::getSize(void)
{ 
    capturareCadru();
    if (!fb)
        return 0;
    return fb->len;
}
//returneaza un pointer la bufferul de cadre al imaginii 
uint8_t *OV2640::getfb(void)
{
    capturareCadru();
    if (!fb)
        return NULL; 
    return fb->buf;
}
//returneaza dimensiune cadrului 
framesize_t OV2640::getFrameSize(void)
{
    return _cam_config.frame_size;
}
//seteaza dimensiunea cadrului
void OV2640::setFrameSize(framesize_t size)
{
    _cam_config.frame_size = size;
}
//returneaza formatul pixelilor
pixformat_t OV2640::getPixelFormat(void)
{
    return _cam_config.pixel_format;
}
//seteaza formatul pixelilor
void OV2640::setPixelFormat(pixformat_t format)
{
    switch (format)
    {
    case PIXFORMAT_RGB565:
    case PIXFORMAT_YUV422:
    case PIXFORMAT_GRAYSCALE:
    case PIXFORMAT_JPEG:
        _cam_config.pixel_format = format;
        break;
    default:
        _cam_config.pixel_format = PIXFORMAT_GRAYSCALE;
        break;
    }
}

esp_err_t OV2640::init(camera_config_t config)
{
    memset(&_cam_config, 0, sizeof(_cam_config));//seteaza toti octetii din _cam_config la 0 astfel previne orice date reziduale din celelalte utilizari care au mai fost
    memcpy(&_cam_config, &config, sizeof(config));//copiaza datele din config in _cam_config astfel instanta clasei OV2640 o sa foloseasca configuratia specifica

    esp_err_t err = esp_camera_init(&_cam_config); //stocheaza rezultatul initializarii
    //esp_err_t este o functie din biblioteca ESP-IDF  fiind folosita pentru a prezenta coduri de eroare
    //daca rezultatul de la aceasta functie este ESP_OK inseamna ca s-a initializat cu succes ,in caz contrar orice alta valoare inseamna ca a dat eroare
    if (err != ESP_OK)
    {
        printf("Proba camerei a esuat cu eroarea 0x%x", err);
        return err;
    }

    return ESP_OK;
}
