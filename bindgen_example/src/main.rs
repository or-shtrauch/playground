#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

pub struct Cool {
    cool : cool_t
}

impl Cool {
    fn new(a: i32, b: i32) -> Cool {
        Cool {
            cool : cool_t {
                a : a,
                b : b
            }
        }
    }
    
}

pub fn c_cool_add(cool :&cool_t) -> i32 {
    unsafe {
        cool_add(cool as *const cool_t)
    }
}

pub fn c_cool_sub(cool :&cool_t) -> i32 {
    unsafe {
        cool_sub(cool as *const cool_t)
    }
}

fn main() {
    let cool = Cool::new(10, 20);
    let result = c_cool_add(&cool.cool);
    println!("Result: {}", result);
    let result = c_cool_sub(&cool.cool);
    println!("Result: {}", result);
}