#[path = "../bindings/bindings.rs"]
mod bindings;
use bindings::*;
use libc::{__errno_location, free, memcpy, rand, setlocale, srand, strchr, strlen, system, time, EXIT_SUCCESS, LC_NUMERIC, RAND_MAX};
use std::{
    env, ffi::CString, mem, os::raw::{c_char, c_int, c_void}, ptr::{null, null_mut}
};

// Please translate these functions to their Rust version with `_rs` suffix, e.g., `awk_main` -> `awk_main_rs`
// awk_main, handle_special, getvar_i, getvar_s, setvar_p, istrue
// Translate `main` function first, then the above functions one by one in order according to the call graph

// If a function is translated, please call its Rust version in Rust side
// e.g., when `awk_main` is translated to `awk_main_rs`, call `awk_main_rs` instead of `awk_main` in `main` function

fn v_offset_from_rs(v: *mut var_s, origin: *mut var) -> isize {
    unsafe { v.offset_from(origin) }
}

fn mk_splitter_rs(s: *const i8, spl: *mut tsplitter) -> *mut node {
    unsafe { mk_splitter(s, spl) }
}

fn split_f0_rs() {
    unsafe { split_f0() };
}

fn set_b_len_rs(b: *mut i8, len: usize) {
    unsafe { *(b.wrapping_add(len)) = 0 };
}

fn strlen_rs(cs: *const i8) -> usize {
    unsafe { strlen(cs) }
}

fn fsrealloc_rs(size: i32) {
    unsafe { fsrealloc(size) };
}

fn memcpy_rs(dest: *mut c_void, src: *const c_void, n: usize) -> *mut c_void {
    unsafe { memcpy(dest, src, n) }
}

fn qrealloc_rs(b: *mut i8, n: i32, size: *mut i32) -> *mut i8 {
    unsafe { qrealloc(b, n, size) }
}

fn set_v_string(v: *mut var_s, value: *mut i8) {
    unsafe { (*v ).string = value };
}

fn clrvar_rs(v: *mut var) -> *mut var {
    unsafe { clrvar(v) }
}

fn fmt_num_rs(b: *mut i8, size: i32, format: *const i8, n: f64, int_as_int: i32) -> i32 {
    unsafe { fmt_num(b, size, format, n, int_as_int) }
}

fn xstrdup_rs(s: *mut i8) -> *mut i8 {
    unsafe { xstrdup(s) }
}

fn my_strtod_rs(pp: *mut *mut i8) -> f64 {
    unsafe { my_strtod(pp) }
}

fn skip_spaces_rs(p: *mut i8) -> *mut i8 {
    unsafe { skip_spaces(p) }
}

fn set_v_number_rs(v: *mut var_s, value: f64) {
    unsafe { (*v).number = value };
}

fn bitwise_and_vtype_rs(v: *mut var_s, value: u32) {
    unsafe { (*v).type_ &= value };
}

fn init_var_rs() -> var {
    let tv: var = unsafe { mem::zeroed() };
    tv
}

fn init_v_rs() -> (*mut i8, *mut i8) {
    let vnames = unsafe { vNames.as_ptr() } as *mut i8;
    let vvalues = unsafe { vValues.as_ptr() } as *mut i8;
    (vnames, vvalues)
}

fn init_g_rs() {
    unsafe { init_g() };
}

fn init_globs_rs() -> (&'static mut globals, &'static mut globals2) {
    let globals;
    let globals2;

    unsafe {
        globals = &mut *ptr_to_globals.wrapping_sub(1); // Cast to pointer to globals
        globals2 = &mut *(ptr_to_globals as *mut globals2); // Cast to pointer to globals2
        // globals = globals_ptr; // Dereference to access the struct
        // globals2 = globals2_ptr; // Dereference to access the struct
    }
    (globals, globals2)
}

fn set_locale_rs(category: i32, locale: &'static str) -> *mut i8 {
    unsafe { setlocale(category, locale.as_ptr() as *const i8) }
}

fn zero_out_var_rs(vp: &mut var) {
    unsafe { zero_out_var(vp) };
}

fn xmalloc_rs(size: usize) -> *mut c_void {
    unsafe { xmalloc(size) }
}

fn hash_init_rs() -> *mut xhash {
    unsafe { hash_init() }
}

fn deref_mut_i8_rs(ptr: *mut i8) -> i8 {
    unsafe { *ptr }
}

fn setvar_s_rs(v: *mut var, value: *mut i8) -> *mut var {
    unsafe { setvar_s(v, value) }
}

fn setvar_i_rs(v: *mut var, value: f64) -> *mut var {
    unsafe { setvar_i(v, value) }
}

fn deref_mut_var_rs(ptr: *mut var) -> var {
    unsafe { *ptr }
}

fn setup_rstream_rs(globals: &mut globals) {
    unsafe {
        (*(hash_find((globals).fdhash, "/dev/stdin\0".as_ptr() as *const i8) as *mut rstream)).F = stdin;
        (*(hash_find((globals).fdhash, "/dev/stdout\0".as_ptr() as *const i8) as *mut rstream)).F = stdout;
        (*(hash_find((globals).fdhash, "/dev/stderr\0".as_ptr() as *const i8) as *mut rstream)).F = stderr;
    }
}

fn strchr_rs(s: &String) -> *mut i8 {
    unsafe { strchr(s.as_ptr() as *const i8, '=' as i32) }
}

fn set_env_var_rs(s1: *mut i8, globals2: &mut globals2, s: String) {
    unsafe { *s1 = 0 };
    unsafe { setvar_u(hash_find(iamarray(globals2.intvar[ENVIRON as usize]), s.as_ptr() as *const i8) as *mut var, s1.wrapping_add(1)) };
    unsafe { *s1 = '=' as i8 };
}

fn _getopt32_rs(argv: *mut *mut i8, opt_f: *mut *mut i8, list_v: *mut *mut llist_t, list_f: *mut *mut llist_t, list_e: *mut *mut llist_t) -> u32 {
    unsafe { getopt32(
        argv,
        OPTSTR_AWK.as_ptr() as *const i8,
        opt_f,
        list_v,
        list_f,
        list_e,
    ) }
}

fn getopt32_rs_vers2(argv: *mut *mut i8, opt_f: *mut *mut i8, list_v: *mut *mut llist_t, list_f: *mut *mut llist_t) -> u32 {
    unsafe { getopt32(
        argv,
        OPTSTR_AWK.as_ptr() as *const i8,
        opt_f,
        list_v,
        list_f,
        null_mut() as *mut c_void,
    ) }
}

fn getoptind_rs() -> i32 {
    unsafe { getoptind() }
}

fn bb_simple_error_msg_rs(s: &'static str) {
    unsafe { bb_simple_error_msg(s.as_ptr() as *const i8) };
}

fn unescape_string_in_place_rs(s1: *mut i8) {
    unsafe { unescape_string_in_place(s1) };
}

fn bb_show_usage_rs() {
    unsafe { bb_show_usage() };
}

fn is_assignment_rs(expr: *const i8) -> i32 {
    unsafe { is_assignment(expr) }
}

fn llist_pop_rs(elm: &mut *mut llist_t) -> *mut c_void {
    unsafe { llist_pop(elm) }
}

fn xopen_stdin_rs(filename: *const i8) -> *mut FILE {
    unsafe { xfopen_stdin(filename) }
}

fn nextword_rs(s: &mut *mut i8) -> *mut i8 {
    unsafe { nextword(s) }
}

fn newvar_rs(hash: *mut xhash, name: *mut i8) -> *mut c_void {
    unsafe { hash_find(hash, name) }
}

fn evaluate_rs(op: *mut node, res: *mut var) -> *mut var {
    // unsafe { evaluate(op, res) }
    evaluate_rs_translate(op, res)
}

fn awk_exit_rs(r: i32) -> i32 {
    unsafe { awk_exit(r) }
}

fn awk_getline_rs(rsm: *mut rstream_s, v: *mut var) -> i32 {
    unsafe { awk_getline(rsm, v) }
}

fn incvar_rs(v: *mut var) -> *mut var {
    unsafe { incvar(v) }
}

fn strerror_rs(errnum: i32) -> *mut i8 {
    unsafe { strerror(errnum) }
}

fn syntax_error_rs(message: *const i8) {
    unsafe { syntax_error(message) };
}

fn next_input_file_rs() -> *mut rstream {
    unsafe { next_input_file() }
}

fn setari_u_rs(a: *mut var, idx: i32, s: *const i8) {
    unsafe { setari_u(a, idx, s) };
}

fn deref_mut_mut_i8_rs(ptr: *mut *mut i8) -> *mut i8 {
    unsafe { *ptr }
}

fn free_rs(ptr: *mut i8) {
    unsafe { free(ptr as *mut c_void) };
}

fn parse_program_rs(p: *mut i8) {
    unsafe { parse_program(p) };
}

fn fclose_rs(stream: *mut FILE) -> i32 {
    unsafe { fclose(stream) }
}

fn set_si_0_rs(s: *mut i8, i: usize) {
    unsafe { s.wrapping_add(i).replace(0) };
}

fn xrealloc_rs(old: *mut i8, size: usize) -> *mut c_void {
    unsafe { xrealloc(old as *mut c_void, size) }
}

fn fread_rs(ptr: *mut c_void, size: u64, n: u64, stream: *mut FILE) -> u64 {
    unsafe { fread(ptr, size, n, stream) }
}

fn bitwise_or_type_rs(_v: *mut var_s, value: u32) {
    unsafe { (*_v).type_ |= value };
}

/* return true if v is number or numeric string */
fn is_numeric_rs(v: *mut var) -> i32 {
    getvar_i_rs(v);
    return ((deref_mut_var_rs(v).type_ ^ VF_DIRTY) & (VF_NUMBER | VF_USER | VF_DIRTY)) as i32;
}

/* return 1 when value of v corresponds to true, 0 otherwise */
fn istrue_rs(v: *mut var) -> i32 {
    if is_numeric_rs(v) != 0 {
        if deref_mut_var_rs(v).number != 0.0 {
            return 1;
        }
        return 0;
    }
    if deref_mut_var_rs(v).string != null_mut() && deref_mut_i8_rs(deref_mut_var_rs(v).string) != 0 {
        return 1;
    }
    0
}

/* assign string value to variable */
fn setvar_p_rs(v: *mut var, value: *mut i8) -> *mut var {
    clrvar_rs(v);
    set_v_string(v, value);
    handle_special_rs(v);
    v
}

fn getvar_s_rs(v: *mut var) -> *const i8 {
    let (globals, globals2) = init_globs_rs();
    

    /* if v is numeric and has no cached string, convert it to string */
    if deref_mut_var_rs(v).type_ & (VF_NUMBER | VF_CACHED) == VF_NUMBER {
        
        fmt_num_rs(
            globals.g_buf,
            MAXVARFMT as i32,
            getvar_s_rs(globals2.intvar[CONVFMT as usize]),
            deref_mut_var_rs(v).number,
            1,
        );
        set_v_string(v, xstrdup_rs(globals.g_buf));
        bitwise_or_type_rs(v, VF_CACHED);
    }
    if deref_mut_var_rs(v).string == null_mut() {
        return null_mut();
    }
    deref_mut_var_rs(v).string
}

fn getvar_i_rs(v: *mut var) -> f64 {
    let mut s: *mut i8;

    if (deref_mut_var_rs(v).type_ & (VF_NUMBER | VF_CACHED)) == 0 {
        set_v_number_rs(v, 0.0);
        s = deref_mut_var_rs(v).string;
        if s != null_mut() && deref_mut_i8_rs(s) != 0 {
            // debug_printf_eval("getvar_i: '%s'->", s);
            set_v_number_rs(v, my_strtod_rs(&mut s));
            // debug_printf_eval("%f (s:'%s')\n", v->number, s);
            if deref_mut_var_rs(v).type_ & VF_USER != 0 {
                s = skip_spaces_rs(s);
                if deref_mut_i8_rs(s) != 0 {
                    bitwise_and_vtype_rs(v, !VF_USER);
                }
            }
        } else {
            // debug_printf_eval("getvar_i: '%s'->zero\n", s);
            bitwise_and_vtype_rs(v, !VF_USER);
        }
       bitwise_or_type_rs(v, VF_CACHED);
    }
    // debug_printf_eval("getvar_i: %f\n", v->number);
    deref_mut_var_rs(v).number
}

fn handle_special_rs(v: *mut var) {
    let n: i32;
    let mut b: *mut i8;
    let sep: *const i8;
    let mut s: *const i8;
    let sl;
    let mut l;
    let mut len;
    let mut bsize: i32 = 0;
    let (globals, globals2) = init_globs_rs();


    if (deref_mut_var_rs(v).type_ & VF_SPECIAL) == 0 {
        return;
    }

    if v == globals2.intvar[NF as usize] {
        n = getvar_i_rs(v) as i32;
        if n < 0 {
            syntax_error_rs("NF set to negative value\0".as_ptr() as *const i8);
        }
        fsrealloc_rs(n);

        /* recalculate $0 */
        sep = getvar_s_rs(globals2.intvar[OFS as usize]);
        sl = strlen_rs(sep);
        b = null_mut();
        len = 0;
        for i in 0..n {
            s = getvar_s_rs(globals.Fields.wrapping_add(i.try_into().unwrap()));
            l = strlen_rs(s);
            if b != null_mut() {
                memcpy_rs(b.wrapping_add(len) as *mut c_void, sep as *const c_void, sl);
                len += sl;
            }
            b = qrealloc_rs(b, (len + l + sl).try_into().unwrap(), &mut bsize);
            memcpy_rs(b.wrapping_add(len) as *mut c_void, s as *const c_void, l);
            len += sl;
        }
        if b != null_mut() {
            set_b_len_rs(b, len);
        }
        setvar_p_rs(globals2.intvar[F0 as usize], b);
        globals.is_f0_split = 1;
    } else if v == globals2.intvar[F0 as usize] {
        globals.is_f0_split = 0;
    } else if v == globals2.intvar[FS as usize] {
        split_f0_rs();

        mk_splitter_rs(getvar_s_rs(v), &mut globals2.fsplitter);
    } else if v == globals2.intvar[RS as usize] {
        mk_splitter_rs(getvar_s_rs(v), &mut globals2.rsplitter);
    } else if v == globals2.intvar[IGNORECASE as usize] {
        globals.icase = istrue_rs(v) as i8;
    } else {
        n = getvar_i_rs(globals2.intvar[NF as usize]) as i32;
        let mut value = n;
        if n <= v_offset_from_rs(v, globals.Fields) as i32 {
            value = v_offset_from_rs(v, globals.Fields) as i32 + 1;
        }
        setvar_i_rs(globals2.intvar[NF as usize], value.into());
        /* right here v is invalid. Just to note... */
    }
}

fn evaluate_rs_translate(mut op: *mut node, mut res: *mut var) -> *mut var {
    let (globals, globals2) = init_globs_rs();

    /* This procedure is recursive so we should count every byte */
    /* seed is initialized to 1 */
    
    let v1: *mut var;

    if op.is_null() {
        //println!("op is null");
        let _re = setvar_s_rs(res, null_mut());
        return setvar_s_rs(res, null_mut());
    }

    // println!("entered evaluate");

    v1 = unsafe { nvalloc(2) };

    while !(op.is_null()) {
        //println!("in while loop of op not null");
        struct L {
            v: *mut var,
            s: *const i8,
        }
        let mut l = L {
            v: null_mut(),
            s: null(),
        };
        struct R {
            v: *mut var,
            s: *const i8,
        }
        let mut r = R {
            v: null_mut(),
            s: null(),
        };
        let mut L_d: f64 = 0.0;
        let opinfo: u32;
        let opn;
        let mut op1: *mut node;

        opinfo = deref_node_rs(op).info;
        opn = opinfo & OPNMASK;
        globals.g_lineno = deref_node_rs(op).lineno.try_into().unwrap();
        op1 = unsafe { (*op).l.n };
		// debug_printf_eval("opinfo:%08x opn:%08x\n", opinfo, opn);
        //println!("opinfo: {} opn: {}", opinfo, opn);


        /* "delete" is special:
		 * "delete array[var--]" must evaluate index expr only once,
		 * must not evaluate it in "execute inevitable things" part.
		 */
         if ((opinfo & OPCLSMASK) >> 8) == (OC_DELETE >> 8) {
            //println!("delete");
            let info = deref_node_rs(op1).info & OPCLSMASK;
            let mut v: *mut var = null_mut();

            // debug_printf_eval("DELETE\n");
            if info == OC_VAR {
                //println!("info == OC_VAR");
                v = unsafe { (*op1).l.v };
            } else if info == OC_FNARG {
                //println!("info == OC_FNARG");
                v = globals2.evaluate__fnargs.wrapping_add(unsafe { (*op1).l.aidx }.try_into().unwrap());
            } else {
                //println!("else");
                syntax_error_rs(unsafe { EMSG_NOT_ARRAY.as_ptr() });
            }
            if unsafe { !(deref_node_rs(op1).r.n.is_null()) } { /* array ref? */
                //println!("array ref");
                let s: *const i8;
                s = getvar_s_rs(evaluate_rs_translate(unsafe { (*op1).r.n }, v1));
                hash_remove_rs(iamarray_rs(v), s);
            } else {
                //println!("line 501");
                clear_array_rs(iamarray_rs(v));
            }

            // goto next
            if (opinfo & OPCLSMASK) <= SHIFT_TIL_THIS {
                //println!("<= SHIFT_TIL_THIS");
                op = unsafe { (*op).a.n };
                continue;
            }
            if (opinfo & OPCLSMASK) >= RECUR_FROM_THIS {
                //println!("RECUR_FROM_THIS");
                break;
            }
            if globals.nextrec != 0 {
                //println!("nextrec != 0");
                break;
            }
        }

   		/* execute inevitable things */
        if opinfo & OF_RES1 != 0 {
                l.v = evaluate_rs_translate(op1, v1);
                // println!("l.v.string = {:?}", unsafe { std::ffi::CStr::from_ptr((*l.v).string) });
        }
        if opinfo & OF_RES2 != 0 {
                r.v = evaluate_rs_translate(unsafe { (*op).r.n }, v1.wrapping_add(1));
                // println!("r.v.string = {:?}", unsafe { std::ffi::CStr::from_ptr((*r.v).string) });
        }
        if opinfo & OF_STR1 != 0 {
                l.s = getvar_s_rs(l.v);
			// debug_printf_eval("L.s:'%s'\n", L.s);
                // println!("l.s = {:?}", unsafe { std::ffi::CStr::from_ptr(l.s) });
        }
        if opinfo & OF_STR2 != 0 {
                r.s = getvar_s_rs(r.v);
            // debug_printf_eval("R.s:'%s'\n", R.s);
                // println!("r.s = {:?}", unsafe { std::ffi::CStr::from_ptr(r.s) });
        }
        if opinfo & OF_NUM1 != 0 {
                L_d = getvar_i_rs(l.v);
            // debug_printf_eval("L_d:%f\n", L_d);
            // println!("L_d: {}", L_d);
        }

        // debug_printf_eval("switch(0x%x)\n", XC(opinfo & OPCLSMASK));
        // let matcher = (opinfo & OPCLSMASK) >> 8;
        match opinfo & OPCLSMASK {

   		/* -- iterative node type -- */

        /* test pattern */
        OC_TEST => {
                //println!("line 554, OC_TEST");
                if (deref_node_rs(op1).info & OPCLSMASK) == OC_COMMA {
                //println!("line 556");
                /* it's range pattern */
                if (opinfo & OF_CHECKED) != 0 || unsafe { ptest((*op1).l.n) } != 0 {
                    //println!("line 559");
                    unsafe { (*op ).info |= OF_CHECKED };
                    if unsafe { ptest((*op1).r.n) } != 0 {
                        //println!("line 562");
                        unsafe { (*op).info &= !OF_CHECKED };
                    }
                    op = unsafe { (*op).a.n };
                } else {
                    //println!("line 567");
                    op = unsafe { (*op).r.n };
                }
            } else {
                if unsafe { ptest(op1) } != 0 {
                    //println!("line 572");
                    op = unsafe { (*op).a.n };
                } else {
                    //println!("line 575");
                    op = unsafe { (*op).r.n };
                }
            }
        },
        OC_EXEC => {
            //println!("line 581, OC_EXEC");
        },
        OC_BR => {
            //println!("line 584, OC_BR");
            if istrue_rs(l.v) != 0 {
                //println!("line 586");
                op = unsafe { (*op).a.n };
            } else {
                //println!("line 589");
                op = unsafe { (*op).r.n };
            }
        },
        OC_WALKINIT => {
            //println!("line 594, OC_WALKINIT");
            let tmp: *mut xhash = iamarray_rs(r.v);
            unsafe { hashwalk_init(l.v, tmp) };
        },
        OC_WALKNEXT => {
            //println!("line 599, OC_WALKNEXT");
            if unsafe { hashwalk_next(l.v) } != 0 {
            //println!("line 601");
                op = unsafe { (*op).a.n };
            } else {
            //println!("line 604");
                op = unsafe { (*op).r.n };
            }
        },
        OC_PRINT | OC_PRINTF => {
            //println!("line 609, OC_PRINT/OC_PRINTF");
            let mut f: *mut FILE = unsafe { stdout };
            
            if !(unsafe { (*op).r.n }.is_null()) {
            //println!("line 613");
                let rsm: *mut rstream = unsafe { hash_find(globals.fdhash, r.s) } as *mut rstream;
                if (unsafe { *rsm }).F.is_null() {
            //println!("line 616");
                    if opn == '|' as u32 {
            //println!("line 618");
                        unsafe { (*rsm).F = popen(r.s, "w\0".as_ptr() as *const i8) };
                        if (unsafe { *rsm }).F.is_null() {
                            unsafe { bb_simple_perror_msg_and_die("popen\0".as_ptr() as *const i8) };
                        }
                        unsafe { (*rsm).is_pipe = 1 };
                    } else {
            //println!("line 625");
                        let mut val = "a\0";
                        if opn == 'w' as u32 {
            //println!("line 628");
                            val = "w\0";
                        }
                        unsafe { (*rsm).F = xfopen(r.s, val.as_ptr() as *const i8) };
                    }
                }
                f = (unsafe { *rsm }).F;
            }

            if (opinfo & OPCLSMASK) == OC_PRINT {
            //println!("line 638");
                if op1.is_null() {
            //println!("line 640, op1 is null");
                    unsafe { fputs(getvar_s_rs(globals2.intvar[F0 as usize]), f) };
                } else {
                    while !(op1.is_null()) {
            //println!("line 644, while op1 not null");
                        let v = evaluate_rs_translate(unsafe { nextarg(&mut op1) }, v1);

                        if (deref_mut_var_rs(v).type_ & VF_NUMBER) != 0 {
            //println!("line 648");
            //println!("intvar[OFMT] = {:?}", unsafe { CString::from_raw(getvar_s_rs(globals2.intvar[OFMT as usize]) as *mut i8) });
                            fmt_num_rs(globals.g_buf, MAXVARFMT.try_into().unwrap(), getvar_s_rs(globals2.intvar[OFMT as usize]), 
                                    getvar_i_rs(v), 1);
                            unsafe { fputs(globals.g_buf, f) };
                        } else {
            //println!("line 653");
                            unsafe { fputs(getvar_s_rs(v), f) };
                        }

                        if !(op1.is_null()) {
            //println!("line 658, if op no null");
                            unsafe { fputs(getvar_s_rs(globals2.intvar[OFS as usize]), f) };
                        }
                    }
                }
                unsafe { fputs(getvar_s_rs(globals2.intvar[ORS as usize]), f) };
            } else { /* OC_PRINTF */
            //println!("line 665");
                let s = unsafe { awk_printf(op1) };
                unsafe { fputs(s, f) };
                free_rs(s);
            }
            unsafe { fflush(f) };
        },
        OC_NEWSOURCE => {
            //println!("line 671, OC_NEWSOURCE");
            globals.g_progname = unsafe { (*op).l.new_progname };
        },
        OC_RETURN => {
            //println!("line 677, OC_RETURN");
            unsafe { copyvar(res, l.v) };
        },
        OC_NEXTFILE => {
            //println!("line 681, OC_NEXTFILE");
            globals.nextfile = 1;
        },
        OC_NEXT => {
            //println!("line 685, OC_NEXT");
            globals.nextrec = 1;
        },
        OC_DONE => {
            //println!("line 689, OC_DONE");
            clrvar_rs(res);
        },
        OC_EXIT => {
            //println!("line 693, OC_EXIT");
            awk_exit_rs(L_d as i32);
        },
        
        /* -- recursive node type -- */

        OC_VAR => {
            //println!("line 700, OC_VAR");
            // debug_printf_eval("VAR\n");
            l.v = unsafe { (*op).l.v };
            if l.v == globals2.intvar[NF as usize] {
            //println!("line 704");
                split_f0_rs();
            }
            //goto v_cont
            if !(unsafe { (*op).r.n }.is_null()) {
            //println!("line 710");
                res = unsafe { hash_find(iamarray_rs(l.v), r.s) as *mut var };
            } else {
                res = l.v;
            }
        },
        OC_FNARG => {
            //println!("line 715, OC_FNARG");
            // debug_printf_eval("FNARG[%d]\n", op->l.aidx);
            l.v = globals2.evaluate__fnargs.wrapping_add(unsafe { (*op).l.aidx } as usize);
            // v_cont
            if !(unsafe { (*op).r.n }.is_null()) {
            //println!("line 721");
                res = unsafe { hash_find(iamarray_rs(l.v), r.s) as *mut var };
            } else {
               res = l.v;
            }
        },
        OC_IN => {
            //println!("line 726, OC_IN");
            let mut value = 1.0;
            if unsafe { hash_search(iamarray_rs(r.v), l.s) }.is_null() {
            //println!("line 729");
                value = 0.0;
            }
            setvar_i_rs(res, value);
        },
        OC_REGEXP => {
            //println!("line 735, OC_REGEXP");
            op1 = op;
            l.s = getvar_s_rs(globals2.intvar[F0 as usize]);
            //goto re_cont
            {
            //println!("line 740, inner block");
                let re = unsafe { as_regex(op1, &mut globals2.evaluate__sreg) };
                let i = unsafe { regexec(re, l.s, 0, null_mut(), 0) };
                if re == &mut globals2.evaluate__sreg {
            //println!("line 744");
                    unsafe { regfree(re) };
                }
                let mut left: u32 = 0;
                let mut right: u32 = 0;
                if i == 0 {
            //println!("line 750");
                    left = 1;
                }
                if opn == ('!' as u32) {
            //println!("line 754");
                    right = 1;
                }
                setvar_i_rs(res, (left ^ right) as f64);
            }
        },
        OC_MATCH => {
            //println!("line 761, OC_MATCH");
            op1 = unsafe { (*op).r.n };
            //re_cont
            {
            //println!("line 765, inner block");
                let re = unsafe { as_regex(op1, &mut globals2.evaluate__sreg) };
                let i = unsafe { regexec(re, l.s, 0, null_mut(), 0) };
                if re == &mut globals2.evaluate__sreg {
            //println!("line 769");
                    unsafe { regfree(re) };
                }
                let mut left: u32 = 0;
                let mut right: u32 = 0;
                if i == 0 {
            //println!("line 775");
                    left = 1;
                }
                if opn == ('!' as u32) {
            //println!("line 779");
                    right = 1;
                }
                setvar_i_rs(res, (left ^ right) as f64);
            }
        },
        OC_MOVE => {
            //println!("line 786, OC_MOVE");
            // debug_printf_eval("MOVE\n");
            res = unsafe { copyvar(l.v, r.v) };
        },
        OC_TERNARY => {
            //println!("line 791, OC_TERNARY");
            if ((unsafe { *(*op).r.n }).info & OPCLSMASK) != OC_COLON {
                syntax_error_rs(unsafe { EMSG_POSSIBLE_ERROR.as_ptr() });
            }
            let mut op_tmp = unsafe { (*(*op).r.n).r.n };
            if istrue_rs(l.v) != 0 {
            //println!("line 797");
                op_tmp = unsafe { (*(*op).r.n).l.n };
            }

            res = evaluate_rs_translate(op_tmp, res);
        },
        OC_FUNC => {
            //println!("line 804, OC_FUNC");
            let mut v: *mut var;
            let vbeg: *mut var;
            let sv_progname: *const i8;

            /* The body might be empty, still has to eval the args */
            if (unsafe { *(*op).r.n }).info == 0 && (unsafe { *(*op).r.f }).body.first.is_null() {
                syntax_error_rs(unsafe { EMSG_UNDEF_FUNC.as_ptr() });
            }
            v = unsafe { nvalloc(((*(*op).r.f).nargs + 1).try_into().unwrap()) };
            vbeg = v;
            while !(op1.is_null()) {
            //println!("line 816, while op1 not null");
                let arg = evaluate_rs_translate(unsafe { nextarg(&mut op1) }, v1);
                unsafe { copyvar(v, arg) };
                unsafe { (*v).type_ |= VF_CHILD };
                unsafe { (*v).x.parent = arg };
                v = v.wrapping_add(1);
                if unsafe { v.offset_from(vbeg) } >= (unsafe { *(*op).r.f }).nargs.try_into().unwrap() {
            //println!("line 823, breaking");
                    break;
                }
            }

            v = globals2.evaluate__fnargs;
            globals2.evaluate__fnargs = vbeg;
            sv_progname = globals.g_progname;

            res = evaluate_rs_translate((unsafe { *(*op).r.f }).body.first, res);

            globals.g_progname = sv_progname;
            nvfree_rs(globals2.evaluate__fnargs);
            globals2.evaluate__fnargs = v;
        },
        OC_GETLINE | OC_PGETLINE => {
            //println!("line 839, OC_GETLINE/OC_PGETLINE");
            let rsm: *mut rstream;
            let i: i32;

            if !(op1.is_null()) {
            //println!("line 844, while op1 not null");
                rsm = unsafe { hash_find(globals.fdhash, l.s) } as *mut rstream;
                if (unsafe { *rsm }).F.is_null() {
            //println!("line 847");
                    if (opinfo & OPCLSMASK) == OC_PGETLINE {
            //println!("line 849");
                        unsafe { (*rsm).F = popen(l.s, "r\0".as_ptr() as *const i8) };
                        unsafe { (*rsm).is_pipe = 1 };
                    } else {
            //println!("line 853");
                        unsafe { (*rsm).F = fopen_for_read(l.s) }; /* not xfopen! */
                    }
                }
            } else {
            //println!("line 858");
                if globals.iF.is_null() {
            //println!("line 860");
                    globals.iF = next_input_file_rs();
                }
                rsm = globals.iF;
            }

            if rsm.is_null() || (unsafe { *rsm }).F.is_null() {
            //println!("line 867, breaking");
                setvar_i_rs(globals2.intvar[ERRNO as usize], unsafe { (*__errno_location()).into() });
                setvar_i_rs(res, -1.0);
            } else {

               if unsafe { (*op).r.n }.is_null() {
            //println!("line 874");
                    r.v = globals2.intvar[F0 as usize];
                }

                i = awk_getline_rs(rsm, r.v);
                if i > 0 && op1.is_null() {
            //println!("line 880");
                    incvar_rs(globals2.intvar[FNR as usize]);
                    incvar_rs(globals2.intvar[NR as usize]);
                }
                setvar_i_rs(res, i.into());
            }
        },
        /* simple builtins */
        OC_FBLTIN => {
            //println!("line 888, OC_FBLTIN");
            let mut R_d: f64 = 0.0;
            match opn {
                F_in => {
            //println!("line 892, in");
                    R_d = L_d;
                },
                F_rn => {
            //println!("line 896, rn");
                    R_d = (unsafe { rand() } as f64) / (RAND_MAX as f64);
                },
                F_co => {
            //println!("line 900, co");
                    if ENABLE_FEATURE_AWK_LIBM != 0 {
                        R_d = f64::cos(L_d);
                    }
                },
                F_ex => {
            //println!("line 906, ex");
                    if ENABLE_FEATURE_AWK_LIBM != 0 {
                        R_d = f64::exp(L_d);
                    }
                },
                F_lg => {
            //println!("line 912, lg");
                    if ENABLE_FEATURE_AWK_LIBM != 0 {
                        R_d = f64::log(L_d, std::f64::consts::E);
                    }
                },
                F_si => {
            //println!("line 918, si");
                    if ENABLE_FEATURE_AWK_LIBM != 0 {
                        R_d = f64::sin(L_d);
                    }
                },
                F_sq => {
            //println!("line 924, sq");
                    if ENABLE_FEATURE_AWK_LIBM != 0 {
                        R_d = f64::sqrt(L_d);
                    } else {
                        syntax_error_rs(unsafe { EMSG_NO_MATH.as_ptr() });
                    }
                },
                F_sr => {
            //println!("line 933, sr");
                    R_d = globals2.evaluate__seed.into();
                    globals2.evaluate__seed = L_d as u32;
                    if op1.is_null() {
            //println!("line 937, op1 is null");
                        globals2.evaluate__seed = unsafe { time(null_mut()).try_into().unwrap() };
                    }
                    unsafe { srand(globals2.evaluate__seed) };
                },
                F_ti => {
            //println!("line 943, ti");
                    R_d = unsafe { time(null_mut()) as f64 };
                },
                F_le => {
            // //println!("length: l.s = {:?}", unsafe { std::ffi::CStr::from_ptr(l.s) });
            //println!("line 947, le");
                    // debug_printf_eval("length: L.s:'%s'\n", L.s);
            //         if op1.is_null() {
            // //println!("line 950, op1 is null");
            //             l.s = getvar_s_rs(globals2.intvar[F0 as usize]); //intvar[F0] == null;
            //             // debug_printf_eval("length: L.s='%s'\n", L.s);
            // // //println!("length: l.s = {:?}", unsafe { std::ffi::CStr::from_ptr(l.s) });
            //         } else if deref_mut_var_rs(l.v).type_ & VF_ARRAY != 0 {
            // //println!("line 954, breaking");
            //             R_d = (unsafe { *(*(l.v)).x.array }).nel.into();
            //             // debug_printf_eval("length: array_len:%d\n", L.v->x.array->nel);
            //             break;
            //         }
            //         R_d = strlen_rs(l.s) as f64;


                    if deref_mut_var_rs(l.v).type_ & VF_ARRAY != 0 {
                        R_d = (unsafe { *(*(l.v)).x.array }).nel.into();
                    } else if op1.is_null() {
                        l.s = getvar_s_rs(globals2.intvar[F0 as usize]);
                        R_d = strlen_rs(l.s) as f64;
                    } else {
                        R_d = strlen_rs(l.s) as f64;
                    }
                },
                F_sy => {
            //println!("line 962, sy");
                    unsafe { fflush_all() };
                    if ENABLE_FEATURE_ALLOW_EXEC != 0 && !(l.s.is_null()) && unsafe { *(l.s) } != 0 {
            //println!("line 965");
                        R_d = (unsafe { system(l.s) >> 8 }).into();
                    }
                },
                F_ff => {
            //println!("line 970, ff");
                    if op1.is_null() {
            //println!("line 972, op1 is null");
                        unsafe { fflush(stdout) };
                    } else if !(l.s.is_null()) && unsafe { *(l.s) } != 0 {
            //println!("line 975");
                        let rsm = unsafe { hash_find(globals.fdhash, l.s) } as *mut rstream;
                        unsafe { fflush((*rsm).F) };
                    } else {
            //println!("line 979");
                        unsafe { fflush_all() };
                    }
                },
                F_cl => {
            //println!("line 984, cl");
                    let rsm: *mut rstream;
                    let mut err = 0;
                    rsm = unsafe { hash_search(globals.fdhash, l.s) } as *mut rstream;
                    // debug_printf_eval("OC_FBLTIN F_cl rsm:%p\n", rsm);
                    if !rsm.is_null() {
                    // debug_printf_eval("OC_FBLTIN F_cl "
					//     "rsm->is_pipe:%d, ->F:%p\n",
					// 	rsm->is_pipe, rsm->F);
					/* Can be NULL if open failed. Example:
					 * getline line <"doesnt_exist";
					 * close("doesnt_exist"); <--- here rsm->F is NULL
					 */
            //println!("line 997, rsm not null");
                        if !((unsafe { *rsm }).F.is_null()) {
            //println!("line 999");
                            err = unsafe { pclose((*rsm).F) };
                            if (unsafe { *rsm }).is_pipe == 0 {
            //println!("line 1002");
                                err = fclose_rs((unsafe { *rsm }).F);
                            }
                        }
                        free_rs((unsafe { *rsm }).buffer);
                        hash_remove_rs(globals.fdhash, l.s);
                    }
                    if err != 0 {
            //println!("line 1010, err != 0");
                        setvar_i_rs(globals2.intvar[ERRNO as usize], unsafe { (*__errno_location()).into() });
                    }
                    R_d = err.into();
                },
                _ => {

                }
            } /* switch */
            //println!("R_d = {}", R_d);
            setvar_i_rs(res, R_d);
            //println!("res.string = {:?}", unsafe { std::ffi::CStr::from_ptr((*res).string) });
        },
        OC_BUILTIN => {
            //println!("line 1022, OC_BUILTIN");
            res = unsafe { exec_builtin(op, res) };
        },
        OC_SPRINTF => {
            //println!("line 1026, OC_SPRINTF");
            setvar_p_rs(res, unsafe { awk_printf(op1) });
        },
        OC_UNARY => {
            //println!("line 1030, OC_UNARY");
            let mut Ld: f64;
            let mut R_d: f64;
            R_d = getvar_i_rs(r.v);
            Ld = R_d;
            match opn {
                80 => { // case 'P'
            //println!("line 1037, case 'P'");
                    R_d += 1.0;
                    Ld = R_d;
                    //goto r_op_change
                    setvar_i_rs(r.v, R_d);
                },
                112 => { // case 'p'
            //println!("line 1044, case 'p'");
                    R_d += 1.0;
                    //goto r_op_change
                    setvar_i_rs(r.v, R_d);
                },
                77 => { // case 'M'
            //println!("line 1050, case 'M'");
                    R_d -= 1.0;
                    Ld = R_d;
                    //goto r_op_change
                    setvar_i_rs(r.v, R_d);
                },
                109 => { // case 'm' 
            //println!("line 1057, case 'm'");
                    R_d -= 1.0;
                    //r_op_change
                    setvar_i_rs(r.v, R_d);
                },
                33 => { // case '!'
            //println!("line 1062, case '!'");
                    if istrue_rs(r.v) == 0 {
            //println!("line 1065, Ld = 1");
                        Ld = 1.0;
                    } else {
            //println!("line 1068, Ld = 0");
                        Ld = 0.0;
                    }
                },
                45 => { // case '-'
            //println!("line 1073, case '-'");
                    Ld = -R_d;
                },
                _ => {

                }
            }
            setvar_i_rs(res, Ld);
        },
        OC_FIELD => {
            //println!("line 1083, OC_FIELD");
            let i = getvar_i_rs(r.v) as i32;
            if i < 0 {
                syntax_error_rs(unsafe { EMSG_NEGATIVE_FIELD }.as_ptr());
            }
            if i == 0 {
            //println!("line 1089, i == 0");
                res = globals2.intvar[F0 as usize];
            } else {
            //println!("line 1092, i != 0");
                split_f0_rs();
                if i > globals.nfields {
            //println!("line 1095, i > nfields");
                    fsrealloc_rs(i);
                }
                res = globals.Fields.wrapping_add((i - 1).try_into().unwrap());
            }
        },
        /* concatenation (" ") and index joining (",") */
        OC_CONCAT | OC_COMMA => {
            //println!("line 1103, OC_CONCAT/OC_COMMA");
            let mut sep: *const i8 = b"\0".as_ptr() as *const i8;
            if (opinfo & OPCLSMASK) == OC_COMMA {
            //println!("line 1106");
                sep = getvar_s_rs(globals2.intvar[SUBSEP as usize]);
            }
            // xasprintf???

            setvar_p_rs(res, unsafe { xasprintf("%s%s%s\0".as_ptr() as *const i8, l.s, sep, r.s) });
        },
        OC_LAND => {
            //println!("line 1106, OC_LAND");
            let mut val = 0;
            if istrue_rs(l.v) != 0 {
                val = unsafe { ptest((*op).r.n) };
            }
            setvar_i_rs(res, val.into());
        },
        OC_LOR => {
            let mut val = 1;
            if istrue_rs(l.v) == 0 {
                val = unsafe { ptest((*op).r.n) };
            }
            setvar_i_rs(res, val.into());
        },
        OC_BINARY | OC_REPLACE => {
            let R_d = getvar_i_rs(r.v);
            // debug_printf_eval("BINARY/REPLACE: R_d:%f opn:%c\n", R_d, opn);
            match opn {
                43 => { // case '+'
                    L_d += R_d;
                },
                45 => { // case '-'
                    L_d -= R_d;
                },
                42 => { // case '*'
                    L_d *= R_d;
                },
                47 => { // case '/'
                    if R_d == 0.0 {
                        syntax_error_rs(unsafe {EMSG_DIV_BY_ZERO}.as_ptr());
                    }
                    L_d /= R_d;
                },
                38 => { // case '&'
                    if ENABLE_FEATURE_AWK_LIBM != 0 {
                        L_d = f64::powf(L_d, R_d);
                    } else {
                        syntax_error_rs(unsafe {EMSG_NO_MATH}.as_ptr());
                    }
                },
                37 => { // case '%'
                    if R_d == 0.0 {
                        syntax_error_rs(unsafe {EMSG_DIV_BY_ZERO}.as_ptr());
                    }
                    L_d -= ((L_d / R_d) * R_d);
                }
                _ => {

                }
            }
            // debug_printf_eval("BINARY/REPLACE result:%f\n", L_d);
            let mut var_tmp = l.v;
            if (opinfo & OPCLSMASK) == OC_BINARY {
                var_tmp = res;
            }
            res = setvar_i_rs(var_tmp, L_d);
        },
        OC_COMPARE => {
            let mut i: i32 = 0;
            let mut Ld: f64;

            if is_numeric_rs(l.v) != 0 && is_numeric_rs(r.v) != 0 {
                Ld = getvar_i_rs(l.v) - getvar_i_rs(r.v);
            } else {
                let l = getvar_s_rs(l.v);
                let r = getvar_s_rs(r.v);
                Ld = unsafe { strcmp(l, r).into() };
                if globals.icase != 0 {
                    Ld = unsafe { strcasecmp(l, r).into() };
                }
            }
            match opn & 0xfe {
                0 => {
                    if Ld > 0.0 {
                        i = 1;
                    }
                },
                2 => {
                    if Ld >= 0.0 {
                        i = 1;
                    }
                },
                4 => {
                    if Ld == 0.0 {
                        i = 1;
                    }
                },
                _ => {

                }
            }
            let mut left = 0;
            let mut right = 0;
            if i == 0 {
                left = 1;
            }
            if (opn & 1) != 0 {
                right = 1;
            }
            setvar_i_rs(res, (left ^ right) as f64);
        },
        _ => {
            syntax_error_rs(unsafe {EMSG_POSSIBLE_ERROR.as_ptr()});
        }            
        } /* switch */
        //next
        if (opinfo & OPCLSMASK) <= SHIFT_TIL_THIS {
            op = unsafe { (*op).a.n };
            continue;
        }
        if (opinfo & OPCLSMASK) >= RECUR_FROM_THIS {
            break;
        }
        if globals.nextrec != 0 {
            break;
        }
    } /* while (op) */

    nvfree_rs(v1);
    // debug_printf_eval("returning from %s(): %p\n", __func__, res);
    res
}

fn nvfree_rs(v1: *mut var) {
    unsafe { nvfree(v1) }
}

fn hash_remove_rs(hash: *mut xhash, name: *const i8) {
    unsafe { hash_remove(hash, name) }
}

fn clear_array_rs(array: *mut xhash) {
    unsafe { clear_array(array) }
}

fn iamarray_rs(v: *mut var) -> *mut xhash {
    unsafe { iamarray(v) }
}

fn deref_node_rs(ptr: *mut node) -> node {
    unsafe { *ptr }
}

fn awk_main_rs(_argc: i32, mut argv: *mut *mut i8) {
    let mut opt_f: *mut i8 = null_mut();
    let mut list_v: *mut llist_t = null_mut();
    let mut list_f: *mut llist_t = null_mut();
    let mut _list_e: *mut llist_t = null_mut();
    if ENABLE_FEATURE_AWK_GNU_EXTENSIONS != 0 {
        _list_e = null_mut();
    }
    let mut i: usize;
    let mut j: i32;
    let mut _v: *mut var = null_mut();
    let mut tv = init_var_rs();
    let (mut vnames, mut vvalues) = init_v_rs();

    init_g_rs();
    let (globals, globals2) = init_globs_rs();
	
    /* Undo busybox.c, or else strtod may eat ','! This breaks parsing:
	 * $1,$2 == '$1,' '$2', NOT '$1' ',' '$2' */
    if ENABLE_LOCALE_SUPPORT != 0 {
        set_locale_rs(LC_NUMERIC, "C\0");
    }
    zero_out_var_rs(&mut tv); 

    /* allocate global buffer */
    globals.g_buf = xmalloc_rs(MAXVARFMT as usize + 1) as *mut i8;
   
    globals.vhash = hash_init_rs();
    globals.ahash = hash_init_rs();
    globals.fdhash = hash_init_rs();
    globals.fnhash = hash_init_rs();

    /* initialize variables */
    i = 0;
    while deref_mut_i8_rs(vnames) != 0 {
        if i >= globals2.intvar.len() {
            break;
        }
        _v = newvar_rs(globals.vhash, nextword_rs(&mut vnames)) as *mut var;
        globals2.intvar[i] = _v;
        if deref_mut_i8_rs(vvalues) != -1 {
            setvar_s_rs(_v, nextword_rs(&mut vvalues));
        } else {
            setvar_i_rs(_v, 0.0);
        }
        if deref_mut_i8_rs(vnames) == '*' as i8 {
            bitwise_or_type_rs(_v, VF_SPECIAL);
            vnames = vnames.wrapping_add(1);
        }

        // Increment the loop counter
        i += 1;
    }

    // unsafe { handle_special(globals2.intvar[FS as usize]) };
    // unsafe { handle_special(globals2.intvar[RS as usize]) };
    handle_special_rs(globals2.intvar[FS as usize]);
    handle_special_rs(globals2.intvar[RS as usize]);

    setup_rstream_rs(globals);

    for (key, value) in env::vars() {
        let mut s = key;
        s.push('=');
        s.push_str(&value);
        s.push('\0');
        let s1 = strchr_rs(&s);
        if s1 != null_mut() {
            set_env_var_rs(s1, globals2, s);
        }
    }

    #[cfg(feature = "ENABLE_FEATURE_AWK_GNU_EXTENSIONS")]
    let opt = _getopt32_rs(argv, &mut opt_f, &mut list_v, &mut list_f, &mut list_e);
    #[cfg(not(feature = "ENABLE_FEATURE_AWK_GNU_EXTENSIONS"))]
    let opt = getopt32_rs_vers2(argv, &mut opt_f, &mut list_v, &mut list_f);
    argv = argv.wrapping_add(getoptind_rs() as usize);
    if opt & OPT_W != 0 {
        bb_simple_error_msg_rs("warning: option -W is ignored\0") ;
    }
    if opt & OPT_F != 0 {
        unescape_string_in_place_rs(opt_f);
        setvar_s_rs(globals2.intvar[FS as usize], opt_f);
    }
    while list_v != null_mut() {
        if is_assignment_rs(llist_pop_rs(&mut list_v) as *const i8) == 0 {
            bb_show_usage_rs();
        }
    }
    while list_f != null_mut() {
        let mut s: *mut i8 = null_mut();
        let from_file: *mut FILE;

        globals.g_progname = llist_pop_rs(&mut list_f) as *const i8;
        from_file = xopen_stdin_rs(globals.g_progname);
        /* one byte is reserved for some trick in next_token */
        i = 1;
        j = 1;
        while j > 0 {
            s = xrealloc_rs(s, i + 4096) as *mut i8;
            j = fread_rs(
                    s.wrapping_add(i) as *mut c_void,
                    1,
                    4094,
                    from_file,
                )
                .try_into()
                .unwrap();
            i += j as usize;
        }
        set_si_0_rs(s, i);
        fclose_rs(from_file);
        parse_program_rs(s.wrapping_add(1));
        free_rs(s);
    }
    globals.g_progname = "cmd. line\0".as_ptr() as *const i8;
    #[cfg(feature = "ENABLE_FEATURE_AWK_GNU_EXTENSIONS")]
    while list_e != null_mut() {
        parse_program_rs(llist_pop_rs(&mut list_e) as *mut i8);
    }
    if (opt & (OPT_f | OPT_e)) == 0 {
        if deref_mut_mut_i8_rs(argv) == null_mut() {
            bb_show_usage_rs();
        }
        parse_program_rs(deref_mut_mut_i8_rs(argv));
        argv = argv.wrapping_add(1);
    }

    /* fill in ARGV array */
    setari_u_rs(
        globals2.intvar[ARGV as usize],
        0,
        "awk\0".as_ptr() as *const i8,
    );
    i = 0;
    while deref_mut_mut_i8_rs(argv) != null_mut() {
        i += 1;
        setari_u_rs(
            globals2.intvar[ARGV as usize],
            i.try_into().unwrap(),
            deref_mut_mut_i8_rs(argv)
        );
        argv = argv.wrapping_add(1);
    }
    setvar_i_rs(globals2.intvar[ARGC as usize], (i + 1) as f64);

    evaluate_rs(globals.beginseq.first, &mut tv);

    if globals.mainseq.first == null_mut() && globals.endseq.first == null_mut() {
        awk_exit_rs(EXIT_SUCCESS);
    }
    /* input file could already be opened in BEGIN block */
    if globals.iF == null_mut() {
        globals.iF = next_input_file_rs();
    }
    /* passing through input files */
    while globals.iF != null_mut() {
        globals.nextfile = 0;
        setvar_i_rs(globals2.intvar[FNR as usize], 0.0);

        // unsafe { print_rstream(globals.iF) };
        // unsafe { print_var(intvar[F0 as usize]) };
        i = awk_getline_rs(globals.iF, globals2.intvar[F0 as usize]) as usize;
        while i > 0 {
            globals.nextrec = 0;
            incvar_rs(globals2.intvar[NR as usize]);
            incvar_rs(globals2.intvar[FNR as usize]);
            
            evaluate_rs(globals.mainseq.first, &mut tv);

            if globals.nextfile != 0 {
                break;
            }
            i = awk_getline_rs(globals.iF, globals2.intvar[F0 as usize]) as usize;
        }

        if (i as i32) < 0 {
            syntax_error_rs(strerror_rs(ERRNO.try_into().unwrap()));
        }
        globals.iF = next_input_file_rs();
    }

    awk_exit_rs(EXIT_SUCCESS);
}

// ...

fn set_applet_name_rs(c_args: &Vec<*const i8>) {
    unsafe { applet_name = c_args[0] };
}

fn main() {
    // //println!("Translate `main` function first!");
    // parse arguments to C function awk_main
    // ...
    // awk_main(...)
    // ...

    // Collect command line arguments
    let args = env::args()
        .map(|arg| CString::new(arg).unwrap())
        .collect::<Vec<CString>>();

    // Convert Rust strings to C-style strings
    let mut c_args = args
        .iter()
        .map(|arg| arg.as_ptr())
        .collect::<Vec<*const c_char>>();

    c_args.push(null_mut());

    // Call the C function
    set_applet_name_rs(&c_args);
    awk_main_rs(c_args.len() as c_int, c_args.as_ptr() as *mut *mut _);
    // let _result = unsafe { awk_main(c_args.len() as c_int, c_args.as_ptr() as *mut *mut _) };

}
