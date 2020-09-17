# Tesla Overlay JSON selection for SM3DAS GCM Injection
A simple Tesla overlay to change the default config JSON (by copying yours) in Super Mario 3D All-Stars that define wich game to boot. You could also erase the custom JSON to boot into the stock "stardust" Super Mario Sunshine ROM.

Make sur to create a Directory  ```/JSON_ROM``` (at the root of your SD) with your JSON's config file in. You just need to open ***Tesla > ROM Injection*** and select the JSON of the game you would launch. It will copy the content of the selected JSON into ```atmosphere/contents/010049900F546002/romfs/rom.json```, doesn't matter if it is already create.

Furthermore, to delete the custom JSON copy, just select the ```Delete``` option. This would erase ```atmosphere/contents/010049900F546002/romfs/rom.json```.

Tell me if you have bugs !
## Prerequisites

- [devKitPro for switch](https://devkitpro.org/) (only if you want to compile it yourself)

***Note that the project will not compile if you are missing the librarie.***

## How to use

### Compilation

Go to ```/rom_injection``` and compile the program with ```make```.

### Utilisation

Transfer ```/rom_injection/rom_injection.ovl```to your switch SD card at ```/switch/.overlays```. (The file already exist if you won't compile it...)

## Screen

![*1](https://github.com/3t13nn3/Tesla-Overlay-JSON-selection-for-SM3DAS-GCM-Injection/blob/master/screen/overlay.jpg)
