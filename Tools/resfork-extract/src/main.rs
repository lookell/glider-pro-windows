mod apple_double;
mod mac_roman;
mod macbinary;
mod rsrcfork;
mod utils;

use crate::apple_double::AppleDouble;
use crate::macbinary::MacBinary;
use crate::rsrcfork::ResourceFork;
use std::env;
use std::fs::File;
use std::io::{self, BufReader, BufWriter, Cursor, Read, Seek, SeekFrom, Write};

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

fn derez_resfork(resfork: ResourceFork, mut writer: impl Write) -> io::Result<()> {
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
        writeln!(&mut writer, "}};")?;
        writeln!(&mut writer, "")?;
    }
    Ok(())
}

fn dump_resfork(_resfork: ResourceFork, _writer: impl Write) -> io::Result<()> {
    unimplemented!("dump_resfork");
}

fn convert_resfork(_resfork: ResourceFork, _writer: impl Write) -> io::Result<()> {
    unimplemented!("convert_resfork");
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
    let stdout_handle = io::stdout();
    let output_file: Box<dyn Write> = match output_name {
        Some(name) => match File::create(name) {
            Ok(file) => Box::new(BufWriter::new(file)),
            Err(e) => {
                eprintln!("error: could not open output file: {}", e);
                return;
            }
        }
        None => Box::new(BufWriter::new(stdout_handle.lock())),
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
        "derez" => derez_resfork(resfork, output_file),
        "dump" => dump_resfork(resfork, output_file),
        "convert" => convert_resfork(resfork, output_file),
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
