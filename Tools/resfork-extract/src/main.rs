mod apple_double;
mod mac_roman;
mod macbinary;
mod res;
mod rsrcfork;
mod utils;

use crate::apple_double::AppleDouble;
use crate::macbinary::MacBinary;
use crate::rsrcfork::{Resource, ResourceFork};
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
    for res in resfork.resources.iter() {
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
    for res in resfork.resources.iter() {
        if let Some(name) = &res.name {
            writeln!(&mut writer, "('{}', {}): {:?}", res.restype, res.id, name)?;
        }
    }
    Ok(())
}

fn dump_resfork(resfork: &ResourceFork, writer: impl Seek + Write) -> AnyResult<()> {
    let mut zip_writer = ZipWriter::new(writer);
    zip_writer.set_comment("");

    if resfork.resources.iter().any(|res| res.name.is_some()) {
        zip_writer.start_file("names.txt", Default::default())?;
        write_names_txt(resfork, &mut zip_writer)?;
    }

    for res in resfork.resources.iter() {
        let entry_name = make_zip_entry_path(&res);
        zip_writer.start_file(entry_name, Default::default())?;
        zip_writer.write_all(res.data.as_slice())?;
    }
    Ok(())
}

// TODO:
//   'ALRT': Alert
//   'BNDL': Bundle
//   'cicn': Color Icon
//   'crsr': Color Cursor
//   'CURS': Cursor
//   'DITL': Item List
//   'FREF': File Reference
//   'icl4': Large 4-Bit Color Icon
//   'icl8': Large 8-Bit Color Icon
//   'ICN#': Icon List
//   'ICON': Icon
//   'ics#': Small Icon List
//   'ics4': Small 4-Bit Color Icon
//   'ics8': Small 8-Bit Color Icon
//   'ictb': Item Color Table
//   'mctb': Menu Color Information Table
//   'MENU': Menu
//   'ozm5': Glider Pro Copyright String
//   'PAT#': Pattern List
//   'PICT': Picture
//   'snd ': Sound
//   'vers': Version
//   'WIND': Window

fn convert_resfork(resfork: &ResourceFork, writer: impl Seek + Write) -> AnyResult<()> {
    let mut zip_writer = ZipWriter::new(writer);
    zip_writer.set_comment("");

    if resfork.resources.iter().any(|res| res.name.is_some()) {
        zip_writer.start_file("names.txt", Default::default())?;
        write_names_txt(resfork, &mut zip_writer)?;
    }

    for res in resfork.resources.iter() {
        match res.restype.to_string().as_str() {
            "acur" => {
                let entry_name = res::animated_cursor::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::animated_cursor::convert(&res.data, &mut zip_writer)?;
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
            "DLOG" => {
                let entry_name = res::dialog::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::dialog::convert(&res.data, &mut zip_writer)?;
            }
            "STR#" => {
                let entry_name = res::string_list::get_entry_name(&res);
                zip_writer.start_file(entry_name, Default::default())?;
                res::string_list::convert(&res.data, &mut zip_writer)?;
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
    let input_file = match File::open(input_name) {
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
    let resfork_bytes = match extract_resource_bytes(input_file) {
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
        Err(e) => {
            eprintln!("error: could not parse resource fork: {}", e);
            return;
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
