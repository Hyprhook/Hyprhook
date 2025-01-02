SOURCE_FILES=$(wildcard hyprhook/src/*.cpp hyprhook/src/*.hpp)

all:
	$(CXX) -shared -fPIC --no-gnu-unique $(SOURCE_FILES) -o hyprhook.so -g `pkg-config --cflags pixman-1 libdrm hyprland pangocairo libinput libudev wayland-server xkbcommon` -std=c++2b -O2
	strip hyprhook.so

debug:
	$(CXX) -shared -fPIC --no-gnu-unique $(SOURCE_FILES) -o hyprhook.so -g `pkg-config --cflags pixman-1 libdrm hyprland pangocairo libinput libudev wayland-server xkbcommon` -std=c++2b -O2

clean:
	rm ./hyprhook.so
