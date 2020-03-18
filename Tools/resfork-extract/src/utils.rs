use std::io;
use std::io::{Read, Seek, SeekFrom, Write};

fn read_one_byte(mut reader: impl Read) -> io::Result<[u8; 1]> {
    let mut buf = [0; 1];
    reader.read_exact(&mut buf).map(|_| buf)
}

fn read_two_bytes(mut reader: impl Read) -> io::Result<[u8; 2]> {
    let mut buf = [0; 2];
    reader.read_exact(&mut buf).map(|_| buf)
}

fn read_four_bytes(mut reader: impl Read) -> io::Result<[u8; 4]> {
    let mut buf = [0; 4];
    reader.read_exact(&mut buf).map(|_| buf)
}

impl<R: Read> ReadExt for R {}

pub(crate) trait ReadExt: Read {
    fn read_be_u8(&mut self) -> io::Result<u8> {
        read_one_byte(self).map(u8::from_be_bytes)
    }

    fn read_le_u8(&mut self) -> io::Result<u8> {
        read_one_byte(self).map(u8::from_le_bytes)
    }

    fn read_be_i8(&mut self) -> io::Result<i8> {
        read_one_byte(self).map(i8::from_be_bytes)
    }

    fn read_le_i8(&mut self) -> io::Result<i8> {
        read_one_byte(self).map(i8::from_le_bytes)
    }

    fn read_be_u16(&mut self) -> io::Result<u16> {
        read_two_bytes(self).map(u16::from_be_bytes)
    }

    fn read_le_u16(&mut self) -> io::Result<u16> {
        read_two_bytes(self).map(u16::from_le_bytes)
    }

    fn read_be_i16(&mut self) -> io::Result<i16> {
        read_two_bytes(self).map(i16::from_be_bytes)
    }

    fn read_le_i16(&mut self) -> io::Result<i16> {
        read_two_bytes(self).map(i16::from_le_bytes)
    }

    fn read_be_u32(&mut self) -> io::Result<u32> {
        read_four_bytes(self).map(u32::from_be_bytes)
    }

    fn read_le_u32(&mut self) -> io::Result<u32> {
        read_four_bytes(self).map(u32::from_le_bytes)
    }

    fn read_be_i32(&mut self) -> io::Result<i32> {
        read_four_bytes(self).map(i32::from_be_bytes)
    }

    fn read_le_i32(&mut self) -> io::Result<i32> {
        read_four_bytes(self).map(i32::from_le_bytes)
    }
}

impl<W: Write> WriteExt for W {}

pub(crate) trait WriteExt: Write {
    fn write_be_u8(&mut self, v: u8) -> io::Result<()> {
        self.write_all(&v.to_be_bytes())
    }

    fn write_le_u8(&mut self, v: u8) -> io::Result<()> {
        self.write_all(&v.to_le_bytes())
    }

    fn write_be_i8(&mut self, v: i8) -> io::Result<()> {
        self.write_all(&v.to_be_bytes())
    }

    fn write_le_i8(&mut self, v: i8) -> io::Result<()> {
        self.write_all(&v.to_le_bytes())
    }

    fn write_be_u16(&mut self, v: u16) -> io::Result<()> {
        self.write_all(&v.to_be_bytes())
    }

    fn write_le_u16(&mut self, v: u16) -> io::Result<()> {
        self.write_all(&v.to_le_bytes())
    }

    fn write_be_i16(&mut self, v: i16) -> io::Result<()> {
        self.write_all(&v.to_be_bytes())
    }

    fn write_le_i16(&mut self, v: i16) -> io::Result<()> {
        self.write_all(&v.to_le_bytes())
    }

    fn write_be_u32(&mut self, v: u32) -> io::Result<()> {
        self.write_all(&v.to_be_bytes())
    }

    fn write_le_u32(&mut self, v: u32) -> io::Result<()> {
        self.write_all(&v.to_le_bytes())
    }

    fn write_be_i32(&mut self, v: i32) -> io::Result<()> {
        self.write_all(&v.to_be_bytes())
    }

    fn write_le_i32(&mut self, v: i32) -> io::Result<()> {
        self.write_all(&v.to_le_bytes())
    }
}

impl<S: Seek> SeekExt for S {}

pub(crate) trait SeekExt: Seek {
    fn align_to(&mut self, align: u64) -> io::Result<u64> {
        let cur_pos = self.seek(SeekFrom::Current(0))?;
        let offset = align - (cur_pos % align);
        if offset == align {
            return Ok(cur_pos);
        }
        self.seek(SeekFrom::Current(offset as _))
    }
}
