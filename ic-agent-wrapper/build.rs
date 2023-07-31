extern crate cbindgen;

use std::env;

use cbindgen::{Builder, Config, Language};

fn main() {
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let config_c = Config {
        language: Language::C,
        ..Config::default()
    };

    println!("cargo:rerun-if-changed=src/");

    Builder::new()
          .with_crate(&crate_dir)
          .with_config(config_c)
          .with_parse_deps(true)
          .with_pragma_once(true)
          .with_parse_include(&["ic-agent"])
          .with_after_include("#ifdef __cplusplus\nextern \"C\"{\n#endif\n\ntypedef struct IDLValue IDLValue;\ntypedef struct IDLArgs IDLArgs;")
          .with_trailer("#ifdef __cplusplus\n}\n#endif")
          .generate()
          .expect("Unable to generate bindings")
          .write_to_file("bindings.h");
}
