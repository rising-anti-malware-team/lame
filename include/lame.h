

#ifndef __RAME_EZ_API__
#define __RAME_EZ_API__

#include "sdk.types.h"


typedef void *				rx_obj_t;
typedef rx_obj_t			rx_sdr_t;
typedef rx_obj_t			rx_sfr_t;
typedef void *				rx_ptr_t;
typedef const char *		rx_cstr_t;
typedef const wchar_t *		rx_cwstr_t;
typedef char *				rx_str_t;

typedef struct rx_result_t {
	rx_mk_t			mklass;
	rx_cstr_t		engid;
	rx_cstr_t		vname;
	uint32_t		vid32;
	uint64_t		vid40;
	rx_cstr_t		hitag;
	rx_trc_t		treat;
} rx_result_t;

#ifdef _WIN32
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;
typedef long long			int64_t;
#endif


typedef enum rx_obj_id_t {
	COBJ_LAME,
	COBJ_CONF,
	COBJ_RESULT,
} rx_obj_id_t;



EXTERN_C rx_obj_t rx_obj_create(rx_obj_id_t objid); // object has been referenced
EXTERN_C void rx_obj_reference(rx_obj_t);
EXTERN_C void rx_obj_release(rx_obj_t);

EXTERN_C long rx_conf_set_json(rx_obj_t conf, rx_cstr_t content);
EXTERN_C long rx_conf_set_str(rx_obj_t conf, rx_cstr_t path, rx_cstr_t val);
EXTERN_C long rx_conf_set_int(rx_obj_t conf, rx_cstr_t path, const int64_t val);
EXTERN_C long rx_conf_set_bool(rx_obj_t conf, rx_cstr_t path, const bool val);
EXTERN_C long rx_conf_erase(rx_obj_t conf, rx_cstr_t path);

EXTERN_C long rx_conf_get_str(rx_obj_t conf, rx_cstr_t path, rx_cstr_t * pval);
EXTERN_C long rx_conf_get_int(rx_obj_t conf, rx_cstr_t path, int64_t * pval);
EXTERN_C long rx_conf_get_bool(rx_obj_t conf, rx_cstr_t path, bool * pval);

EXTERN_C long rx_conf_cloud_init(rx_obj_t conf);
EXTERN_C long rx_lame_init(rx_obj_t eng, rx_obj_t conf);
EXTERN_C rx_sdr_t rx_lame_scan(rx_obj_t eng, rx_cstr_t rx_uri, rx_scan_opt_t opt);
EXTERN_C rx_sdr_t rx_lame_scan_w(rx_obj_t eng, rx_cwstr_t rx_uri, rx_scan_opt_t opt);
EXTERN_C rx_sdr_t rx_lame_scan_memory(rx_obj_t eng, rx_cstr_t buffer, int length);

EXTERN_C bool rx_sdr_get_result(rx_sdr_t sdr, rx_result_t * result);	// return false if NOT infected.

EXTERN_C bool rx_lame_get_version(rx_str_t ver, int length);
EXTERN_C bool rx_lame_get_virus_library_version(rx_str_t ver, int length, rx_cstr_t vlcfgpath);
EXTERN_C bool rx_lame_get_licence_info(rx_licence_info* info, rx_cstr_t path);



#ifdef __cplusplus
namespace rx {
	struct objptr_t {
		rx_obj_t ptr;
		rx_obj_t create(rx_obj_id_t obj_id) {
			dispose(); ptr = rx_obj_create(obj_id); return ptr;
		}
		objptr_t():ptr(0){
		}
		objptr_t(rx_obj_id_t obj_id):ptr(0) {
			create(obj_id);
		}
		objptr_t(const rx_obj_t p, bool add_ref = false) {
			ptr = p; if (add_ref) rx_obj_reference(ptr);
		}
		objptr_t(const objptr_t & r):ptr(0) {

		}
		virtual ~objptr_t() {
			dispose();
		}
		objptr_t & operator = (rx_obj_id_t obj_id) {
			dispose(); create(obj_id); return *this;
		}
		objptr_t & operator = (const rx_obj_t p ) {
			dispose(); ptr = p; return *this;
		}
		objptr_t & operator = (const objptr_t & r) {
			dispose(); return *this;
		}
		inline void attach(rx_obj_t p, bool add_ref = false) {
			dispose(); ptr = p;
		}
		inline void dispose() {
			if (ptr) rx_obj_release(ptr); ptr = 0;
		}
		inline rx_obj_t detach() {
			rx_obj_t r = ptr; ptr = 0; return r;
		}
		rx_obj_t _ptr() {
			return ptr;
		};
		operator rx_obj_t () const {
			return ptr;
		}
	};
};

#endif




#endif //__RAME_EZ_API__