# Licenta
Fișiere lucrarea de licență.
Fișierul Automat_apa conține codul pentru funcționarea automatului cu apă.
Acest cod se copiaza și se introduce în Arduino IDE și se selectează placa Arduino Uno de la Boards.
Mai apoi se introduce cablul USB de la Arduino în laptop și se selectează portul corespunzator (exemplu COM5) și se dă run și se așteaptă ca codul să se uploadeze.
La fel se procedează și pentru fișierul Automat_mancare care conține codul pentru funcționarea automatului cu mancare.
Pentru funcționarea camerei avem următoarele:
OV2640.cpp si headerul OV2640.h , folosite pentru a defini configurația camerei OV2640 , pentru a declara pinii modului și pentru a face configurațiile necesare pentru imaginile capturate.
Aceste două fișiere se vor introduce într-un folder numit sursa_camera.
Codul de la camera ESP32_CAM va fi introdus într-o noua pagina din Arduino IDE , se va salva cu numele de ESP32_CAM și automat va fi creat un folder pentru acesta ,cu același nume.
Mai apoi tot în Arduino IDE , vom introduce conținutul fișierului pini_camera.h și vom numi și fisierul la fel (pini_camera.h).
Iar mai apoi fișierul pini_camera.h , împreuna cu folderul sursa_camera vor fi introduse în folderul creat de la ESP32_CAM unde se află și fișierul principal ESP32_CAM.
Atunci când deschidem fișierul ESP32_CAM vom vedea că se deschide automat în Arduino IDE alături de el și fisierul pini_camera.h.
Pentru acest cod trebuie selectată placa AI THINKER ESP32-CAM.

