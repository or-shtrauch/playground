use std::env;
use std::fs;
use std::io;
use std::io::BufRead;
use std::path::Path;

struct WCData {
    file_reader: io::BufReader<fs::File>,
    pub file_name: String,
    pub words: i128,
    pub lines: i128,
    pub chars: i128,
}

impl WCData {
    fn new<P: AsRef<Path>>(file_path: P) -> io::Result<WCData> {
        let path = file_path.as_ref();
        let file_name = path.to_string_lossy().to_string();
        let file_handler = fs::File::open(&file_name)?;
        let file_reader = io::BufReader::new(file_handler);

        Ok(WCData {
            file_reader,
            file_name,
            words: 0,
            lines: 0,
            chars: 0,
        })
    }

    fn count(&mut self) -> io::Result<()> {
        let mut bytes_size: usize;
        let mut line_str = String::new();

        loop {
            bytes_size = self.file_reader.read_line(&mut line_str)?;

            if bytes_size == 0 {
                break; /* EOF */
            }

            self.lines += 1;
            self.chars += bytes_size as i128;
            self.words += line_str.split_whitespace().count() as i128;

            line_str.clear();
        }

        Ok(())
    }
}

fn main() -> io::Result<()> {
    let file_name = env::args()
        .into_iter()
        .nth(1)
        .expect("no file name provided");

    let mut data = WCData::new(&file_name)?;
    data.count()?;

    println!(
        "file name: {}\nlines: {} | words: {} | chars: {}",
        data.file_name, data.lines, data.words, data.chars
    );

    Ok(())
}
