================================================================================
          ANTBOY (2026) — PANDUAN STRUKTUR FOLDER KARTU MICROSD
================================================================================

Folder ini berisi seluruh file ROM retro dan musik yang siap disalin langsung 
ke MicroSD Anda untuk dimainkan di perangkat ANTBOY.

--------------------------------------------------------------------------------
CARA MEMASANG KE KARTU MICROSD:
--------------------------------------------------------------------------------
1. Masukkan kartu MicroSD ke PC / Card Reader Anda (format FAT32).
2. Salin SELURUH folder dari folder ini ("sdcard_content") langsung ke 
   ROOT kartu MicroSD Anda (misalnya drive E:\ atau D:\):

   Root MicroSD (E:\)
   ├── /roms/
   │   ├── /gb/      <- ROM Game Boy (.gb) & Game Boy Color (.gbc)
   │   ├── /nes/     <- ROM Nintendo NES / Famicom (.nes)
   │   ├── /sms/     <- ROM Sega Master System (.sms) & Game Gear (.gg)
   │   ├── /atari/   <- ROM Atari 2600 (.bin) & CHIP-8 VM (.ch8)
   │   └── /saves/   <- File Save RAM & State (.sav) otomatis tersimpan di sini
   ├── /music/       <- File Lagu 8-Bit Chiptune (.rtttl)
   ├── /apps/        <- Aplikasi & payload eksternal
   ├── /logs/        <- Catatan log sistem & network audit
   └── /screenshots/ <- Tangkapan layar framebuffer LCD

3. Lepaskan kartu MicroSD dari PC dengan aman (Safely Remove).
4. Masukkan kartu MicroSD ke slot J2 di ANTBOY Anda.
5. Nyalakan ANTBOY: Ikon kartu SD di Status Bar atas akan berwarna HIJAU [SD READY].
6. Buka pilar RETRO GAMING di launcher, pilih konsol yang ingin dimainkan, 
   dan ROM akan langsung muncul pada daftar menu!

--------------------------------------------------------------------------------
DAFTAR GAME YANG SUDAH TERSEDIA:
--------------------------------------------------------------------------------
[1] Game Boy (GB) & Game Boy Color (GBC) (/roms/gb/):
    - blastah.gb        : Space shooter arcade klasik
    - brickster.gbc     : Game Boy Color Breakout / Arkanoid
    - burly.gbc         : Platformer petualangan GBC
    - combatsoccer.gbc  : Pertandingan sepak bola aksi arcade
    - geometrix.gbc     : Puzzle balok susun warna
    - initiald.gbc      : Balapan drift jalan raya
    - klondike.gbc      : Permainan kartu klasik
    - pokedamon.gbc     : Parodi RPG pertarungan monster
    - ucity.gbc         : Simulasi pembangunan kota ala SimCity

[2] Nintendo NES / Famicom (/roms/nes/):
    - ambushed.nes      : Tembak-tembakan aksi NES
    - assimilate.nes    : Cyber sci-fi action
    - blaster.nes       : Tembak sasaran arcade
    - bombarray.nes     : Ledakan bom puzzle
    - bootee.nes        : Platformer aksi
    - cheril-the-goddess: Petualangan aksi fantasi
    - cl1k.nes          : Puzzle refleks
    - croom.nes         : Maze escape
    - dabg.nes          : Retro arcade
    - debrisdodger.nes  : Menghindar asteroid luar angkasa

[3] Sega Master System (/roms/sms/):
    - astroforce.sms    : Shmup luar angkasa grafis halus
    - 2048sanqui.sms    : Puzzle angka 2048
    - 3dcity.sms        : Aksi 3D kota
    - acidreflux.sms    : Aksi arcade
    - artillerymaster8k : Tembak artileri taktis
    - balubabalok.sms   : Petualangan platformer
    - battleships.sms   : Perang kapal laut
    - blockquest.sms    : Petualangan teka-teki

[4] Atari 2600 & CHIP-8 (/roms/atari/):
    - halo2600.bin      : Karya legendaris Master Chief ala Atari 2600 (Ed Fries)
    - flappy_the_duck   : Flappy Bird retro Atari
    - anguna.bin        : Action RPG Zelda-style
    - dkarcade2600.bin  : Donkey Kong arcade
    - fishy.bin         : Survival ikan laut
    - nanowing.bin      : Pesawat luar angkasa
    - jammed.bin        : Puzzle geser mobil
    - pong.ch8          : CHIP-8 Pong klasik
    - brix.ch8          : CHIP-8 Breakout
    - invaders.ch8      : CHIP-8 Space Invaders
    - tetris.ch8        : CHIP-8 Tetris
    - blinky.ch8        : CHIP-8 Pac-Man clone
    - astrododge.ch8    : CHIP-8 Astro Dodge
    - airplane.ch8      : CHIP-8 Airplane Dogfight
    - blitz.ch8         : CHIP-8 City Bomber

[5] Chiptune 8-Bit Audio Tracks (/music/):
    - tetris.rtttl      : Korobeiniki (Game Boy Theme A)
    - mario.rtttl       : Super Mario Bros Overworld
    - zelda.rtttl       : The Legend of Zelda Overworld
    - pacman.rtttl      : Pac-Man Intro Jingle
    - megaman.rtttl     : Mega Man 2 (Dr. Wily Stage 1)
    - doom.rtttl        : Doom (E1M1 - At Doom's Gate)
    - pokemon.rtttl     : Pokemon Title Theme
    - starwars.rtttl    : Star Wars Main Theme
    - castlevania.rtttl : Castlevania Vampire Killer

================================================================================
Selamat bernostalgia bermain retro game di ANTBOY!
================================================================================
