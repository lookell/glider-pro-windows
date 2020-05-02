mod apple_double;
mod bitmap;
mod icocur;
mod mac_clut;
mod mac_roman;
mod macbinary;
mod res;
mod rsrcfork;
mod utils;

use crate::apple_double::AppleDouble;
use crate::bitmap::{Bitmap, BitmapEight, BitmapFour, BitmapOne};
use crate::icocur::IconFile;
use crate::macbinary::MacBinary;
use crate::rsrcfork::{ResType, Resource, ResourceFork};
use std::collections::{HashMap, HashSet};
use std::env;
use std::error::Error;
use std::ffi::OsString;
use std::fs::File;
use std::io::prelude::*;
use std::io::{self, BufReader, BufWriter, Cursor, SeekFrom};
use std::path::Path;
use zip::ZipWriter;

pub type AnyResult<T> = Result<T, Box<dyn Error>>;

fn extract_resource_bytes(mut reader: impl Read + Seek) -> io::Result<Vec<u8>> {
    reader.seek(SeekFrom::Start(0))?;
    if let Ok(Some(data)) = AppleDouble::read_from(&mut reader) {
        return Ok(data.rsrc);
    }
    reader.seek(SeekFrom::Start(0))?;
    if let Ok(Some(data)) = MacBinary::read_from(&mut reader) {
        return Ok(data.rsrc);
    }
    reader.seek(SeekFrom::Start(0))?;
    let mut bytes = Vec::new();
    reader.read_to_end(&mut bytes).map(|_| bytes)
}

fn unprintable_to_period(b: u8) -> u8 {
    match b {
        0x00..=0x1F | 0x7F => b'.',
        _ => b,
    }
}

fn hex_byte_line(bytes: &[u8]) -> String {
    let mut line = String::from("$\"");
    for pair in bytes.chunks(2) {
        match pair.len() {
            1 => line.push_str(&format!("{:02X} ", pair[0])),
            2 => line.push_str(&format!("{:02X}{:02X} ", pair[0], pair[1])),
            _ => unreachable!(),
        }
    }
    let _space = line.pop();
    line.push('"');
    let bytes_string = bytes
        .iter()
        .copied()
        .map(unprintable_to_period)
        .map(mac_roman::decode)
        .collect::<String>();
    format!("\t{: <53} /* {} */", line, bytes_string)
}

fn escape_squote(unescaped: &str) -> String {
    let mut escaped = String::with_capacity(unescaped.len());
    for c in unescaped.chars() {
        if c == '\'' {
            escaped.push('\\');
        }
        escaped.push(c);
    }
    escaped
}

fn escape_dquote(unescaped: &str) -> String {
    let mut escaped = String::with_capacity(unescaped.len());
    for c in unescaped.chars() {
        if c == '\"' {
            escaped.push('\\');
        }
        escaped.push(c);
    }
    escaped
}

fn derez_resfork(resfork: &ResourceFork, mut writer: impl Write) -> AnyResult<()> {
    for res in resfork.iter() {
        let restype_str = escape_squote(&res.restype.to_string());
        write!(&mut writer, "data '{}' ({}", restype_str, res.id)?;
        if let Some(name) = &res.name {
            write!(&mut writer, ", \"{}\"", escape_dquote(name))?;
        }
        writeln!(&mut writer, ") {{")?;
        for row in res.data.chunks(16) {
            writeln!(&mut writer, "{}", hex_byte_line(row))?;
        }
        writeln!(&mut writer, "}};\n")?;
    }
    Ok(())
}

fn make_zip_entry_path(res: &Resource) -> String {
    let typ = res.restype;
    let id = res.id;
    fn valid_char(b: u8) -> bool {
        b.is_ascii_alphanumeric() || b"-_.#() ".contains(&b)
    }
    if typ.bytes.iter().copied().all(valid_char) {
        return format!("({})/{}.bin", typ, id);
    } else {
        return format!(
            "({:02X}-{:02X}-{:02X}-{:02X})/{}.bin",
            u32::from(typ.bytes[0]),
            u32::from(typ.bytes[1]),
            u32::from(typ.bytes[2]),
            u32::from(typ.bytes[3]),
            id
        );
    }
}

fn write_names_txt(resfork: &ResourceFork, mut writer: impl Write) -> io::Result<()> {
    for res in resfork.iter() {
        if let Some(name) = &res.name {
            writeln!(&mut writer, "('{}', {}): {:?}", res.restype, res.id, name)?;
        }
    }
    Ok(())
}

fn dump_resfork(resfork: &ResourceFork, writer: impl Seek + Write) -> AnyResult<()> {
    let mut zip_writer = ZipWriter::new(writer);
    zip_writer.set_comment("");

    if resfork.iter().any(|res| res.name.is_some()) {
        zip_writer.start_file("names.txt", Default::default())?;
        write_names_txt(resfork, &mut zip_writer)?;
    }

    for res in resfork.iter() {
        let entry_name = make_zip_entry_path(&res);
        zip_writer.start_file(entry_name, Default::default())?;
        zip_writer.write_all(res.data.as_slice())?;
    }
    Ok(())
}

fn convert_resfork(resfork: &ResourceFork, writer: impl Seek + Write) -> AnyResult<()> {
    let mut zip_writer = ZipWriter::new(writer);
    zip_writer.set_comment("");

    if resfork.iter().any(|res| res.name.is_some()) {
        zip_writer.start_file("names.txt", Default::default())?;
        write_names_txt(resfork, &mut zip_writer)?;
    }

    let mut finder_icon_ids = HashSet::new();
    for res in resfork.iter() {
        match res.restype.as_bstr() {
            b"icl8" | b"icl4" | b"ICN#" | b"ics8" | b"ics4" | b"ics#" => {
                finder_icon_ids.insert(res.id);
            }
            _ => {}
        }
    }

    let mut rc_script = String::new();
    rc_script += "#pragma code_page(65001)\n";
    rc_script += "#include <windows.h>\n";
    rc_script += "LANGUAGE LANG_ENGLISH, SUBLANG_ENGLISH_US\n";
    for icon_id in finder_icon_ids.iter().copied() {
        let unsigned_id = icon_id as i16 as u16;
        rc_script += &format!("{0} ICON \"build\\\\icon_{1}.ico\"\n", unsigned_id, icon_id);
    }
    for res in resfork.iter_type(ResType::new(b"bnds")) {
        rc_script += &format!("{0} BOUNDS \"build\\\\bnds_{0}.bin\"\n", res.id);
    }
    for res in resfork.iter_type(ResType::new(b"PICT")) {
        rc_script += &format!("{0} BITMAP \"build\\\\pict_{0}.bmp\"\n", res.id);
    }
    for res in resfork.iter_type(ResType::new(b"snd ")) {
        rc_script += &format!("{0} WAVE \"build\\\\snd_{0}.wav\"\n", res.id);
    }
    zip_writer.start_file("resource.rc", Default::default())?;
    zip_writer.write_all(rc_script.as_bytes())?;

    let mut do_rle_script = String::new();
    do_rle_script += "magick convert %1 \"bmp3:%~2\"\n";
    do_rle_script += "if %~z1 leq %~z2 copy %1 %2 >nul\n";
    zip_writer.start_file("do_rle.cmd", Default::default())?;
    zip_writer.write_all(do_rle_script.as_bytes())?;

    let mut build_script = String::new();
    build_script += "@echo off\n";
    build_script += "rem This build script uses ImageMagick and FFmpeg for conversion\n";
    build_script += "if not exist \"%~dp0build\" mkdir \"%~dp0build\"\n";
    for icon_id in finder_icon_ids.iter().copied() {
        build_script += &format!(
            "copy \"%~dp0FinderIcon\\{0}.ico\" \"%~dp0build\\icon_{0}.ico\" >nul\n",
            icon_id
        );
    }
    for res in resfork.iter_type(ResType::new(b"bnds")) {
        build_script += &format!(
            "copy \"%~dp0(bnds)\\{0}.bin\" \"%~dp0build\\bnds_{0}.bin\" >nul\n",
            res.id
        );
    }
    build_script += "echo Processing bitmap images...\n";
    for res in resfork.iter_type(ResType::new(b"PICT")) {
        build_script += &format!(
            "call \"%~dp0do_rle.cmd\" \"%~dp0Picture\\{0}.bmp\" \"%~dp0build\\pict_{0}.bmp\"\n",
            res.id
        );
    }
    build_script += "echo Processing audio files...\n";
    for res in resfork.iter_type(ResType::new(b"snd ")) {
        build_script += &format!(
            "ffmpeg -v quiet -y -i \"%~dp0Sound\\{0}.aif\" -acodec pcm_u8 \"%~dp0build\\snd_{0}.wav\"\n",
            res.id
        );
    }
    build_script += "echo Compiling resource script...\n";
    build_script += "rc /nologo /fo \"%~dp0resource.res\" \"%~dp0resource.rc\"\n";
    build_script += "echo Linking resource DLL...\n";
    build_script +=
        "link /nologo /dll /noentry /machine:x86 \"/out:%~dp0house.glr\" \"%~dp0resource.res\"\n";
    zip_writer.start_file("build.cmd", Default::default())?;
    zip_writer.write_all(build_script.as_bytes())?;

    for res in resfork.iter() {
        match res.restype.as_bstr() {
            b"acur" => {
                let entry_name = res::animated_cursor::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::animated_cursor::convert(&res.data, &mut zip_writer)?;
            }
            b"ALRT" => {
                let entry_name = res::alert::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::alert::convert(&res.data, &mut zip_writer)?;
            }
            b"BNDL" => {
                let entry_name = res::bundle::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::bundle::convert(&res.data, &mut zip_writer)?;
            }
            b"cctb" => {
                let entry_name = res::control_color_table::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::control_color_table::convert(&res.data, &mut zip_writer)?;
            }
            b"CDEF" => {
                let entry_name = format!("ControlDefinitionFunction/{}.bin", res.id);
                zip_writer.start_file(entry_name, Default::default())?;
                zip_writer.write_all(&res.data)?;
            }
            b"cicn" => {
                let entry_name = res::color_icon::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::color_icon::convert(&res.data, &mut zip_writer)?;
            }
            b"clut" => {
                let entry_name = res::color_table::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::color_table::convert(&res.data, &mut zip_writer)?;
            }
            b"CNTL" => {
                let entry_name = res::control::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::control::convert(&res.data, &mut zip_writer)?;
            }
            b"crsr" => {
                let entry_name = res::color_cursor::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::color_cursor::convert(&res.data, &mut zip_writer)?;
            }
            b"CURS" => {
                let entry_name = res::cursor::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::cursor::convert(&res.data, &mut zip_writer)?;
            }
            b"dctb" => {
                let entry_name = res::dialog_color_table::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::dialog_color_table::convert(&res.data, &mut zip_writer)?;
            }
            b"demo" => {
                let entry_name = format!("DemoData/{}.bin", res.id);
                zip_writer.start_file(entry_name, Default::default())?;
                zip_writer.write_all(&res.data)?;
            }
            b"DITL" => {
                let entry_name = res::item_list::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::item_list::convert(&res.data, &mut zip_writer)?;
            }
            b"DLOG" => {
                let entry_name = res::dialog::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::dialog::convert(&res.data, &mut zip_writer)?;
            }
            b"FREF" => {
                let entry_name = res::file_reference::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::file_reference::convert(&res.data, &mut zip_writer)?;
            }
            b"ICON" => {
                let entry_name = res::icon::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::icon::convert(&res.data, &mut zip_writer)?;
            }
            b"icl8" => {
                let entry_name = res::large_8bit_icon::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                let mut icon_file = IconFile::new();
                let data_bits = res::large_8bit_icon::convert(&res.data)?;
                icon_file.add_entry(data_bits, BitmapOne::new(32, 32));
                icon_file.write_to(&mut zip_writer)?;
            }
            b"icl4" => {
                let entry_name = res::large_4bit_icon::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                let mut icon_file = IconFile::new();
                let data_bits = res::large_4bit_icon::convert(&res.data)?;
                icon_file.add_entry(data_bits, BitmapOne::new(32, 32));
                icon_file.write_to(&mut zip_writer)?;
            }
            b"ICN#" => {
                let entry_name = res::icon_list::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                let mut icon_file = IconFile::new();
                let (data_bits, mask_bits) = res::icon_list::convert(&res.data)?;
                icon_file.add_entry(data_bits, mask_bits);
                icon_file.write_to(&mut zip_writer)?;
            }
            b"ics8" => {
                let entry_name = res::small_8bit_icon::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                let mut icon_file = IconFile::new();
                let data_bits = res::small_8bit_icon::convert(&res.data)?;
                icon_file.add_entry(data_bits, BitmapOne::new(16, 16));
                icon_file.write_to(&mut zip_writer)?;
            }
            b"ics4" => {
                let entry_name = res::small_4bit_icon::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                let mut icon_file = IconFile::new();
                let data_bits = res::small_4bit_icon::convert(&res.data)?;
                icon_file.add_entry(data_bits, BitmapOne::new(16, 16));
                icon_file.write_to(&mut zip_writer)?;
            }
            b"ics#" => {
                let entry_name = res::small_icon_list::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                let mut icon_file = IconFile::new();
                let (data_bits, mask_bits) = res::small_icon_list::convert(&res.data)?;
                icon_file.add_entry(data_bits, mask_bits);
                icon_file.write_to(&mut zip_writer)?;
            }
            b"mctb" => {
                let entry_name = res::menu_color_table::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::menu_color_table::convert(&res.data, &mut zip_writer)?;
            }
            b"MENU" => {
                let entry_name = res::menu::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::menu::convert(&res.data, &mut zip_writer)?;
            }
            b"PICT" => {
                let entry_name = res::picture::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::picture::convert(&res.data, &mut zip_writer)?;
            }
            b"snd " => {
                let entry_name = res::sound::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::sound::convert(&res.data, &mut zip_writer)?;
            }
            b"STR#" => {
                let entry_name = res::string_list::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::string_list::convert(&res.data, &mut zip_writer)?;
            }
            b"vers" => {
                let entry_name = res::version::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::version::convert(&res.data, &mut zip_writer)?;
            }
            b"wctb" => {
                let entry_name = res::window_color_table::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::window_color_table::convert(&res.data, &mut zip_writer)?;
            }
            b"WDEF" => {
                let entry_name = format!("WindowDefinitionFunction/{}.bin", res.id);
                zip_writer.start_file(entry_name, Default::default())?;
                zip_writer.write_all(&res.data)?;
            }
            b"WIND" => {
                let entry_name = res::window::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::window::convert(&res.data, &mut zip_writer)?;
            }
            _ => {
                let entry_name = make_zip_entry_path(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                zip_writer.write_all(&res.data)?;
            }
        }
    }

    for resource in resfork.iter_type(ResType::new(b"PAT#")) {
        let patterns = res::pattern_list::convert(&resource.data)?;
        for (idx, patt) in patterns.into_iter().enumerate() {
            let entry_name = format!("PatternList/{}/{}.bmp", resource.id, idx);
            zip_writer.start_file(entry_name, Default::default())?;
            patt.write_bmp_file(&mut zip_writer)?;
        }
    }

    struct FinderIconBitmaps {
        large_8bit: Option<BitmapEight>,
        large_4bit: Option<BitmapFour>,
        large_1bit: Option<BitmapOne>,
        large_mask: BitmapOne,
        small_8bit: Option<BitmapEight>,
        small_4bit: Option<BitmapFour>,
        small_1bit: Option<BitmapOne>,
        small_mask: BitmapOne,
    }
    impl Default for FinderIconBitmaps {
        fn default() -> Self {
            Self {
                large_8bit: None,
                large_4bit: None,
                large_1bit: None,
                large_mask: BitmapOne::new(32, 32),
                small_8bit: None,
                small_4bit: None,
                small_1bit: None,
                small_mask: BitmapOne::new(16, 16),
            }
        }
    }
    let mut finder_icons = HashMap::<i16, FinderIconBitmaps>::new();
    for res in resfork.iter() {
        match res.restype.as_bstr() {
            b"icl8" => {
                let data_bits = res::large_8bit_icon::convert(&res.data)?;
                finder_icons.entry(res.id).or_default().large_8bit = Some(data_bits);
            }
            b"icl4" => {
                let data_bits = res::large_4bit_icon::convert(&res.data)?;
                finder_icons.entry(res.id).or_default().large_4bit = Some(data_bits);
            }
            b"ICN#" => {
                let (data_bits, mask_bits) = res::icon_list::convert(&res.data)?;
                let entry = finder_icons.entry(res.id).or_default();
                entry.large_1bit = Some(data_bits);
                entry.large_mask = mask_bits;
            }
            b"ics8" => {
                let data_bits = res::small_8bit_icon::convert(&res.data)?;
                finder_icons.entry(res.id).or_default().small_8bit = Some(data_bits);
            }
            b"ics4" => {
                let data_bits = res::small_4bit_icon::convert(&res.data)?;
                finder_icons.entry(res.id).or_default().small_4bit = Some(data_bits);
            }
            b"ics#" => {
                let (data_bits, mask_bits) = res::small_icon_list::convert(&res.data)?;
                let entry = finder_icons.entry(res.id).or_default();
                entry.small_1bit = Some(data_bits);
                entry.small_mask = mask_bits;
            }
            _ => {}
        }
    }
    for (icon_id, icon_entry) in finder_icons {
        let mut icon_file = IconFile::new();
        if let Some(large_8bit) = icon_entry.large_8bit {
            icon_file.add_entry(large_8bit.clone(), icon_entry.large_mask.clone());
        }
        if let Some(large_4bit) = icon_entry.large_4bit {
            icon_file.add_entry(large_4bit.clone(), icon_entry.large_mask.clone());
        }
        if let Some(large_1bit) = icon_entry.large_1bit {
            icon_file.add_entry(large_1bit.clone(), icon_entry.large_mask.clone());
        }
        if let Some(small_8bit) = icon_entry.small_8bit {
            icon_file.add_entry(small_8bit.clone(), icon_entry.small_mask.clone());
        }
        if let Some(small_4bit) = icon_entry.small_4bit {
            icon_file.add_entry(small_4bit.clone(), icon_entry.small_mask.clone());
        }
        if let Some(small_1bit) = icon_entry.small_1bit {
            icon_file.add_entry(small_1bit.clone(), icon_entry.small_mask.clone());
        }
        zip_writer.start_file(format!("FinderIcon/{}.ico", icon_id), Default::default())?;
        icon_file.write_to(&mut zip_writer)?;
    }

    Ok(())
}

fn parse_resfork<P: AsRef<Path>>(filename: P) -> io::Result<ResourceFork> {
    let file_reader = BufReader::new(File::open(filename)?);
    let rsrc_cursor = Cursor::new(extract_resource_bytes(file_reader)?);
    ResourceFork::read_from(rsrc_cursor)
}

fn do_derez_command<I>(args: I) -> Result<(), Box<dyn Error>>
where
    I: IntoIterator<Item = OsString>,
{
    let mut args = args.into_iter();
    let input_name = match args.next() {
        Some(s) => s,
        None => {
            invalid_cmdline();
            return Ok(());
        }
    };
    let output_name = args.next();

    let resfork = parse_resfork(input_name)?;
    match output_name {
        Some(filename) => {
            let output_file = BufWriter::new(File::create(filename)?);
            derez_resfork(&resfork, output_file)
        }
        None => derez_resfork(&resfork, io::stdout().lock()),
    }
}

fn do_dump_command<I>(args: I) -> Result<(), Box<dyn Error>>
where
    I: IntoIterator<Item = OsString>,
{
    let mut args = args.into_iter();
    let input_name = match args.next() {
        Some(s) => s,
        None => {
            invalid_cmdline();
            return Ok(());
        }
    };
    let output_name = match args.next() {
        Some(s) => s,
        None => {
            invalid_cmdline();
            return Ok(());
        }
    };

    let resfork = parse_resfork(input_name)?;
    let output_file = BufWriter::new(File::create(output_name)?);
    dump_resfork(&resfork, output_file)
}

fn do_convert_command<I>(args: I) -> Result<(), Box<dyn Error>>
where
    I: IntoIterator<Item = OsString>,
{
    let mut args = args.into_iter();
    let input_name = match args.next() {
        Some(s) => s,
        None => {
            invalid_cmdline();
            return Ok(());
        }
    };
    let output_name = match args.next() {
        Some(s) => s,
        None => {
            invalid_cmdline();
            return Ok(());
        }
    };

    let resfork = parse_resfork(input_name)?;
    let output_file = BufWriter::new(File::create(output_name)?);
    convert_resfork(&resfork, output_file)
}

fn main() -> Result<(), Box<dyn Error>> {
    let mut args = env::args_os().skip(1);
    let command = args.next();
    match command.as_ref().and_then(|s| s.to_str()) {
        Some("derez") => do_derez_command(args)?,
        Some("dump") => do_dump_command(args)?,
        Some("convert") => do_convert_command(args)?,
        Some("--help") => print_long_help(),
        None => print_short_help(),
        _ => invalid_cmdline(),
    }
    Ok(())
}

fn invalid_cmdline() {
    eprintln!(
        r#"error: invalid usage

Run "resfork-extract --help" for usage information."#
    );
}

fn print_short_help() {
    eprintln!(
        r#"resfork-extract
Extract information from a Macintosh resource fork

Run "resfork-extract --help" for usage information."#
    );
}

fn print_long_help() {
    eprintln!(
        r#"resfork-extract
Extract information from a Macintosh resource fork

USAGE:
    resfork-extract <subcommand>

FLAGS:
    --help          Print this help information

SUBCOMMANDS:
    derez <resource-fork> [<output-text>]

        Produce output similar to Apple's old DeRez resource decompiler.
        If <output-text> is omitted, then standard output is used.

    dump <resource-fork> <output-zip>

        Dump the raw bytes of each resource in the resource fork into
        its own entry in the output .zip file. Each resource is grouped
        under a directory named after its type (e.g., 'PICT' or 'DLOG'),
        and named after its ID number.

    convert <resource-fork> <output-zip>

        Like the 'dump' command, but converts any resources that it
        knows about into a more modern or human-readable format, if
        possible. If a conversion fails or the format is unknown, then
        the raw bytes are dumped. For example 'PICT' resources are
        converted into BMP image files, and 'snd ' resources are
        converted into AIFF audio files.

REMARKS:

    The resource fork input file can be in either an AppleDouble
    container file, a MacBinary container file, or the raw resource
    fork's bytes on its own. resfork-extract attempts to decode the
    input file as AppleDouble, then as MacBinary, then as raw bytes."#
    );
}
