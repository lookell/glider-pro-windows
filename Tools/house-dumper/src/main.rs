use std::fmt::{self, Debug, Formatter};
use std::io;
use std::io::prelude::*;

trait ReadExt: Read {
    /// Read a big-endian, 8-bit unsigned integer.
    fn read_be_u8(&mut self) -> io::Result<u8> {
        let mut buf = [0; 1];
        self.read_exact(&mut buf)?;
        Ok(u8::from_be_bytes(buf))
    }

    /// Read a big-endian, 8-bit signed integer.
    fn read_be_i8(&mut self) -> io::Result<i8> {
        let mut buf = [0; 1];
        self.read_exact(&mut buf)?;
        Ok(i8::from_be_bytes(buf))
    }

    /// Read a big-endian, 16-bit unsigned integer.
    fn read_be_u16(&mut self) -> io::Result<u16> {
        let mut buf = [0; 2];
        self.read_exact(&mut buf)?;
        Ok(u16::from_be_bytes(buf))
    }

    /// Read a big-endian, 16-bit signed integer.
    fn read_be_i16(&mut self) -> io::Result<i16> {
        let mut buf = [0; 2];
        self.read_exact(&mut buf)?;
        Ok(i16::from_be_bytes(buf))
    }

    /// Read a big-endian, 32-bit unsigned integer.
    fn read_be_u32(&mut self) -> io::Result<u32> {
        let mut buf = [0; 4];
        self.read_exact(&mut buf)?;
        Ok(u32::from_be_bytes(buf))
    }

    /// Read a big-endian, 32-bit signed integer.
    fn read_be_i32(&mut self) -> io::Result<i32> {
        let mut buf = [0; 4];
        self.read_exact(&mut buf)?;
        Ok(i32::from_be_bytes(buf))
    }
}

impl<R: Read + ?Sized> ReadExt for R {}

static MAC_ROMAN_CHARMAP: [char; 256] = {
    let mut map: [char; 256] = [0 as char; 256];
    let mut index = 0;
    while index < 128 {
        map[index] = index as u8 as char;
        index += 1;
    }
    map[0x80] = '\u{00C4}'; // LATIN CAPITAL LETTER A WITH DIAERESIS
    map[0x81] = '\u{00C5}'; // LATIN CAPITAL LETTER A WITH RING ABOVE
    map[0x82] = '\u{00C7}'; // LATIN CAPITAL LETTER C WITH CEDILLA
    map[0x83] = '\u{00C9}'; // LATIN CAPITAL LETTER E WITH ACUTE
    map[0x84] = '\u{00D1}'; // LATIN CAPITAL LETTER N WITH TILDE
    map[0x85] = '\u{00D6}'; // LATIN CAPITAL LETTER O WITH DIAERESIS
    map[0x86] = '\u{00DC}'; // LATIN CAPITAL LETTER U WITH DIAERESIS
    map[0x87] = '\u{00E1}'; // LATIN SMALL LETTER A WITH ACUTE
    map[0x88] = '\u{00E0}'; // LATIN SMALL LETTER A WITH GRAVE
    map[0x89] = '\u{00E2}'; // LATIN SMALL LETTER A WITH CIRCUMFLEX
    map[0x8A] = '\u{00E4}'; // LATIN SMALL LETTER A WITH DIAERESIS
    map[0x8B] = '\u{00E3}'; // LATIN SMALL LETTER A WITH TILDE
    map[0x8C] = '\u{00E5}'; // LATIN SMALL LETTER A WITH RING ABOVE
    map[0x8D] = '\u{00E7}'; // LATIN SMALL LETTER C WITH CEDILLA
    map[0x8E] = '\u{00E9}'; // LATIN SMALL LETTER E WITH ACUTE
    map[0x8F] = '\u{00E8}'; // LATIN SMALL LETTER E WITH GRAVE
    map[0x90] = '\u{00EA}'; // LATIN SMALL LETTER E WITH CIRCUMFLEX
    map[0x91] = '\u{00EB}'; // LATIN SMALL LETTER E WITH DIAERESIS
    map[0x92] = '\u{00ED}'; // LATIN SMALL LETTER I WITH ACUTE
    map[0x93] = '\u{00EC}'; // LATIN SMALL LETTER I WITH GRAVE
    map[0x94] = '\u{00EE}'; // LATIN SMALL LETTER I WITH CIRCUMFLEX
    map[0x95] = '\u{00EF}'; // LATIN SMALL LETTER I WITH DIAERESIS
    map[0x96] = '\u{00F1}'; // LATIN SMALL LETTER N WITH TILDE
    map[0x97] = '\u{00F3}'; // LATIN SMALL LETTER O WITH ACUTE
    map[0x98] = '\u{00F2}'; // LATIN SMALL LETTER O WITH GRAVE
    map[0x99] = '\u{00F4}'; // LATIN SMALL LETTER O WITH CIRCUMFLEX
    map[0x9A] = '\u{00F6}'; // LATIN SMALL LETTER O WITH DIAERESIS
    map[0x9B] = '\u{00F5}'; // LATIN SMALL LETTER O WITH TILDE
    map[0x9C] = '\u{00FA}'; // LATIN SMALL LETTER U WITH ACUTE
    map[0x9D] = '\u{00F9}'; // LATIN SMALL LETTER U WITH GRAVE
    map[0x9E] = '\u{00FB}'; // LATIN SMALL LETTER U WITH CIRCUMFLEX
    map[0x9F] = '\u{00FC}'; // LATIN SMALL LETTER U WITH DIAERESIS
    map[0xA0] = '\u{2020}'; // DAGGER
    map[0xA1] = '\u{00B0}'; // DEGREE SIGN
    map[0xA2] = '\u{00A2}'; // CENT SIGN
    map[0xA3] = '\u{00A3}'; // POUND SIGN
    map[0xA4] = '\u{00A7}'; // SECTION SIGN
    map[0xA5] = '\u{2022}'; // BULLET
    map[0xA6] = '\u{00B6}'; // PILCROW SIGN
    map[0xA7] = '\u{00DF}'; // LATIN SMALL LETTER SHARP S
    map[0xA8] = '\u{00AE}'; // REGISTERED SIGN
    map[0xA9] = '\u{00A9}'; // COPYRIGHT SIGN
    map[0xAA] = '\u{2122}'; // TRADE MARK SIGN
    map[0xAB] = '\u{00B4}'; // ACUTE ACCENT
    map[0xAC] = '\u{00A8}'; // DIAERESIS
    map[0xAD] = '\u{2260}'; // NOT EQUAL TO
    map[0xAE] = '\u{00C6}'; // LATIN CAPITAL LETTER AE
    map[0xAF] = '\u{00D8}'; // LATIN CAPITAL LETTER O WITH STROKE
    map[0xB0] = '\u{221E}'; // INFINITY
    map[0xB1] = '\u{00B1}'; // PLUS-MINUS SIGN
    map[0xB2] = '\u{2264}'; // LESS-THAN OR EQUAL TO
    map[0xB3] = '\u{2265}'; // GREATER-THAN OR EQUAL TO
    map[0xB4] = '\u{00A5}'; // YEN SIGN
    map[0xB5] = '\u{00B5}'; // MICRO SIGN
    map[0xB6] = '\u{2202}'; // PARTIAL DIFFERENTIAL
    map[0xB7] = '\u{2211}'; // N-ARY SUMMATION
    map[0xB8] = '\u{220F}'; // N-ARY PRODUCT
    map[0xB9] = '\u{03C0}'; // GREEK SMALL LETTER PI
    map[0xBA] = '\u{222B}'; // INTEGRAL
    map[0xBB] = '\u{00AA}'; // FEMININE ORDINAL INDICATOR
    map[0xBC] = '\u{00BA}'; // MASCULINE ORDINAL INDICATOR
    map[0xBD] = '\u{03A9}'; // GREEK CAPITAL LETTER OMEGA
    map[0xBE] = '\u{00E6}'; // LATIN SMALL LETTER AE
    map[0xBF] = '\u{00F8}'; // LATIN SMALL LETTER O WITH STROKE
    map[0xC0] = '\u{00BF}'; // INVERTED QUESTION MARK
    map[0xC1] = '\u{00A1}'; // INVERTED EXCLAMATION MARK
    map[0xC2] = '\u{00AC}'; // NOT SIGN
    map[0xC3] = '\u{221A}'; // SQUARE ROOT
    map[0xC4] = '\u{0192}'; // LATIN SMALL LETTER F WITH HOOK
    map[0xC5] = '\u{2248}'; // ALMOST EQUAL TO
    map[0xC6] = '\u{2206}'; // INCREMENT
    map[0xC7] = '\u{00AB}'; // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    map[0xC8] = '\u{00BB}'; // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
    map[0xC9] = '\u{2026}'; // HORIZONTAL ELLIPSIS
    map[0xCA] = '\u{00A0}'; // NO-BREAK SPACE
    map[0xCB] = '\u{00C0}'; // LATIN CAPITAL LETTER A WITH GRAVE
    map[0xCC] = '\u{00C3}'; // LATIN CAPITAL LETTER A WITH TILDE
    map[0xCD] = '\u{00D5}'; // LATIN CAPITAL LETTER O WITH TILDE
    map[0xCE] = '\u{0152}'; // LATIN CAPITAL LIGATURE OE
    map[0xCF] = '\u{0153}'; // LATIN SMALL LIGATURE OE
    map[0xD0] = '\u{2013}'; // EN DASH
    map[0xD1] = '\u{2014}'; // EM DASH
    map[0xD2] = '\u{201C}'; // LEFT DOUBLE QUOTATION MARK
    map[0xD3] = '\u{201D}'; // RIGHT DOUBLE QUOTATION MARK
    map[0xD4] = '\u{2018}'; // LEFT SINGLE QUOTATION MARK
    map[0xD5] = '\u{2019}'; // RIGHT SINGLE QUOTATION MARK
    map[0xD6] = '\u{00F7}'; // DIVISION SIGN
    map[0xD7] = '\u{25CA}'; // LOZENGE
    map[0xD8] = '\u{00FF}'; // LATIN SMALL LETTER Y WITH DIAERESIS
    map[0xD9] = '\u{0178}'; // LATIN CAPITAL LETTER Y WITH DIAERESIS
    map[0xDA] = '\u{2044}'; // FRACTION SLASH
    map[0xDB] = '\u{20AC}'; // EURO SIGN
    map[0xDC] = '\u{2039}'; // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
    map[0xDD] = '\u{203A}'; // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
    map[0xDE] = '\u{FB01}'; // LATIN SMALL LIGATURE FI
    map[0xDF] = '\u{FB02}'; // LATIN SMALL LIGATURE FL
    map[0xE0] = '\u{2021}'; // DOUBLE DAGGER
    map[0xE1] = '\u{00B7}'; // MIDDLE DOT
    map[0xE2] = '\u{201A}'; // SINGLE LOW-9 QUOTATION MARK
    map[0xE3] = '\u{201E}'; // DOUBLE LOW-9 QUOTATION MARK
    map[0xE4] = '\u{2030}'; // PER MILLE SIGN
    map[0xE5] = '\u{00C2}'; // LATIN CAPITAL LETTER A WITH CIRCUMFLEX
    map[0xE6] = '\u{00CA}'; // LATIN CAPITAL LETTER E WITH CIRCUMFLEX
    map[0xE7] = '\u{00C1}'; // LATIN CAPITAL LETTER A WITH ACUTE
    map[0xE8] = '\u{00CB}'; // LATIN CAPITAL LETTER E WITH DIAERESIS
    map[0xE9] = '\u{00C8}'; // LATIN CAPITAL LETTER E WITH GRAVE
    map[0xEA] = '\u{00CD}'; // LATIN CAPITAL LETTER I WITH ACUTE
    map[0xEB] = '\u{00CE}'; // LATIN CAPITAL LETTER I WITH CIRCUMFLEX
    map[0xEC] = '\u{00CF}'; // LATIN CAPITAL LETTER I WITH DIAERESIS
    map[0xED] = '\u{00CC}'; // LATIN CAPITAL LETTER I WITH GRAVE
    map[0xEE] = '\u{00D3}'; // LATIN CAPITAL LETTER O WITH ACUTE
    map[0xEF] = '\u{00D4}'; // LATIN CAPITAL LETTER O WITH CIRCUMFLEX
    map[0xF0] = '\u{F8FF}'; // Apple logo
    map[0xF1] = '\u{00D2}'; // LATIN CAPITAL LETTER O WITH GRAVE
    map[0xF2] = '\u{00DA}'; // LATIN CAPITAL LETTER U WITH ACUTE
    map[0xF3] = '\u{00DB}'; // LATIN CAPITAL LETTER U WITH CIRCUMFLEX
    map[0xF4] = '\u{00D9}'; // LATIN CAPITAL LETTER U WITH GRAVE
    map[0xF5] = '\u{0131}'; // LATIN SMALL LETTER DOTLESS I
    map[0xF6] = '\u{02C6}'; // MODIFIER LETTER CIRCUMFLEX ACCENT
    map[0xF7] = '\u{02DC}'; // SMALL TILDE
    map[0xF8] = '\u{00AF}'; // MACRON
    map[0xF9] = '\u{02D8}'; // BREVE
    map[0xFA] = '\u{02D9}'; // DOT ABOVE
    map[0xFB] = '\u{02DA}'; // RING ABOVE
    map[0xFC] = '\u{00B8}'; // CEDILLA
    map[0xFD] = '\u{02DD}'; // DOUBLE ACUTE ACCENT
    map[0xFE] = '\u{02DB}'; // OGONEK
    map[0xFF] = '\u{02C7}'; // CARON
    map
};

fn convert_mac_roman_to_string(pascal_string: &[u8]) -> io::Result<String> {
    if pascal_string.is_empty() {
        return Err(io::Error::new(
            io::ErrorKind::InvalidData,
            "Pascal string cannot be an empty array",
        ));
    }
    let length = usize::from(pascal_string[0]);
    let data = &pascal_string[1..];
    if length > data.len() {
        return Err(io::Error::new(
            io::ErrorKind::InvalidData,
            "Pascal string length byte exceeds available data",
        ));
    }
    let result = data[..length]
        .iter()
        .map(|&b| MAC_ROMAN_CHARMAP[usize::from(b)])
        .collect();
    Ok(result)
}

#[derive(Clone, Default)]
struct Str15 {
    pub value: String,
}

impl Debug for Str15 {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        Debug::fmt(&self.value, f)
    }
}

impl Str15 {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        let mut buf = [0; 16];
        reader.read_exact(&mut buf)?;
        Ok(Self {
            value: convert_mac_roman_to_string(&buf)?,
        })
    }
}

#[derive(Clone, Default)]
struct Str27 {
    pub value: String,
}

impl Debug for Str27 {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        Debug::fmt(&self.value, f)
    }
}

impl Str27 {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        let mut buf = [0; 28];
        reader.read_exact(&mut buf)?;
        Ok(Self {
            value: convert_mac_roman_to_string(&buf)?,
        })
    }
}

#[derive(Clone, Default)]
struct Str31 {
    pub value: String,
}

impl Debug for Str31 {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        Debug::fmt(&self.value, f)
    }
}

impl Str31 {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        let mut buf = [0; 32];
        reader.read_exact(&mut buf)?;
        Ok(Self {
            value: convert_mac_roman_to_string(&buf)?,
        })
    }
}

#[derive(Clone, Default)]
struct Str255 {
    pub value: String,
}

impl Debug for Str255 {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        Debug::fmt(&self.value, f)
    }
}

impl Str255 {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        let mut buf = [0; 256];
        reader.read_exact(&mut buf)?;
        Ok(Self {
            value: convert_mac_roman_to_string(&buf)?,
        })
    }
}

#[derive(Clone, Copy, Debug, Default)]
struct Point {
    pub v: i16,
    pub h: i16,
}

impl Point {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        Ok(Self {
            v: reader.read_be_i16()?,
            h: reader.read_be_i16()?,
        })
    }
}

#[derive(Clone, Copy, Debug, Default)]
struct Rect {
    pub top: i16,
    pub left: i16,
    pub bottom: i16,
    pub right: i16,
}

impl Rect {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        Ok(Self {
            top: reader.read_be_i16()?,
            left: reader.read_be_i16()?,
            bottom: reader.read_be_i16()?,
            right: reader.read_be_i16()?,
        })
    }
}

#[derive(Clone, Copy, Debug, Default)]
struct Blower {
    pub top_left: Point,
    pub distance: i16,
    pub initial: u8,
    pub state: u8,
    pub vector: u8,
    pub tall: u8,
}

impl Blower {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        Ok(Self {
            top_left: Point::read_from(reader)?,
            distance: reader.read_be_i16()?,
            initial: reader.read_be_u8()?,
            state: reader.read_be_u8()?,
            vector: reader.read_be_u8()?,
            tall: reader.read_be_u8()?,
        })
    }
}

#[derive(Clone, Copy, Debug, Default)]
struct Furniture {
    pub bounds: Rect,
    pub pict: i16,
}

impl Furniture {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        Ok(Self {
            bounds: Rect::read_from(reader)?,
            pict: reader.read_be_i16()?,
        })
    }
}

#[derive(Clone, Copy, Debug, Default)]
struct Bonus {
    pub top_left: Point,
    pub length: i16,
    pub points: i16,
    pub state: u8,
    pub initial: u8,
}

impl Bonus {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        Ok(Self {
            top_left: Point::read_from(reader)?,
            length: reader.read_be_i16()?,
            points: reader.read_be_i16()?,
            state: reader.read_be_u8()?,
            initial: reader.read_be_u8()?,
        })
    }
}

#[derive(Clone, Copy, Debug, Default)]
struct Transport {
    pub top_left: Point,
    pub tall: i16,
    pub where_: i16,
    pub who: u8,
    pub wide: u8,
}

impl Transport {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        Ok(Self {
            top_left: Point::read_from(reader)?,
            tall: reader.read_be_i16()?,
            where_: reader.read_be_i16()?,
            who: reader.read_be_u8()?,
            wide: reader.read_be_u8()?,
        })
    }
}

#[derive(Clone, Copy, Debug, Default)]
struct Switch {
    pub top_left: Point,
    pub delay: i16,
    pub where_: i16,
    pub who: u8,
    pub type_: u8,
}

impl Switch {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        Ok(Self {
            top_left: Point::read_from(reader)?,
            delay: reader.read_be_i16()?,
            where_: reader.read_be_i16()?,
            who: reader.read_be_u8()?,
            type_: reader.read_be_u8()?,
        })
    }
}

#[derive(Clone, Copy, Debug, Default)]
struct Light {
    pub top_left: Point,
    pub length: i16,
    pub byte0: u8,
    pub byte1: u8,
    pub initial: u8,
    pub state: u8,
}

impl Light {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        Ok(Self {
            top_left: Point::read_from(reader)?,
            length: reader.read_be_i16()?,
            byte0: reader.read_be_u8()?,
            byte1: reader.read_be_u8()?,
            initial: reader.read_be_u8()?,
            state: reader.read_be_u8()?,
        })
    }
}

#[derive(Clone, Copy, Debug, Default)]
struct Appliance {
    pub top_left: Point,
    pub height: i16,
    pub byte0: u8,
    pub delay: u8,
    pub initial: u8,
    pub state: u8,
}

impl Appliance {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        Ok(Self {
            top_left: Point::read_from(reader)?,
            height: reader.read_be_i16()?,
            byte0: reader.read_be_u8()?,
            delay: reader.read_be_u8()?,
            initial: reader.read_be_u8()?,
            state: reader.read_be_u8()?,
        })
    }
}

#[derive(Clone, Copy, Debug, Default)]
struct Enemy {
    pub top_left: Point,
    pub length: i16,
    pub delay: u8,
    pub byte0: u8,
    pub initial: u8,
    pub state: u8,
}

impl Enemy {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        Ok(Self {
            top_left: Point::read_from(reader)?,
            length: reader.read_be_i16()?,
            delay: reader.read_be_u8()?,
            byte0: reader.read_be_u8()?,
            initial: reader.read_be_u8()?,
            state: reader.read_be_u8()?,
        })
    }
}

#[derive(Clone, Copy, Debug, Default)]
struct Clutter {
    pub bounds: Rect,
    pub pict: i16,
}

impl Clutter {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        Ok(Self {
            bounds: Rect::read_from(reader)?,
            pict: reader.read_be_i16()?,
        })
    }
}

#[derive(Clone, Copy, Debug)]
enum ObjectInfo {
    Empty,
    Blower(Blower),
    Furniture(Furniture),
    Bonus(Bonus),
    Transport(Transport),
    Switch(Switch),
    Light(Light),
    Appliance(Appliance),
    Enemy(Enemy),
    Clutter(Clutter),
    Unknown([u8; 10]),
}

impl Default for ObjectInfo {
    fn default() -> Self {
        Self::Unknown([0; 10])
    }
}

impl From<Blower> for ObjectInfo {
    fn from(inner: Blower) -> Self {
        Self::Blower(inner)
    }
}

impl From<Furniture> for ObjectInfo {
    fn from(inner: Furniture) -> Self {
        Self::Furniture(inner)
    }
}

impl From<Bonus> for ObjectInfo {
    fn from(inner: Bonus) -> Self {
        Self::Bonus(inner)
    }
}

impl From<Transport> for ObjectInfo {
    fn from(inner: Transport) -> Self {
        Self::Transport(inner)
    }
}

impl From<Switch> for ObjectInfo {
    fn from(inner: Switch) -> Self {
        Self::Switch(inner)
    }
}

impl From<Light> for ObjectInfo {
    fn from(inner: Light) -> Self {
        Self::Light(inner)
    }
}

impl From<Appliance> for ObjectInfo {
    fn from(inner: Appliance) -> Self {
        Self::Appliance(inner)
    }
}

impl From<Enemy> for ObjectInfo {
    fn from(inner: Enemy) -> Self {
        Self::Enemy(inner)
    }
}

impl From<Clutter> for ObjectInfo {
    fn from(inner: Clutter) -> Self {
        Self::Clutter(inner)
    }
}

#[derive(Clone, Copy, Debug, Default)]
struct Object {
    pub what: i16,
    pub data: ObjectInfo,
}

impl Object {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        let what = reader.read_be_i16()?;
        let data = match what {
            -1 => {
                let mut ignored = [0; 10];
                reader.read_exact(&mut ignored)?;
                ObjectInfo::Empty
            }
            0x01..=0x10 => ObjectInfo::from(Blower::read_from(reader)?),
            0x11..=0x20 => ObjectInfo::from(Furniture::read_from(reader)?),
            0x21..=0x30 => ObjectInfo::from(Bonus::read_from(reader)?),
            0x31..=0x40 => ObjectInfo::from(Transport::read_from(reader)?),
            0x41..=0x50 => ObjectInfo::from(Switch::read_from(reader)?),
            0x51..=0x60 => ObjectInfo::from(Light::read_from(reader)?),
            0x61..=0x70 => ObjectInfo::from(Appliance::read_from(reader)?),
            0x71..=0x80 => ObjectInfo::from(Enemy::read_from(reader)?),
            0x81..=0x90 => ObjectInfo::from(Clutter::read_from(reader)?),
            _ => {
                let mut buf = [0; 10];
                reader.read_exact(&mut buf)?;
                ObjectInfo::Unknown(buf)
            }
        };
        Ok(Self { what, data })
    }
}

const MAX_SCORES: usize = 10;

#[derive(Clone, Debug, Default)]
struct Scores {
    pub banner: Str31,
    pub names: [Str15; MAX_SCORES],
    pub scores: [i32; MAX_SCORES],
    pub time_stamps: [u32; MAX_SCORES],
    pub levels: [i16; MAX_SCORES],
}

impl Scores {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        Ok(Self {
            banner: Str31::read_from(reader)?,
            names: {
                let mut result: [Str15; MAX_SCORES] = Default::default();
                for entry in &mut result {
                    *entry = Str15::read_from(reader)?;
                }
                result
            },
            scores: {
                let mut result: [i32; MAX_SCORES] = Default::default();
                for entry in &mut result {
                    *entry = reader.read_be_i32()?;
                }
                result
            },
            time_stamps: {
                let mut result: [u32; MAX_SCORES] = Default::default();
                for entry in &mut result {
                    *entry = reader.read_be_u32()?;
                }
                result
            },
            levels: {
                let mut result: [i16; MAX_SCORES] = Default::default();
                for entry in &mut result {
                    *entry = reader.read_be_i16()?;
                }
                result
            },
        })
    }
}

#[derive(Clone, Copy, Debug, Default)]
struct Game {
    pub version: i16,
    pub was_stars_left: i16,
    pub time_stamp: i32,
    pub where_: Point,
    pub score: i32,
    pub unused_long: i32,
    pub unused_long2: i32,
    pub energy: i16,
    pub bands: i16,
    pub room_number: i16,
    pub glider_state: i16,
    pub num_gliders: i16,
    pub foil: i16,
    pub unused_short: i16,
    pub facing: u8,
    pub show_foil: u8,
}

impl Game {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        Ok(Self {
            version: reader.read_be_i16()?,
            was_stars_left: reader.read_be_i16()?,
            time_stamp: reader.read_be_i32()?,
            where_: Point::read_from(reader)?,
            score: reader.read_be_i32()?,
            unused_long: reader.read_be_i32()?,
            unused_long2: reader.read_be_i32()?,
            energy: reader.read_be_i16()?,
            bands: reader.read_be_i16()?,
            room_number: reader.read_be_i16()?,
            glider_state: reader.read_be_i16()?,
            num_gliders: reader.read_be_i16()?,
            foil: reader.read_be_i16()?,
            unused_short: reader.read_be_i16()?,
            facing: reader.read_be_u8()?,
            show_foil: reader.read_be_u8()?,
        })
    }
}

const NUM_TILES: usize = 8;
const MAX_ROOM_OBJECTS: usize = 24;

#[derive(Clone, Debug, Default)]
struct Room {
    pub name: Str27,
    pub bounds: i16,
    pub left_start: u8,
    pub right_start: u8,
    pub unused_byte: u8,
    pub visited: u8,
    pub background: i16,
    pub tiles: [i16; NUM_TILES],
    pub floor: i16,
    pub suite: i16,
    pub openings: i16,
    pub num_objects: i16,
    pub objects: [Object; MAX_ROOM_OBJECTS],
}

impl Room {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        Ok(Self {
            name: Str27::read_from(reader)?,
            bounds: reader.read_be_i16()?,
            left_start: reader.read_be_u8()?,
            right_start: reader.read_be_u8()?,
            unused_byte: reader.read_be_u8()?,
            visited: reader.read_be_u8()?,
            background: reader.read_be_i16()?,
            tiles: {
                let mut result: [i16; NUM_TILES] = Default::default();
                for entry in &mut result {
                    *entry = reader.read_be_i16()?;
                }
                result
            },
            floor: reader.read_be_i16()?,
            suite: reader.read_be_i16()?,
            openings: reader.read_be_i16()?,
            num_objects: reader.read_be_i16()?,
            objects: {
                let mut result: [Object; MAX_ROOM_OBJECTS] = Default::default();
                for entry in &mut result {
                    *entry = Object::read_from(reader)?;
                }
                result
            },
        })
    }
}

#[derive(Clone, Debug, Default)]
struct House {
    pub version: i16,
    pub unused_short: i16,
    pub time_stamp: i32,
    pub flags: i32,
    pub initial: Point,
    pub banner: Str255,
    pub trailer: Str255,
    pub high_scores: Scores,
    pub saved_game: Game,
    pub has_game: u8,
    pub unused_boolean: u8,
    pub first_room: i16,
    pub num_rooms: i16,
    pub rooms: Vec<Room>,
}

impl House {
    fn read_from(reader: &mut dyn Read) -> io::Result<Self> {
        let mut myself = Self {
            version: reader.read_be_i16()?,
            unused_short: reader.read_be_i16()?,
            time_stamp: reader.read_be_i32()?,
            flags: reader.read_be_i32()?,
            initial: Point::read_from(reader)?,
            banner: Str255::read_from(reader)?,
            trailer: Str255::read_from(reader)?,
            high_scores: Scores::read_from(reader)?,
            saved_game: Game::read_from(reader)?,
            has_game: reader.read_be_u8()?,
            unused_boolean: reader.read_be_u8()?,
            first_room: reader.read_be_i16()?,
            num_rooms: reader.read_be_i16()?,
            rooms: Vec::new(),
        };
        let rooms_to_read = myself.num_rooms.max(0) as usize;
        myself.rooms.reserve(rooms_to_read);
        for _ in 0..rooms_to_read {
            myself.rooms.push(Room::read_from(reader)?);
        }
        Ok(myself)
    }
}

fn main() {
    let mut house_data: &[u8] = include_bytes!("../../../Houses/Slumberland");
    let house = House::read_from(&mut house_data).unwrap();
    println!("{:#?}", &house);
}
