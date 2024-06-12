{
  description = "hyprhook";

  inputs = {
    hyprland.url = "git+https://github.com/hyprwm/Hyprland?submodules=1";
    systems.follows = "hyprland/systems";
  };

  outputs = {
    self,
    hyprland,
    systems,
    ...
  }: let
    inherit (hyprland.inputs) nixpkgs;
    inherit (nixpkgs) lib;
    eachSystem = lib.genAttrs (import systems);

    pkgsFor = eachSystem (system:
      import nixpkgs {
        localSystem.system = system;
        overlays = [
          self.overlays.hyprland-plugins
          hyprland.overlays.hyprland-packages
        ];
      });
  in {
    packages = eachSystem (system: {
      inherit
        (pkgsFor.${system})
        hyprhook
        nlohmann_json
        ;
    });

    overlays = {
      default = self.overlays.hyprland-plugins;

      hyprland-plugins = final: prev: let
        inherit (final) callPackage;
      in {
        hyprhook = callPackage ./hyprhook {};
      };
    };

    checks = eachSystem (system: self.packages.${system});

    devShells = eachSystem (system:
      with pkgsFor.${system}; {
        default = mkShell.override {stdenv = gcc13Stdenv;} {
          name = "hyprhook";
          buildInputs = [hyprland.packages.${system}.hyprland];
          nativeBuildInputs = [nlohmann_json];
          inputsFrom = [
            hyprland.packages.${system}.hyprland
            self.packages.${system}.hyprhook
          ];
        };
        programs.hyprland = {
          enable = true; 
          package = (hyprland.packages.${system}.hyprland.override { debug = true; });
        };
      });
  };
}
