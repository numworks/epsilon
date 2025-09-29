use std::process::Command;

fn main() {
    // Turn icon.png into icon.nwi
    println!("cargo:rerun-if-changed=src/icon.png");
    let output = Command::new("sh")
        .arg("-c")
        .arg("npx --yes -- nwlink@0.0.19 png-nwi src/icon.png target/icon.nwi")
        .output()
        .expect("Icon creation failed.");
    assert!(
        output.status.success(),
        "{}",
        String::from_utf8_lossy(&output.stderr)
    );
}
