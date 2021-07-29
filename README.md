# Andamiro MK5 Emulation Layer

This library comprises a collection of HLE routines to replicate an Andamiro MK5 arcade computer.

Features:
- Programmable lockchip key via the CAT702_KEY envar.
- Keyboard support via the PIUIO_KEY envar.
- EEPROM Path support via the EEPROM_PATH envar.

To Use:
1. Bind this library against any MK5 game (windows/linux).
2. Set your envars:
    - 02c1f9787c1c10f0 for Premere 3
    - f0e1f9787cfec2c0 for Prex 3
    - f078f9fd1c20c202 for Exceed
3. Go for it!
