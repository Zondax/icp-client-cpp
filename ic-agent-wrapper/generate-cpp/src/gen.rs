use std::collections::BTreeSet;

/// Compile the given type declarations into C++ bindings
pub fn compile(env: &TypeEnv, actor: &Option<Type>) -> String {
    let header = r#"// This is an experimental feature to generate C++ bindings from Candid.
// You may want to manually adjust some of the types.

#pragma once

#include <optional>
#include <stdint.h>
#include <string>
#include <variant>
#include <vector>

//lib-agent-cpp
#include "agent.h"
#include "func.h"
#include "idl_value.h"
#include "principal.h"
"#;

    let (env, actor) = nominalize_all(env, actor);
    let def_list: Vec<_> = if let Some(actor) = &actor {
        chase_actor(&env, actor).unwrap()
    } else {
        env.0.iter().map(|pair| pair.0.as_ref()).collect()
    };

    let recs = infer_rec(&env, &def_list).unwrap();
    let defs = pp_defs(&env, &def_list, &recs);
    let doc = match &actor {
        None => defs,
        Some(actor) => {
            let actor = pp_actor(&env, actor);
            defs.append(actor)
        }
    };
    let doc = RcDoc::text(header).append(RcDoc::line()).append(doc);
    doc.pretty(LINE_WIDTH).to_string()
}
static KEYWORDS: [&str; 127] = [
    "alignas",
    "alignof",
    "and",
    "and_eq",
    "asm",
    "atomic_cancel",
    "atomic_commit",
    "atomic_noexcept",
    "auto",
    "bitand",
    "bitor",
    "bool",
    "break",
    "case",
    "catch",
    "char",
    "char8_t",
    "char16_t",
    "char32_t",
    "class",
    "compl",
    "concept",
    "const",
    "consteval",
    "constexpr",
    "constinit",
    "const_cast",
    "continue",
    "co_await",
    "co_return",
    "co_yield",
    "decltype",
    "default",
    "delete",
    "do",
    "double",
    "dynamic_cast",
    "else",
    "enum",
    "explicit",
    "export",
    "extern",
    "false",
    "float",
    "for",
    "friend",
    "goto",
    "if",
    "inline",
    "int",
    "long",
    "mutable",
    "namespace",
    "new",
    "noexcept",
    "not",
    "not_eq",
    "nullptr",
    "operator",
    "or",
    "or_eq",
    "private",
    "protected",
    "public",
    "reflexpr",
    "register",
    "reinterpret_cast",
    "requires",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "static_assert",
    "static_cast",
    "struct",
    "switch",
    "synchronized",
    "template",
    "this",
    "thread_local",
    "throw",
    "true",
    "try",
    "typedef",
    "typeid",
    "typename",
    "union",
    "unsigned",
    "using",
    "virtual",
    "void",
    "volatile",
    "wchar_t",
    "while",
    "xor",
    "xor_eq",
    "final",
    "override",
    "transaction_safe",
    "transaction_safe_dynamic",
    "import",
    "module",
    "posix",
    "if",
    "elif",
    "else",
    "endif",
    "ifdef",
    "ifndef",
    "elifdef",
    "elifndef",
    "define",
    "undef",
    "include",
    "line",
    "error",
    "warning",
    "pragma",
    "defined",
    "__has_include",
    "__has_cpp_attribute",
    "export",
    "import",
    "module",
    "_Pragma",
    "std",
];

fn ident_(id: &str) -> (String, bool) {
    if id.is_empty()
        || id.starts_with(|c: char| !c.is_ascii_alphabetic() && c != '_')
        || id.chars().any(|c| !c.is_ascii_alphanumeric() && c != '_')
        //reserved
        || id.contains("__")
        || KEYWORDS.contains(&id)
        //global namespace reserved items
        || id
            .as_bytes()
            .windows(2)
            .take(1) //only check the start of the identifier
            .any(|id| id[0] == b'_' && id[1].is_ascii_uppercase())
    {
        (format!("_{}_", idl_hash(id)), true)
    } else {
        (id.to_string(), false)
    }
}
fn ident(id: &str) -> RcDoc {
    RcDoc::text(ident_(id).0)
}

type RecPoints<'a> = BTreeSet<&'a str>;

fn pp_label(id: &Label) -> RcDoc {
    match id {
        Label::Named(str) => ident(str),
        Label::Id(n) | Label::Unnamed(n) => str("_").append(RcDoc::as_string(n)).append("_"),
    }
}

fn pp_ty<'a, 'b>(ty: &'a Type, recs: &'b RecPoints) -> RcDoc<'a> {
    use Type::*;
    match *ty {
        Null => str("void"),
        Bool => str("bool"),
        Nat => str("zondax::Number"),
        Int => str("zondax::Number"),
        Nat8 => str("uint8_t"),
        Nat16 => str("uint16_t"),
        Nat32 => str("uint32_t"),
        Nat64 => str("uint64_t"),
        Int8 => str("int8_t"),
        Int16 => str("int16_t"),
        Int32 => str("int32_t"),
        Int64 => str("int64_t"),
        Float32 => str("float"),
        Float64 => str("double"),
        Text => str("std::string"),
        Reserved => str("void* /* candid::Reserved */"),
        Empty => str("void* /* candid::Empty */"),
        Var(ref id) => {
            let name = ident(id);
            if recs.contains(id.as_str()) {
                str("std::unique_ptr<").append(name).append(">")
            } else {
                name
            }
        }
        Principal => str("zondax::Principal"),
        Opt(ref t) => str("std::optional").append(enclose("<", pp_ty(t, recs), ">")),
        Vec(ref t) => str("std::vector").append(enclose("<", pp_ty(t, recs), ">")),
        Record(ref fs) => pp_record_fields(fs, recs),
        Variant(_) => unreachable!(), // not possible after rewriting
        Func(_) => str("zondax::Func"),
        Service(_) => str("zondax::Service"),
        Class(_, _) => unreachable!(),
        Knot(_) | Unknown => unreachable!(),
    }
}

fn pp_record_field<'a, 'b>(field: &'a Field, recs: &'b RecPoints) -> RcDoc<'a> {
    pp_ty(&field.ty, recs)
        .append(" ")
        .append(pp_label(&field.id))
        .append(";")
        .append(RcDoc::line_())
}

fn pp_record_fields<'a, 'b>(fs: &'a [Field], recs: &'b RecPoints) -> RcDoc<'a> {
    if is_tuple(fs) {
        strict_concat(fs.iter().map(|f| pp_ty(&f.ty, recs)), ",")
    } else {
        RcDoc::concat(fs.iter().map(|f| pp_record_field(f, recs)))
    }
}

fn pp_record_conversion<'a>(name: &'a str, fs: &'a [Field]) -> RcDoc<'a> {
    let convert_field = |id| {
        RcDoc::text(format!(
            r#"auto name = std::string("{id}");
        auto val = IdlValue(std::move(arg.{id}));
        fields.emplace_back(std::make_pair(name, std::move(val)));"#
        ))
    };

    let all_fields = RcDoc::concat(
        fs.iter()
            .map(|Field { id, .. }| enclose_space("{", convert_field(id), "}")),
    );

    let body = enclose(
        "std::vector<std::pair<std::string, IdlValue>> fields;\n",
        all_fields,
        "\n     *this = std::move(IdlValue::FromRecord(fields));",
    );
    let body = enclose_space("{", body, "}");

    let ctor = str("template <> IdlValue::IdlValue")
        .append(enclose("(", str(name), " arg)"))
        .append(body);

    let get_field = |id, ty| {
        RcDoc::text(format!(
            r#"auto field = std::move(fields["{id}"]);
        auto val = field.get"#
        ))
        .append(enclose("<", ty, ">();"))
        .append(RcDoc::hardline())
        .append(RcDoc::text(format!(
            r#"if (val.has_value()) {{
            result.{id} = std::move(val.value());
        }} else {{
            return std::nullopt;
        }}"#
        )))
    };

    let all_fields = RcDoc::concat(fs.iter().map(|Field { id, ty }| {
        enclose_space("{", get_field(id, pp_ty(ty, &BTreeSet::new())), "}")
    }));

    let body = str(name)
        .append(" result;")
        .append(RcDoc::hardline())
        .append("auto fields = this->getRecord();")
        .append(RcDoc::hardline())
        .append(all_fields)
        .append(RcDoc::hardline())
        .append("\treturn std::make_optional(std::move(result));");
    let getter = str("template <> std::optional")
        .append(enclose("<", str(name), ">"))
        .append(str(" IdlValue::get() "))
        .append(enclose("{", body, "}"));

    enclose_space(
        "namespace zondax {",
        ctor.append(RcDoc::hardline())
            .append(RcDoc::hardline())
            .append(getter),
        "}",
    )
}

fn pp_record<'a, 'b>(id: &'a str, fs: &'a [Field], recs: &'b RecPoints) -> RcDoc<'a> {
    let name = str(id).append(" ");

    let pp_mby_unit_record_constructor = || {
        if fs.is_empty() && !is_tuple(fs) {
            // avoids `{}` being a valid value of the struct
            // so instead you should use Name{}
            kwd("explicit").append(id).append("() = default;")
        } else {
            RcDoc::nil()
        }
    };

    let pp_mby_record_conversion = || {
        if is_tuple(fs) {
            RcDoc::nil()
        } else if fs.is_empty() {
            let constructor = str("template <> IdlValue::IdlValue").append(enclose(
                "(",
                name.clone(),
                " arg): IdlValue(std::move(IdlValue::null())) {}",
            ));

            let getter = str("template <> std::optional")
                .append(enclose("<", str(id), ">"))
                .append(str(" IdlValue::get() "))
                .append(enclose(
                    "{",
                    str("return this->get<std::monostate>().has_value() ? std::make_optional")
                        .append(enclose("<", str(id), ">() "))
                        .append(": std::nullopt;"),
                    "}",
                ));

            enclose_space(
                "namespace zondax {",
                constructor.append(RcDoc::hardline()).append(getter),
                "}",
            )
        } else {
            pp_record_conversion(id, fs)
        }
    };

    kwd("struct")
        .append(name.clone())
        .append(enclose(
            "{",
            pp_mby_unit_record_constructor().append(pp_record_fields(fs, recs)),
            "}",
        ))
        .append(";")
        .append(RcDoc::hardline())
        .append(pp_mby_record_conversion())
        .append(RcDoc::hardline())
}

fn pp_variant_fields<'a, 'b>(fs: &'a [Field], recs: &'b RecPoints) -> RcDoc<'a> {
    //TODO: generate name + code for variants
    //   static constexpr std::string_view name{"unknown"};
    //  static constexpr std::size_t code = 0;
    strict_concat(fs.iter().map(|Field { ty, .. }| pp_ty(ty, recs)), ",")
}

fn pp_defs<'a>(env: &'a TypeEnv, def_list: &'a [&'a str], recs: &'a RecPoints) -> RcDoc<'a> {
    lines(def_list.iter().map(|id| {
        let ty = env.find_type(id).unwrap();
        let name = ident(id).append(" ");
        match ty {
            Type::Record(fs) if is_tuple(fs) => kwd("using")
                .append(name)
                .append("= std::tuple")
                .append(enclose("<", pp_record_fields(fs, recs), ">;"))
                .append(RcDoc::hardline()),
            Type::Record(fs) => pp_record(id, fs, recs),
            Type::Variant(fs) => kwd("using")
                .append(name)
                .append("= std::variant")
                .append(enclose("<", pp_variant_fields(fs, recs), ">;"))
                .append(RcDoc::hardline()),
            _ => {
                // if recs.contains(id) {
                //TODO: newtype Foo(FooTy)
                // kwd("struct")
                //     .append(ident(id))
                //     .append(enclose("(", pp_ty(ty, recs), ")"))
                //     .append(";")
                //     .append(RcDoc::hardline())
                // } else {
                kwd("using")
                    .append(name)
                    .append("= ")
                    .append(pp_ty(ty, recs))
                    .append(";")
                // }
            }
        }
    }))
}

fn pp_function<'a>(id: &'a str, func: &'a Function) -> RcDoc<'a> {
    let name = ident(id);
    let empty = BTreeSet::new();
    let args = strict_concat(
        func.args
            .iter()
            .enumerate()
            .map(|(i, ty)| pp_ty(ty, &empty).append(RcDoc::text(format!(" arg{}", i)))),
        ",",
    );
    let rets = match func.rets.as_slice() {
        [] => str("std::monostate"),
        [ty] => pp_ty(ty, &empty),
        rets @ [..] => enclose(
            "std::tuple<",
            strict_concat(rets.iter().map(|ty| pp_ty(ty, &empty)), ","),
            ">",
        ),
    };

    let inner_ret_ty = enclose("std::variant<", rets.clone(), ", std::string>");

    let sig = inner_ret_ty
        .clone()
        .append(name)
        .append(enclose("(", args, ")"));
    let args = RcDoc::concat((0..func.args.len()).map(|i| RcDoc::text(format!(", arg{}", i))));
    let method = id.escape_debug().to_string();

    let is_query = func.is_query();
    let agent_method = if is_query { "Query" } else { "Update" };

    let body = RcDoc::text(format!("auto result = agent.{agent_method}"))
        .append(enclose("<", rets.clone(), ">"))
        .append(RcDoc::text(format!(r#"("{method}""#)))
        .append(args)
        .append(");");

    let index_0 = kwd("return").append(inner_ret_ty.clone()).append(str(
        "(std::in_place_index<0>, std::move(std::get<0>(result).value()));",
    ));
    let index_1 = kwd("return")
        .append(inner_ret_ty.clone())
        .append(str("(std::in_place_index<1>, std::get<1>(result));"));

    let clause = str("if (result.index() == 0)").append(
        enclose("{", index_0, "}")
            .append(kwd("else"))
            .append(enclose("{", index_1, "}")),
    );

    let body = body.append(RcDoc::hardline()).append(clause);

    sig.append(enclose_space("{", body, "}"))
}

fn pp_actor<'a>(env: &'a TypeEnv, actor: &'a Type) -> RcDoc<'a> {
    // TODO trace to service before we figure out what canister means in C++
    let serv = env.as_service(actor).unwrap();
    let body = RcDoc::intersperse(
        serv.iter().map(|(id, func)| {
            let func = env.as_func(func).unwrap();
            pp_function(id, func)
        }),
        RcDoc::hardline(),
    );
    RcDoc::text("class SERVICE {")
        .append(RcDoc::hardline())
        .append("private:")
        .append(RcDoc::hardline())
        .append("zondax::Agent agent;")
        .append(RcDoc::hardline())
        .append("public:")
        .append(RcDoc::hardline())
        .append("SERVICE(zondax::Agent&& agent): agent(std::move(agent)) {}")
        .append(RcDoc::hardline())
        .append(body)
        .append(RcDoc::hardline())
        .append("};")
}

/// The definition of tuple is language specific.
///
/// We'll be converting Unnamed records and the like into `std::tuple`
pub(crate) fn is_tuple(fs: &[Field]) -> bool {
    if fs.is_empty() {
        return false;
    }
    !fs.iter()
        .enumerate()
        .any(|(i, field)| field.id.get_id() != (i as u32))
}

fn path_to_var(path: &[TypePath]) -> String {
    let name: Vec<&str> = path
        .iter()
        .map(|node| match node {
            TypePath::Id(id) => id.as_str(),
            TypePath::RecordField(f) | TypePath::VariantField(f) => f.as_str(),
            TypePath::Opt => "inner",
            TypePath::Vec => "item",
            TypePath::Func(id) => id.as_str(),
            TypePath::Init => "init",
        })
        .collect();
    name.join("_")
}

/// Convert structural typing to nominal typing to fit C++'s type system
fn nominalize(env: &mut TypeEnv, path: &mut Vec<TypePath>, t: Type) -> Type {
    match t {
        Type::Opt(ty) => {
            path.push(TypePath::Opt);
            let ty = nominalize(env, path, *ty);
            path.pop();
            Type::Opt(Box::new(ty))
        }
        Type::Vec(ty) => {
            path.push(TypePath::Opt);
            let ty = nominalize(env, path, *ty);
            path.pop();
            Type::Vec(Box::new(ty))
        }
        Type::Record(fs) => {
            if matches!(
                path.last(),
                None | Some(TypePath::VariantField(_) | TypePath::Id(_))
            ) || is_tuple(&fs)
            {
                let fs: Vec<_> = fs
                    .into_iter()
                    .map(|Field { id, ty }| {
                        path.push(TypePath::RecordField(id.to_string()));
                        let ty = hoist_nested_type(env, path, ty);
                        let ty = nominalize(env, path, ty);
                        path.pop();
                        Field { id, ty }
                    })
                    .collect();
                Type::Record(fs)
            } else {
                let new_var = path_to_var(path);
                let ty = nominalize(
                    env,
                    &mut vec![TypePath::Id(new_var.clone())],
                    Type::Record(fs),
                );
                env.0.insert(new_var.clone(), ty);
                Type::Var(new_var)
            }
        }
        Type::Variant(fs) => match path.last() {
            None | Some(TypePath::Id(_)) => {
                let fs: Vec<_> = fs
                    .into_iter()
                    .map(|Field { id, ty }| {
                        path.push(TypePath::VariantField(id.to_string()));
                        let ty = hoist_nested_type(env, path, ty);
                        let ty = nominalize(env, path, ty);
                        path.pop();
                        Field { id, ty }
                    })
                    .collect();
                Type::Variant(fs)
            }
            Some(_) => {
                let new_var = path_to_var(path);
                let ty = nominalize(
                    env,
                    &mut vec![TypePath::Id(new_var.clone())],
                    Type::Variant(fs),
                );
                env.0.insert(new_var.clone(), ty);
                Type::Var(new_var)
            }
        },
        Type::Func(func) => Type::Func(Function {
            modes: func.modes,
            args: func
                .args
                .into_iter()
                .enumerate()
                .map(|(i, ty)| {
                    path.push(TypePath::Func(format!("arg{}", i)));
                    let ty = nominalize(env, path, ty);
                    path.pop();
                    ty
                })
                .collect(),
            rets: func
                .rets
                .into_iter()
                .enumerate()
                .map(|(i, ty)| {
                    path.push(TypePath::Func(format!("ret{}", i)));
                    let ty = nominalize(env, path, ty);
                    path.pop();
                    ty
                })
                .collect(),
        }),
        Type::Service(serv) => Type::Service(
            serv.into_iter()
                .map(|(meth, ty)| {
                    path.push(TypePath::Id(meth.to_string()));
                    let ty = nominalize(env, path, ty);
                    path.pop();
                    (meth, ty)
                })
                .collect(),
        ),
        Type::Class(args, ty) => Type::Class(
            args.into_iter()
                .map(|ty| {
                    path.push(TypePath::Init);
                    let ty = nominalize(env, path, ty);
                    path.pop();
                    ty
                })
                .collect(),
            Box::new(nominalize(env, path, *ty)),
        ),
        _ => t,
    }
}

/// Hoist a nested type into a separate record to be used
fn hoist_nested_type(env: &mut TypeEnv, path: &mut Vec<TypePath>, ty: Type) -> Type {
    let id = ident_(&path_to_var(path)).0;
    match ty {
        Type::Null => {
            env.0.insert(id.clone(), Type::Record(vec![]));
            Type::Var(id)
        }
        record @ Type::Record(_) => {
            env.0.insert(id.clone(), record);
            Type::Var(id)
        }
        variant @ Type::Variant(_) => {
            env.0.insert(id.clone(), variant);
            Type::Var(id)
        }
        _ => ty,
    }
}

/// Nominalize an entire environment and actor (if provided)
fn nominalize_all(env: &TypeEnv, actor: &Option<Type>) -> (TypeEnv, Option<Type>) {
    let mut res = TypeEnv(Default::default());
    for (id, ty) in env.0.iter() {
        let ty = nominalize(&mut res, &mut vec![TypePath::Id(id.clone())], ty.clone());
        res.0.insert(id.to_string(), ty);
    }

    let actor = actor
        .as_ref()
        .map(|ty| nominalize(&mut res, &mut vec![], ty.clone()));

    (res, actor)
}
