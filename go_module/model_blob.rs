// Embedded placeholder model bytes for testing purposes.
// This is a small byte array to simulate an embedded model or binary payload.
pub static MODEL_BLOB: &'static [u8] = &[
    0x52, 0x75, 0x73, 0x74, 0x4D, 0x6F, 0x64, 0x65, // "RustMode"
    0x00, 0x01, 0x02, 0xFF, 0x10, 0x20, 0x30,
];

// Helper: length of the embedded blob
pub fn model_blob_len() -> usize {
    MODEL_BLOB.len()
}
