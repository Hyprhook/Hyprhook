<a id="readme-top"></a>

<!-- PROJECT SHIELDS -->

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/Hyprhook/Hyprhook">
    <img src="https://repository-images.githubusercontent.com/812448348/684c24bc-4669-431d-b7bf-2d2e266d4db8" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">Hyprhook</h3>

  <p align="center">
    Hyprhook is a plugin for Hyprland that allows you to run scripts when specific events occur. It acts as a proxy to make event information available outside of C++ code, passing relevant data as JSON parameters to your scripts.
    <br />
    <a href="https://github.com/Hyprhook/Hyprhook/blob/master/README.md"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/Hyprhook/Hyprhook">View Demo</a>
    ·
    <a href="https://github.com/Hyprhook/Hyprhook/issues/new?labels=bug&template=bug-report---.md">Report Bug</a>
    ·
    <a href="https://github.com/Hyprhook/Hyprhook/issues/new?labels=enhancement&template=feature-request---.md">Request Feature</a>
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#installation">Installation</a>
      <ul>
        <li><a href="#manual">Manual</a></li>
        <li><a href="#hyprpm">Hyprpm</a></li>
        <li><a href="#nix">Nix</a></li>
      </ul>
    </li>
    <li><a href="#configuration">Configuration</a></li>
    <li><a href="#json-parameters">JSON Parameters</a></li>
    <li><a href="#examples">Examples</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>

## Installation

### Manual

To build the project, ensure that the Hyprland headers are installed on your system. Then, navigate to the repository directory and run:

```fish
make all
```

Then use `hyprctl plugin load` followed by the absolute path to the `.so` file to load, you could add this to your `exec-once` to load the plugin on startup

### Hyprpm

```fish
hyprpm add https://github.com/Hyprhook/Hyprhook
hyprpm enable Hyprhook
```

### Nix

Refer to the [Hyprland wiki](https://wiki.hyprland.org/Nix/Hyprland-on-Home-Manager/#plugins) on plugins, but your flake might look like this:

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Configuration

To configure Hyprhook, specify the script directories for each event in your Hyprland configuration file. When an event occurs, the corresponding script will be executed with the event data passed as a JSON parameter (if applicable). The available keywords are the names of the events as listed in <a href="#json-parameters">JSON Parameters</a>.

> [!IMPORTANT]
> Some events have a different name in the hyprhook config:
>
> | Event     | Hyprhook config name |
> | --------- | -------------------- |
> | submap    | onSubmap             |
> | workspace | onWorkspace          |

Example configuration snippet:

```ini
plugin {
  hyprhook {
    activeWindow = /path/to/your/script.sh
    openWindow = /path/to/another/script.sh # Add more event-script mappings as needed
    onWorkspace = /path/to/your/workspace/script.sh # Use the right config name
  }
}
```

> [!NOTE]
> Ensure your scripts are executable and include the appropriate shebang (e.g., `#!/bin/bash` for Bash scripts). If your scripts do not execute as expected, verify that they have execute permissions and that the shebang matches the script's interpreter.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## JSON Parameters

The plugin uses the following functions to generate JSON parameters for each event:

- `Submap Data`: Returns a json with the attribut submap for the submap name changed to.
- `Window Data`: Returns JSON for a window like with `hyprctl activeWindow -j`.
- `Workspace Data`: Returns JSON for a workspace like with `hyprctl activeWorkspace -j`.
- `Monitor Data`: Returns JSON for a monitor like with `hyprctl monitors -j` but for one monitor only.
- `Vector2D Data`: Returns JSON for a `Vector2D` object with x and y values.
- `SButton Data`: Returns JSON for a button event (`IPointer::SButtonEvent`).
- `SDown Data`: Returns JSON for a touch down event (`ITouch::SDownEvent`).
- `SUp Data`: Returns JSON for a touch up event (`ITouch::SUpEvent`).

> [!NOTE]
> If you want to know how the json looks like you can always set the script to be run to `echo` and see the output.

### List of Supported Events

| Event                | Description                                                                                                         | Parameter (JSON)                     |
| -------------------- | ------------------------------------------------------------------------------------------------------------------- | ------------------------------------ |
| `activeWindow`       | Fired on active window change                                                                                       | Window data                          |
| `keyboardFocus`      | Fired on keyboard focus change. Contains the newly focused surface                                                  | None                                 |
| `moveWorkspace`      | Fired when a workspace changes its monitor                                                                          | Vector of Workspace and Monitor data |
| `focusedMon`         | Fired on monitor focus change                                                                                       | Monitor data                         |
| `moveWindow`         | Fired when a window changes workspace                                                                               | Vector of Window and Workspace data  |
| `openLayer`          | Fired when a layer surface is mapped                                                                                | None                                 |
| `closeLayer`         | Fired when a layer surface is unmapped                                                                              | None                                 |
| `openWindow`         | Fired when a window is mapped                                                                                       | Window data                          |
| `closeWindow`        | Fired when a window is unmapped                                                                                     | Window data                          |
| `windowUpdateRules`  | Fired when a window’s rules are updated                                                                             | Window data                          |
| `urgent`             | Fired when a window requests urgent status                                                                          | Window data                          |
| `minimize`           | Fired when a window requests a minimize status change                                                               | None                                 |
| `monitorAdded`       | Fired when a monitor is plugged in                                                                                  | Monitor data                         |
| `monitorRemoved`     | Fired when a monitor is unplugged                                                                                   | Monitor data                         |
| `createWorkspace`    | Fired when a workspace is created                                                                                   | Workspace data                       |
| `destroyWorkspace`   | Fired when a workspace is destroyed                                                                                 | Workspace data                       |
| `fullscreen`         | Fired when a window changes fullscreen state                                                                        | Window data                          |
| `changeFloatingMode` | Fired when a window changes float state                                                                             | Window data                          |
| `workspace`          | Fired on a workspace change (explicitly requested by a user)                                                        | Workspace data                       |
| `submap`             | Fired on a submap change                                                                                            | Submap data                          |
| `mouseMove`          | Fired when the cursor moves. Param is coordinates                                                                   | Vector2D data                        |
| `mouseButton`        | Fired on a mouse button press                                                                                       | Button event data                    |
| `mouseAxis`          | Fired on a mouse axis event                                                                                         | None                                 |
| `touchDown`          | Fired on a touch down event                                                                                         | Touch down event data                |
| `touchUp`            | Fired on a touch up event                                                                                           | Touch up event data                  |
| `touchMove`          | Fired on a touch motion event                                                                                       | None                                 |
| `activeLayout`       | Fired on a keyboard layout change. String pointer temporary, not guaranteed after execution of the handler finishes | None                                 |
| `preRender`          | Fired before a frame for a monitor is about to be rendered                                                          | Monitor data                         |
| `screencast`         | Fired when the screencopy state of a client changes. Keep in mind there might be multiple separate clients          | None                                 |
| `render`             | Fired at various stages of rendering to allow your plugin to render stuff                                           | None                                 |
| `windowtitle`        | Emitted when a window title changes                                                                                 | Window data                          |
| `configReloaded`     | Emitted after the config is reloaded                                                                                | None                                 |
| `preConfigReload`    | Emitted before a config reload                                                                                      | None                                 |
| `keyPress`           | Emitted on a key press                                                                                              | None                                 |
| `pin`                | Emitted when a window is pinned or unpinned                                                                         | Window data                          |
| `swipeBegin`         | Emitted when a touchpad swipe is commenced                                                                          | None                                 |
| `swipeUpdate`        | Emitted when a touchpad swipe is updated                                                                            | None                                 |
| `swipeEnd`           | Emitted when a touchpad swipe is ended                                                                              | None                                 |

In case the information is wrong or outdated, please refer to <a href="https://wiki.hyprland.org/Plugins/Development/Event-list/">Hyprland's event list</a>.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- EXAMPLES -->

## Examples

### Which Key

<!-- align center -->
<p align="center">
  <img src="./assets/which-key.gif" alt="Which Key Demo" width="600" height="400">
</p>

Emacs, Neovim user here? You might be familiar with `which-key`.
If not, it's a plugin that displays the available keybindings in a minibuffer.
I'm horrible at remembering keybinds, so a way of showing possible keybinds is a must for me.
Hyprland has [a way](https://wiki.hyprland.org/Configuring/Binds/#description) to give keybinds a description and a way to print configured keybinds. So the only thing we need is a way to trigger a popup with the keybinds for a specific submap.

For a simple implementation we will use [eww](https://github.com/elkowar/eww) to display things and [jq](https://github.com/jqlang/jq) to parse the JSON data coming from `hyprctl`.

#### Installation

> [!IMPORTANT]
> For all of this to make sense you should have given your keybinds a description in your `hyprland.conf`.
> If you haven't done that yet, you can find more information [here](https://wiki.hyprland.org/Configuring/Binds/#description) or do it like this:

```ini
bindd = $mainMod, C, Close Window, closeWindow
```

| Dependency                 | Description                                            |
| -------------------------- | ------------------------------------------------------ |
| `eww`                      | A widget system to display the keybinds                |
| `jq`                       | A lightweight and flexible command-line JSON processor |
| `JetBrains Mono Nerd Font` | A font that supports the Nerd Font icons               |

> [!NOTE]
> If you want to use a different eww config directory, you will have to change the path used in the script.

1. Get the eww which-key config from [./examples/which-key/eww](./examples/which-key/eww/) and place it in `~/.config/eww-which-key`.
2. Get the which-key script from [./examples/which-key/which-key.sh](./examples/which-key/which-key.sh) and place it in your scripts directory. This example will use `~/.config/hypr/scripts`.
3. Don't forget to make the script executable with `chmod +x ~/.config/hypr/scripts/which-key.sh`.
4. Add the following lines to your `hyprland.conf`:

```ini
plugin {
  hyprhook {
    onSubmap = ~/.config/hypr/scripts/which-key.sh
  }
}
```

If you want you can also bind one key to show all keybinds that are not in a submap.
Just add the following line to your `hyprland.conf`:

```ini
bindd = $mainMod, H, Toggle Binds Help, exec, ~/.config/hypr/scripts/which-key.sh -b
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTRIBUTING -->

## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->

## License

Distributed under the MIT License. For details on what this means, please see the [MIT License summary](https://opensource.org/licenses/MIT) or refer to the `LICENSE.txt` file in this repository.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->

## Contact

Ugh maybe in git :)

Project Link: [https://github.com/Hyprhook/Hyprhook](https://github.com/Hyprhook/Hyprhook)

<p align="right">(<a href="#readme-top">back to top</a>)</p>
<!-- MARKDOWN LINKS & IMAGES -->

[contributors-shield]: https://img.shields.io/github/contributors/Hyprhook/Hyprhook.svg?style=for-the-badge
[contributors-url]: https://github.com/Hyprhook/Hyprhook/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/Hyprhook/Hyprhook.svg?style=for-the-badge
[forks-url]: https://github.com/Hyprhook/Hyprhook/network/members
[stars-shield]: https://img.shields.io/github/stars/Hyprhook/Hyprhook.svg?style=for-the-badge
[stars-url]: https://github.com/Hyprhook/Hyprhook/stargazers
[issues-shield]: https://img.shields.io/github/issues/Hyprhook/Hyprhook.svg?style=for-the-badge
[issues-url]: https://github.com/Hyprhook/Hyprhook/issues
[license-shield]: https://img.shields.io/github/license/Hyprhook/Hyprhook.svg?style=for-the-badge
[license-url]: https://github.com/Hyprhook/Hyprhook/blob/master/LICENSE.txt
