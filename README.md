# FTXUI_ONDRA_FRONT
ftxui Front-end pro https://ondra.rapspace.com chat
## Preview Registrace
![Alt text](images/register.jpg)
## Preview Loginu
![Alt text](images/login.jpg)

## Instalace

### Závislosti:
- [ftxui](https://github.com/ArthurSonzogni/FTXUI)
- Cmake
- GCC
- glibc
- [NLOHMANN - JSON](https://github.com/nlohmann/json)
- OpenSSL
- Curl
- Git
### Arch-based
Přes pacman
```
sudo pacman -S glibc curl openssl gcc cmake  nlohmann-json git
```

!Abyste to mohli používat musíme mít nainstalované ftxui knihovny!

Přes Aur
- Paru
```
paru -S ftxui
```

- Yay
```
yay -S ftxui
```

### Po instalaci závislostí
```
git clone https://github.com/Banderascz/FXTUI_ONDRA_FRONT.git
cd FXTUI_ONDRA_FRONT/
mkdir -p ./build
cd ./build
cmake ..
make -j$(nproc)
```

### Spuštění aplikace
```
chmod +x ./bin/app
./bin/app
```
