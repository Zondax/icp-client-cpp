use candid::{
    bindings::{
        analysis::{chase_actor, infer_rec},
        rust::TypePath,
    },
    idl_hash,
    pretty::*,
    pretty_check_file,
    types::{Field, Function, Label, Type},
    Error, TypeEnv,
};
use pretty::RcDoc;

/// Check the given Candid IDL file and return the type declarations
pub fn check_candid_file(
    idl_path: impl AsRef<std::path::Path>,
) -> Result<(TypeEnv, Option<Type>), Error> {
    pretty_check_file(idl_path.as_ref())
}

/// Generate C++ bindings from the given Candid IDL file
pub fn generate(idl_path: impl AsRef<std::path::Path>) -> Result<String, Error> {
    check_candid_file(idl_path).map(|(env, ty)| compile(&env, &ty))
}

include!("gen.rs");
