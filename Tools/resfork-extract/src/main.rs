mod apple_double;
mod mac_roman;
mod utils;

use crate::apple_double::AppleDouble;
use std::env;
use std::fs::File;

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
    let apple_data = match AppleDouble::read_from(file) {
        Ok(Some(data)) => data,
        Ok(None) => {
            eprintln!("e: could not read file as AppleSingle or AppleDouble");
            return;
        }
        Err(e) => {
            eprintln!("e: could not read file: {}", e);
            return;
        }
    };
    println!("data length = {}", apple_data.data.len());
    println!("rsrc length = {}", apple_data.rsrc.len());
}
