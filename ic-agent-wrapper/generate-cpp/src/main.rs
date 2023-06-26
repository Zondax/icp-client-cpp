use std::env;
use std::fs;
use std::io;
use std::path::Path;

fn ensure_trailing_newline(mut content: String) -> String {
    if !content.ends_with('\n') {
        content.push('\n');
    }
    content
}

fn main() -> io::Result<()> {
    // Parse command line arguments
    let args: Vec<String> = env::args().collect();
    if args.len() < 3 {
        eprintln!("Usage: {} <candid file> <canister name>", args[0]);
        return Ok(());
    }
    let idl_path = Path::new(&args[1]);
    let name = &args[2];

    // Get the parent directory of the generated_idl_path
    let parent_dir = match idl_path.parent() {
        Some(dir) => dir,
        None => {
            eprintln!("Invalid path: {}", idl_path.display());
            return Ok(());
        }
    };

    // Prepare output path
    let output_did_cpp_path = parent_dir
        .join("src")
        .join("declarations")
        .join(name)
        .join(format!("{}.hpp", name));

    // Generate C++ content
    let content = match generate_cpp::generate(idl_path) {
        Ok(result) => ensure_trailing_newline(result),
        Err(err) => {
            eprintln!("Error generating C++ bindings file: {}", err);
            return Ok(());
        }
    };

    // Write content to output path
    fs::write(&output_did_cpp_path, content).map_err(|err| {
        io::Error::new(
            err.kind(),
            format!("Failed to write to {}.", output_did_cpp_path.display()),
        )
    })?;

    eprintln!(
        "Wrote bindings to {}",
        output_did_cpp_path.canonicalize()?.display()
    );
    Ok(())
}
