# Converting Macintosh Glider PRO houses to files that work with this port

(The commands shown below are written for the CMD shell.)

First, compile the `resfork-extract` tool in this repository. It is written
in the Rust language:

    cd Tools\resfork-extract
    cargo build --release

Next, run the `resfork-extract` tool on an AppleDouble, MacBinary, or raw
resource fork containing a house's resources. The first file name must be
the raw house data, the second must be the resource fork, and the third must
be the desired output file:

    cargo run --release gliderpro Slumberland Slumberland.rsrc Slumberland.glh

If this succeeds, then Slumberland.glh is ready to be used by Glider PRO.
Now, simply place it into the `Houses` directory for Glider PRO to find.
If there is a `portable.dat` next to the Glider PRO executable, the Houses
directory is also next to that executable. If `portable.dat` does not exist,
then the directory is at `%APPDATA%\glider-pro-windows\Houses`.


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

In this port of Glider PRO for Windows, I have chosen to package the house data
and its custom resources together into a single zip file. The PICT resources are
converted into BMP files and placed in the `images` directory inside the zip
file. The sound resources are converted into WAV files and placed in the
`sounds` directory inside the zip file.

To perform this resource conversion, the tool `resfork-extract` was developed
alongside this source port. It can take the resources from an AppleSingle,
AppleDouble, MacBinary, or raw resource fork format, and output translated
versions into a zip file. It can also take a house's data and resource fork
together to merge into a single zip file that Glider PRO can read and use.
