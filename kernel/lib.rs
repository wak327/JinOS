#![no_std]

use core::panic::PanicInfo;

const VGA_BUFFER: *mut u16 = 0xb8000 as *mut u16;
const GREEN_ON_BLACK: u16 = (0x02 << 8) | 0x00;
static mut CURSOR_OFFSET: usize = 80; // Start on the second row

#[no_mangle]
pub extern "C" fn rust_entry() {
    let message = b"[rust] subsystem online";
    unsafe {
        for (i, byte) in message.iter().enumerate() {
            let pos = CURSOR_OFFSET + i;
            VGA_BUFFER.add(pos).write_volatile(GREEN_ON_BLACK | (*byte as u16));
        }
        CURSOR_OFFSET += message.len();
    }
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {
        unsafe { core::arch::asm!("hlt"); }
    }
}
