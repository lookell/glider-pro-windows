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
use std::io::{BufReader, Cursor, Seek, SeekFrom};

fn deal_with_resource_fork(rsrc_bytes: &[u8]) {
    let cursor = Cursor::new(rsrc_bytes);
    let resfork = match ResourceFork::read_from(cursor) {
        Ok(fork) => fork,
        Err(e) => {
            eprintln!("e: could not parse resource fork: {}", e);
            return;
        }
    };
    for res in resfork.resources.iter() {
        println!("'{}' ({}) [{} bytes];", res.restype, res.id, res.data.len());
    }
}

fn main() {
    let filename = match env::args().nth(1) {
        Some(filename) => filename,
        None => {
            eprintln!("e: need a file name");
            return;
        }
    };
    let file = match File::open(filename) {
        Ok(file) => file,
        Err(e) => {
            eprintln!("e: could not open file: {}", e);
            return;
        }
    };
    let mut reader = BufReader::new(file);

    reader.seek(SeekFrom::Start(0)).unwrap();
    match AppleDouble::read_from(&mut reader) {
        Ok(Some(data)) => {
            println!("format: AppleSingle/AppleDouble");
            deal_with_resource_fork(data.rsrc.as_slice());
            return;
        }
        Err(_) | Ok(None) => {} // keep trying
    };

    reader.seek(SeekFrom::Start(0)).unwrap();
    match MacBinary::read_from(&mut reader) {
        Ok(Some(data)) => {
            println!("format: MacBinary");
            deal_with_resource_fork(data.rsrc.as_slice());
            return;
        }
        Err(_) | Ok(None) => {} // keep trying
    };

    eprintln!("e: could not determine file format");
}
