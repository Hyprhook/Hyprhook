{
  lib,
  hyprland,
  hyprlandPlugins,
}:
hyprlandPlugins.mkHyprlandPlugin hyprland {
  pluginName = "hyprhook";
  version = "0.1";
  src = ./.;

  inherit (hyprland) nativeBuildInputs;

  meta = with lib; {
    homepage = "https://github.com/Yusuf-Duran/Hyprhook";
    description = "Execute binarys on hyprland events";
    license = licenses.mit;
    platforms = platforms.linux;
  };
}
