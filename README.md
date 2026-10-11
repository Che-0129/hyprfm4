# hyprfm4

A Hyprland plugin that makes `input.follow_mouse = 2` work better with scrolling layouts

With `follow_mouse = 2`, clicking a window moves keyboard focus to it, and the click is also delivered to the app. When the window is partly off-screen, that click often lands on something you didn't mean to hit, like a Firefox scrollbar or a link, because the layout scrolls the window into view at the same moment

hyprfm4 fixes this: if you click an unfocused window that overflows the monitor, it only focuses the window. The click is not passed to the app

## Requirements

- Hyprland **v0.56.2**
- `input.follow_mouse = 2`

## Install (NixOS flake)

Add the input, following your system nixpkgs so the Hyprland version matches

```nix
inputs.hyprfm4 = {
  url = "github:Che-0129/hyprfm4";
  inputs.nixpkgs.follows = "nixpkgs";
};
```

Load the plugin, e.g. with home-manager

```nix
wayland.windowManager.hyprland.plugins = [
  "${inputs.hyprfm4.packages.${pkgs.system}.default}/lib/hyprfm4.so"
];
```
