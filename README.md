# Tux Fishing

[![Screenshot of the Tux Fishing game](https://raw.githubusercontent.com/mrbid/TuxFishing/main/screenshot.png)](https://www.youtube.com/watch?v=HMdYRCAhrAs)

**The first FOSS Linux Fishing game in 3D!**

- **Play Online:** https://pushergames.itch.io/tuxfishing
- **Flathub:** https://flathub.org/apps/com.voxdsp.TuxFishing
- **Snapcraft:** https://snapcraft.io/tuxfishing
- **Windows:** https://github.com/mrbid/TuxFishing/releases

## build
```
sudo apt install --assume-yes libglfw3-dev libglfw3
sudo apt install --assume-yes tcc
sudo apt install --assume-yes upx-ucl
sudo apt install --assume-yes mingw-w64
make
./release/TuxFishing_linux
```
or
```
make deps
make test
```
You don't need [UPX](https://upx.github.io/) but if you don't have the package you can download the binary from [here](https://github.com/upx/upx/releases).

## attribution
* [Tux](https://sketchfab.com/3d-models/tux-157de95fa4014050a969a8361a83d366) made by [Andy Cuccaro](https://andycuccaro.gumroad.com/)
* [Fishing Rod](https://sketchfab.com/3d-models/fishing-rod-1ffdece4c1054f44b640ef3a189ada09) made by [Shedmon](https://sketchfab.com/shedmon)
* All other assets generated using [LUMA GENIE](https://lumalabs.ai/genie).
