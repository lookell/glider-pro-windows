# Converting Macintosh Glider PRO houses to files that work with this port

(The commands shown below are written for the CMD shell.)

You will need to be able to access ImageMagick (`magick`), FFmpeg (`ffmpeg`),
and Microsoft's resource compiler (`rc`) and linker (`link`) to convert a
house's resource fork into a file that can be used by the game.

First, compile the `resfork-extract` tool in this repository. It is written
in the Rust language:

    cd Tools\resfork-extract
    cargo build --release

Next, run the `resfork-extract` tool on an AppleDouble, MacBinary, or raw
resource fork containing a house's resources:

    cargo run --release convert Slumberland.rsrc Slumberland.zip

Extract the output zip file to a convenient directory and then, from that
directory, run the generated batch script:

    .\build.bat

If all goes well, the final output file will be generated (`house.glr`).
Make a copy of the house's data fork and rename it to something like
`Slumberland.glh`, with extension `.glh`. Rename `house.glr` to a matching
base name; in this case it would be `Slumberland.glr`. These two files
together can now be placed into the game's Houses directory, and the game
should load them together without any problems.


## Explanation

In old Macintosh systems, the file was split into two parts: the data fork,
which contains all of the main data for the file; and the resource fork, which
contains resources that are related to the file's data fork. These resources are
packaged in a standard format. The old formats used in these resource forks are
not well supported by modern systems, so the tool `resfork-extract` was written
to help translate these old resources into slightly more modern formats.

The house files for Glider PRO use the data fork to store the main data for the
house: a list of rooms, where objects are within the rooms, the backgrounds to
use within the rooms, etc. The resource fork of a house file contains custom
pictures, custom sounds, and room boundary information (for older houses).

In this port of Glider PRO for Windows, I have chosen to - at least to begin
with - use resource-only DLLs to store the resources of a house. Pictures that
were stored in the `'PICT'` format on Macintosh have been converted to the
BMP format for Windows. Sounds that were stored in the `'snd '` format on a
Macintosh have been converted to the WAV format for Windows.

To perform this resource conversion, the tool `resfork-extract` was developed
alongside this source port. It can take the resources from an AppleSingle,
AppleDouble, MacBinary, or raw resource fork format, and output translated
versions into a zip file.

To package up the pictures and sounds into a DLL file, `resfork-extract` also
outputs a batch script into the zip file it generates. This batch script uses
the tools ImageMagick (via the `magick` command) and FFmpeg (via the `ffmpeg`
command) to compress the BMP files and convert the generated AIFF files into
WAV files, respectively. It then calls upon Microsoft's resource compiler (`rc`)
to compile the generated resource script. Finally, it calls Microsoft's linker
(`link`) to bundle the `.res` file into the final resource DLL, which will be
named `house.glr`.

When loading houses, the game looks for a pair of corresponding files. If the
base house name is `Slumberland`, then the game looks for `Slumberland.glh`
(the data fork) and `Slumberland.glr` (the converted resources). If the `.glr`
file is missing, the game will complain but still load the house.

