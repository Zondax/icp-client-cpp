extern crate cbindgen;

use std::env;

use cbindgen::{Builder, Config, Language};

fn main() {
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let config_c = Config { language: Language::C, .. Config::default() };

    Builder::new()
          .with_crate(&crate_dir)
          .with_config(config_c)
          .with_parse_deps(true)
          .with_pragma_once(true)
          .with_parse_include(&["ic-agent"])
          .generate()
          .expect("Unable to generate bindings")
          .write_to_file("bindings.h");
}
