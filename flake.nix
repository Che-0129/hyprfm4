{
  inputs.nixpkgs.url = "nixpkgs";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        pname = "hyprfm4";
        version = "0.1";
        src = ./.;

        nativeBuildInputs = [ pkgs.pkg-config ];
        buildInputs = [ pkgs.hyprland.dev ] ++ pkgs.hyprland.buildInputs;

        buildPhase = ''
          runHook preBuild
          $CXX -shared -fPIC --no-gnu-unique main.cpp -o hyprfm4.so -std=c++26 \
            $(pkg-config --cflags pixman-1 libdrm hyprland)
          runHook postBuild
        '';

        installPhase = ''
          runHook preInstall
          install -Dm755 hyprfm4.so $out/lib/hyprfm4.so
          runHook postInstall
        '';
      };

      devShells.${system}.default = pkgs.mkShell {
        inputsFrom = [ pkgs.hyprland ];
        packages = [ pkgs.hyprland.dev pkgs.pkg-config ];
      };
    };
}
