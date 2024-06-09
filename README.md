## Installation

### Manual

To build, have hyprland headers installed and under the repo directory do:

```
make all
```

Then use `hyprctl plugin load` followed by the absolute path to the `.so` file to load, you could add this to your `exec-once` to load the plugin on startup

### Hyprpm

```
hyprpm add https://github.com/Moerliy/Hypr-Which-Key
hyprpm enable Hyprspace
```

### Nix

Refer to the [Hyprland wiki](https://wiki.hyprland.org/Nix/Hyprland-on-Home-Manager/#plugins) on plugins, but your flake might look like this:
