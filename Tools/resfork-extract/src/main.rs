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
use crate::bitmap::Bitmap;
use crate::macbinary::MacBinary;
use crate::rsrcfork::{ResType, Resource, ResourceFork};
use std::env;
use std::error::Error;
use std::fs::File;
use std::io::{self, BufReader, BufWriter, Cursor, Read, Seek, SeekFrom, Write};
use zip::ZipWriter;

pub type AnyResult<T> = Result<T, Box<dyn Error>>;

fn extract_resource_bytes(mut reader: impl Read + Seek) -> io::Result<Option<Vec<u8>>> {
    reader.seek(SeekFrom::Start(0))?;
    if let Ok(Some(data)) = AppleDouble::read_from(&mut reader) {
        return Ok(Some(data.rsrc));
    }
    reader.seek(SeekFrom::Start(0))?;
    if let Ok(Some(data)) = MacBinary::read_from(&mut reader) {
        return Ok(Some(data.rsrc));
    }
    reader.seek(SeekFrom::Start(0))?;
    let mut bytes = Vec::new();
    reader.read_to_end(&mut bytes).map(|_| Some(bytes))
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
    fn valid_char(c: char) -> bool {
        c.is_ascii_alphanumeric() || "-_.#() ".contains(c)
    }
    if typ.chars.iter().copied().all(valid_char) {
        return format!("({})/{}.bin", typ, id);
    } else {
        return format!(
            "({:02X}-{:02X}-{:02X}-{:02X})/{}.bin",
            u32::from(typ.chars[0]),
            u32::from(typ.chars[1]),
            u32::from(typ.chars[2]),
            u32::from(typ.chars[3]),
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

    let mut rc_script = String::new();
    rc_script += "#pragma code_page(65001)\n";
    rc_script += "#include <windows.h>\n";
    rc_script += "LANGUAGE LANG_ENGLISH, SUBLANG_ENGLISH_US\n";
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
        match res.restype.to_string().as_str() {
            "acur" => {
                let entry_name = res::animated_cursor::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::animated_cursor::convert(&res.data, &mut zip_writer)?;
            }
            "ALRT" => {
                let entry_name = res::alert::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::alert::convert(&res.data, &mut zip_writer)?;
            }
            "BNDL" => {
                let entry_name = res::bundle::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::bundle::convert(&res.data, &mut zip_writer)?;
            }
            "cctb" => {
                let entry_name = res::control_color_table::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::control_color_table::convert(&res.data, &mut zip_writer)?;
            }
            "CDEF" => {
                let entry_name = format!("ControlDefinitionFunction/{}.bin", res.id);
                zip_writer.start_file(entry_name, Default::default())?;
                zip_writer.write_all(&res.data)?;
            }
            "cicn" => {
                let entry_name = res::color_icon::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::color_icon::convert(&res.data, &mut zip_writer)?;
            }
            "clut" => {
                let entry_name = res::color_table::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::color_table::convert(&res.data, &mut zip_writer)?;
            }
            "CNTL" => {
                let entry_name = res::control::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::control::convert(&res.data, &mut zip_writer)?;
            }
            "crsr" => {
                let entry_name = res::color_cursor::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::color_cursor::convert(&res.data, &mut zip_writer)?;
            }
            "CURS" => {
                let entry_name = res::cursor::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::cursor::convert(&res.data, &mut zip_writer)?;
            }
            "dctb" => {
                let entry_name = res::dialog_color_table::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::dialog_color_table::convert(&res.data, &mut zip_writer)?;
            }
            "demo" => {
                let entry_name = format!("DemoData/{}.bin", res.id);
                zip_writer.start_file(entry_name, Default::default())?;
                zip_writer.write_all(&res.data)?;
            }
            "DITL" => {
                let entry_name = res::item_list::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::item_list::convert(&res.data, &mut zip_writer)?;
            }
            "DLOG" => {
                let entry_name = res::dialog::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::dialog::convert(&res.data, &mut zip_writer)?;
            }
            "FREF" => {
                let entry_name = res::file_reference::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::file_reference::convert(&res.data, &mut zip_writer)?;
            }
            "ICON" => {
                let entry_name = res::icon::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::icon::convert(&res.data, &mut zip_writer)?;
            }
            "icl4" => {
                let entry_name = res::large_4bit_icon::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::large_4bit_icon::convert(&res.data, &mut zip_writer)?;
            }
            "icl8" => {
                let entry_name = res::large_8bit_icon::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::large_8bit_icon::convert(&res.data, &mut zip_writer)?;
            }
            "ICN#" => {
                let entry_name = res::icon_list::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::icon_list::convert(&res.data, &mut zip_writer)?;
            }
            "ics4" => {
                let entry_name = res::small_4bit_icon::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::small_4bit_icon::convert(&res.data, &mut zip_writer)?;
            }
            "ics8" => {
                let entry_name = res::small_8bit_icon::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::small_8bit_icon::convert(&res.data, &mut zip_writer)?;
            }
            "ics#" => {
                let entry_name = res::small_icon_list::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::small_icon_list::convert(&res.data, &mut zip_writer)?;
            }
            "mctb" => {
                let entry_name = res::menu_color_table::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::menu_color_table::convert(&res.data, &mut zip_writer)?;
            }
            "MENU" => {
                let entry_name = res::menu::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::menu::convert(&res.data, &mut zip_writer)?;
            }
            "PAT#" => {
                let patterns = res::pattern_list::convert(&res.data)?;
                for (i, patt) in patterns.into_iter().enumerate() {
                    let entry_name = format!("PatternList/{}/{}.bmp", res.id, i);
                    zip_writer.start_file(entry_name, Default::default())?;
                    patt.write_bmp_file(&mut zip_writer)?;
                }
            }
            "PICT" => {
                let entry_name = res::picture::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::picture::convert(&res.data, &mut zip_writer)?;
            }
            "snd " => {
                let entry_name = res::sound::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::sound::convert(&res.data, &mut zip_writer)?;
            }
            "STR#" => {
                let entry_name = res::string_list::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::string_list::convert(&res.data, &mut zip_writer)?;
            }
            "vers" => {
                let entry_name = res::version::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::version::convert(&res.data, &mut zip_writer)?;
            }
            "wctb" => {
                let entry_name = res::window_color_table::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::window_color_table::convert(&res.data, &mut zip_writer)?;
            }
            "WDEF" => {
                let entry_name = format!("WindowDefinitionFunction/{}.bin", res.id);
                zip_writer.start_file(entry_name, Default::default())?;
                zip_writer.write_all(&res.data)?;
            }
            "WIND" => {
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
    Ok(())
}

fn main() {
    let arguments = env::args().skip(1).collect::<Vec<_>>();
    if arguments.len() < 2 {
        print_usage();
        return;
    }
    let command = &arguments[0];
    let input_name = &arguments[1];
    let output_name = arguments.get(2);
    let mut input_file = match File::open(input_name) {
        Ok(file) => BufReader::new(file),
        Err(e) => {
            eprintln!("error: could not open input file: {}", e);
            return;
        }
    };
    let output_file: Option<BufWriter<File>> = match output_name {
        Some(name) => match File::create(name) {
            Ok(file) => Some(BufWriter::new(file)),
            Err(e) => {
                eprintln!("error: could not open output file: {}", e);
                return;
            }
        },
        None => None,
    };
    let resfork_bytes = match extract_resource_bytes(&mut input_file) {
        Ok(Some(bytes)) => bytes,
        Ok(None) => {
            eprintln!("error: could not determine file format");
            return;
        }
        Err(e) => {
            eprintln!("error: input reading failed: {}", e);
            return;
        }
    };
    let resfork_cursor = Cursor::new(resfork_bytes);
    let resfork = match ResourceFork::read_from(resfork_cursor) {
        Ok(fork) => fork,
        Err(_) => {
            // try again with the raw file bytes
            let mut bytes = Vec::new();
            input_file.seek(SeekFrom::Start(0)).unwrap();
            input_file.read_to_end(&mut bytes).unwrap();
            match ResourceFork::read_from(Cursor::new(bytes)) {
                Ok(fork) => fork,
                Err(e) => {
                    eprintln!("error: could not parse resource fork: {}", e);
                    return;
                }
            }
        }
    };
    let result = match command.as_str() {
        "derez" => match output_file {
            Some(file) => derez_resfork(&resfork, file),
            None => {
                let stdout = io::stdout();
                let stdout_handle = stdout.lock();
                derez_resfork(&resfork, stdout_handle)
            }
        },
        "dump" => match output_file {
            Some(file) => dump_resfork(&resfork, file),
            None => {
                eprintln!("error: 'dump' command cannot be used with stdout");
                return;
            }
        },
        "convert" => match output_file {
            Some(file) => convert_resfork(&resfork, file),
            None => {
                eprintln!("error: 'convert' command cannot be used with stdout");
                return;
            }
        },
        _ => {
            eprintln!("error: unknown command");
            print_usage();
            return;
        }
    };
    if let Err(e) = result {
        eprintln!("error: output writing failed: {}", e);
        return;
    }
}

fn print_usage() {
    eprintln!("usage: resfork-extract <command> <input> [output]");
    eprintln!();
    eprintln!("    <command>  One of 'derez', 'dump', or 'convert'.");
    eprintln!("    <input>    Input file's name.");
    eprintln!("    <output>   Output file's name. Defaults to stdout if omitted.");
}
