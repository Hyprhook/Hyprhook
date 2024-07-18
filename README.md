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
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>

## Installation

### Manual

To build, have hyprland headers installed and under the repo directory do:

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
> The event 'submap' is called 'onSubmap' in the configuration file.

Example configuration snippet:

```ini
hyprhook {
  activeWindow = /path/to/your/script.sh
  openWindow = /path/to/another/script.sh # Add more event-script mappings as needed
}
```
> [!NOTE]
> Ensure your scripts are executable and have the appropriate shebang (`#!/bin/bash` for bash scripts, for example).

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## JSON Parameters

The plugin uses the following functions to generate JSON parameters for each event:

- `Submap Data`: Returns a json with the attribut submap for the submap name changed to.
- `Window Data`: Returns JSON for a window using like with `hyprctl activeWindow -j`.
- `parseWorkspace`: Returns JSON for a workspace using `CHyprCtl::getWorkspaceData`.
- `parseCWorkspace`: Returns JSON for a `CWorkspace` object.
- `parseMonitor`: Returns JSON for a monitor using `CHyprCtl::getMonitorData`.
- `parseVectorWorkspaceMonitor`: Returns a JSON array for a vector of workspace and monitor data.
- `parseVectorWindowWorkspace`: Returns a JSON array for a vector of window and workspace data.
- `parseVector2D`: Returns JSON for a `Vector2D` object.
- `parseSButtonEvent`: Returns JSON for a button event (`IPointer::SButtonEvent`).
- `parseSDownEvent`: Returns JSON for a touch down event (`ITouch::SDownEvent`).
- `parseSUpEvent`: Returns JSON for a touch up event (`ITouch::SUpEvent`).

### List of Supported Events

| Event                | Description                              | Parameter (JSON)                     |
| -------------------- | ---------------------------------------- | ------------------------------------ |
| `activeWindow`       | Triggered when the active window changes | Window data                          |
| `keyboardFocus`      | Keyboard focus change                    | None                                 |
| `moveWorkspace`      | Workspace moved                          | Vector of Workspace and Monitor data |
| `focusedMon`         | Focused monitor changes                  | Monitor data                         |
| `moveWindow`         | Window moved                             | Vector of Window and Workspace data  |
| `openLayer`          | A layer is opened                        | None                                 |
| `closeLayer`         | A layer is closed                        | None                                 |
| `openWindow`         | A window is opened                       | Window data                          |
| `closeWindow`        | A window is closed                       | Window data                          |
| `windowUpdateRules`  | Window update rules                      | Window data                          |
| `urgent`             | Urgent window event                      | Window data                          |
| `minimize`           | Window minimized                         | None                                 |
| `monitorAdded`       | A monitor is added                       | Monitor data                         |
| `monitorRemoved`     | A monitor is removed                     | Monitor data                         |
| `createWorkspace`    | A workspace is created                   | Workspace data                       |
| `destroyWorkspace`   | A workspace is destroyed                 | Workspace data                       |
| `fullscreen`         | Fullscreen mode toggled                  | Window data                          |
| `changeFloatingMode` | Floating mode toggled                    | Window data                          |
| `workspace`          | Workspace changed                        | Workspace data                       |
| `submap`             | Submap event                             | Submap data                          |
| `mouseMove`          | Mouse moved                              | Vector2D data                        |
| `mouseButton`        | Mouse button event                       | Button event data                    |
| `mouseAxis`          | Mouse axis event                         | None                                 |
| `touchDown`          | Touch down event                         | Touch down event data                |
| `touchUp`            | Touch up event                           | Touch up event data                  |
| `touchMove`          | Touch move event                         | None                                 |
| `activeLayout`       | Active layout change                     | None                                 |
| `preRender`          | Pre-render event                         | Monitor data                         |
| `screencast`         | Screencast event                         | None                                 |
| `render`             | Render event                             | None                                 |
| `windowtitle`        | Window title changed                     | Window data                          |
| `configReloaded`     | Configuration reloaded                   | None                                 |
| `preConfigReload`    | Pre-configuration reload                 | None                                 |
| `keyPress`           | Key press event                          | None                                 |
| `pin`                | Pin event                                | Window data                          |
| `swipeBegin`         | Swipe begin event                        | None                                 |
| `swipeUpdate`        | Swipe update event                       | None                                 |
| `swipeEnd`           | Swipe end event                          | None                                 |

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

Distributed under the MIT License. See `LICENSE.txt` for more information.

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
