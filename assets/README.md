# Game assets

The graphical game uses Raylib's built-in drawing for the original reel symbols and interface, so it runs without bundled image or sound files. Optional files can be added here:

- `symbols/`: original or permissively licensed symbol artwork.
- `backgrounds/`: optional scene backgrounds.
- `ui/slot-display.ttf`: optional display font. When absent or unreadable, the game uses Raylib's built-in font.
- `sounds/`: optional sound effects if audio is added later.

Keep resource files small, include license and source details when adding third-party assets, and load them once during startup. The current game has no sound or texture resources to leak or reload.
