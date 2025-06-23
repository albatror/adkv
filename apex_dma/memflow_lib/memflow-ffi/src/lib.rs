#![allow(clippy::missing_safety_doc)]
#![allow(clippy::not_unsafe_ptr_arg_deref)]

use std::ffi::CStr;
use std::os::raw::c_char;
use std::ptr;

use memflow::plugins::Inventory;
use memflow::plugins::connector::{ConnectorArgs, ConnectorInstance};
use memflow::plugins::os::{OsArgs, OsInstance};
use memflow::plugins::args::Args;

// Types cglue corrected for memflow v0.2.x
type CGlueInst = memflow::cglue::CBox<'static, memflow::cglue::trait_group::c_void>;
type CGlueCtx  = memflow::cglue::CArc<memflow::cglue::trait_group::c_void>;

type RustConnectorInner = ConnectorInstance<'static, CGlueInst, CGlueCtx>;
type RustOsInner        = OsInstance<'static, CGlueInst, CGlueCtx>;

pub struct RustConnector(pub RustConnectorInner);
pub struct RustOs(pub RustOsInner);

#[repr(C)]
pub struct CMemflowConnectorInstance {
    pub inner: *mut RustConnector,
}

#[repr(C)]
pub struct CMemflowOsInstance {
    pub inner: *mut RustOs,
}

#[no_mangle]
pub unsafe extern "C" fn inventory_create_connector(
    inventory: *mut Inventory,
    name: *const c_char,
    conn_args: *const c_char,
) -> *mut CMemflowConnectorInstance {
    if inventory.is_null() || name.is_null() {
        return ptr::null_mut();
    }
    let name_str = match CStr::from_ptr(name).to_str() {
        Ok(s) => s,
        Err(_) => return ptr::null_mut(),
    };
    let args_str = if !conn_args.is_null() {
        match CStr::from_ptr(conn_args).to_str() {
            Ok(s) => s,
            Err(_) => return ptr::null_mut(),
        }
    } else { "" };
    let args = Args::with_default(args_str);
    let connector_args = ConnectorArgs::new(Some(name_str), args, None);

    let inventory = &mut *inventory;
    // Pass None for input, Some(&connector_args) for args
    match inventory.create_connector(name_str, None, Some(&connector_args)) {
        Ok(conn) => {
            let boxed = Box::new(RustConnector(conn));
            let cinst = Box::new(CMemflowConnectorInstance {
                inner: Box::into_raw(boxed),
            });
            Box::into_raw(cinst)
        }
        Err(_) => ptr::null_mut(),
    }
}

#[no_mangle]
pub unsafe extern "C" fn inventory_create_os(
    inventory: *mut Inventory,
    name: *const c_char,
    conn: *mut CMemflowConnectorInstance,
    os_args: *const c_char,
) -> *mut CMemflowOsInstance {
    if inventory.is_null() || name.is_null() || conn.is_null() {
        return ptr::null_mut();
    }
    let name_str = match CStr::from_ptr(name).to_str() {
        Ok(s) => s,
        Err(_) => return ptr::null_mut(),
    };
    let args_str = if !os_args.is_null() {
        match CStr::from_ptr(os_args).to_str() {
            Ok(s) => s,
            Err(_) => return ptr::null_mut(),
        }
    } else { "" };
    let args = Args::with_default(args_str);
    let os_args = OsArgs::new(Some(name_str), args);

    let inventory = &mut *inventory;
    // Obtain reference to the inner RustConnector
    let rust_conn = &*(*conn).inner;
    let conn_ref: RustConnectorInner = rust_conn.0.clone(); // Clone here
    // Pass Some(conn_ref) by value and Some(&os_args) as args
    match inventory.create_os(name_str, Some(conn_ref), Some(&os_args)) {
        Ok(os) => {
            let boxed = Box::new(RustOs(os));
            let cinst = Box::new(CMemflowOsInstance {
                inner: Box::into_raw(boxed),
            });
            Box::into_raw(cinst)
        }
        Err(_) => ptr::null_mut(),
    }
}

#[no_mangle]
pub unsafe extern "C" fn free_connector_instance(conn: *mut CMemflowConnectorInstance) {
    if !conn.is_null() {
        let _ = Box::from_raw(conn);
    }
}

#[no_mangle]
pub unsafe extern "C" fn free_os_instance(os: *mut CMemflowOsInstance) {
    if !os.is_null() {
        let _ = Box::from_raw(os);
    }
}
