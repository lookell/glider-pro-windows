mod apple_double;
mod mac_roman;
mod macbinary;
mod utils;

use crate::apple_double::AppleDouble;
use crate::macbinary::MacBinary;
use std::env;
use std::fs::File;
use std::io::{BufReader, Seek, SeekFrom};

fn main() {
    let filename = match env::args().nth(1) {
        Some(filename) => filename,
        None => {
            eprintln!("e: need a file name");
            return;
        }
    };
    let mut file = match File::open(filename) {
        Ok(file) => BufReader::new(file),
        Err(e) => {
            eprintln!("e: could not open file: {}", e);
            return;
        }
    };
    file.seek(SeekFrom::Start(0)).unwrap();
    match AppleDouble::read_from(&mut file) {
        Ok(Some(data)) => {
            println!("format: AppleSingle/AppleDouble");
            println!("data length = {}; {:?}", data.data.len(), &data.data[..8.min(data.data.len())]);
            println!("rsrc length = {}; {:?}", data.rsrc.len(), &data.rsrc[..8.min(data.rsrc.len())]);
            return;
        }
        Err(_) | Ok(None) => {}, // keep trying
    };
    file.seek(SeekFrom::Start(0)).unwrap();
    match MacBinary::read_from(&mut file) {
        Ok(Some(data)) => {
            println!("format: MacBinary");
            println!("data length = {}; {:?}", data.data.len(), &data.data[..8.min(data.data.len())]);
            println!("rsrc length = {}; {:?}", data.rsrc.len(), &data.rsrc[..8.min(data.rsrc.len())]);
            return;
        }
        Err(_) | Ok(None) => {}, // keep trying
    };
    eprintln!("e: could not determine file format");
}
