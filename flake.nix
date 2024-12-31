{
  description = "hyprhook";

  inputs = {
    hyprland.url = "git+https://github.com/hyprwm/Hyprland?submodules=1";
    nixpkgs.follows = "hyprland/nixpkgs";
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
          self.overlays.gcc14Stdenv
          hyprland.overlays.hyprland-packages
        ];
      });
  in {
    packages = eachSystem (system: {
      inherit
        (pkgsFor.${system})
        hyprhook
        ;
    });

    overlays = {
      default = self.overlays.hyprland-plugins;

      hyprland-plugins = final: prev: let
        inherit (final) callPackage;
      in {
        hyprhook = callPackage ./hyprhook {};
      };

      # TODO: remove when https://github.com/NixOS/nixpkgs/pull/365776 lands in master
      gcc14Stdenv = final: prev: {
        hyprlandPlugins =
          (prev.hyprlandPlugins or {})
          // {
            mkHyprlandPlugin = hyprland: args @ {pluginName, ...}:
              hyprland.stdenv.mkDerivation (
                args
                // {
                  pname = pluginName;
                  nativeBuildInputs = [prev.pkg-config] ++ args.nativeBuildInputs or [];
                  buildInputs = [hyprland] ++ hyprland.buildInputs ++ (args.buildInputs or []);
                  meta =
                    args.meta
                    // {
                      description = args.meta.description or "";
                      longDescription =
                        (args.meta.longDescription or "")
                        + "\n\nPlugins can be installed via a plugin entry in the Hyprland NixOS or Home Manager options.";
                    };
                }
              );
          };
      };
    };


    checks = eachSystem (system: self.packages.${system});

    devShells = eachSystem (system:
      with pkgsFor.${system}; {
        default = mkShell.override {stdenv = gcc14Stdenv;} {
          name = "hyprhook";
          buildInputs = [hyprland.packages.${system}.hyprland];
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
