## CMakeLists.txt					-*- CMake -*-
##
## Copyright (C) 2011-2016 Christian Schenk
## 
## This file is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published
## by the Free Software Foundation; either version 2, or (at your
## option) any later version.
## 
## This file is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this file; if not, write to the Free Software
## Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
## USA.

set(botan_sources
  ${botan_source_dir}/source/src/algo_factory/algo_cache.h
  ${botan_source_dir}/source/src/algo_factory/algo_factory.cpp
  ${botan_source_dir}/source/src/algo_factory/algo_factory.h
  ${botan_source_dir}/source/src/algo_factory/prov_weight.cpp
  ${botan_source_dir}/source/src/alloc/allocate.h
  ${botan_source_dir}/source/src/alloc/mem_pool/mem_pool.cpp
  ${botan_source_dir}/source/src/alloc/mem_pool/mem_pool.h
  ${botan_source_dir}/source/src/alloc/secmem.h
  ${botan_source_dir}/source/src/alloc/system_alloc/defalloc.cpp
  ${botan_source_dir}/source/src/alloc/system_alloc/defalloc.h
  ${botan_source_dir}/source/src/asn1/alg_id.cpp
  ${botan_source_dir}/source/src/asn1/alg_id.h
  ${botan_source_dir}/source/src/asn1/asn1_alt.cpp
  ${botan_source_dir}/source/src/asn1/asn1_att.cpp
  ${botan_source_dir}/source/src/asn1/asn1_dn.cpp
  ${botan_source_dir}/source/src/asn1/asn1_int.cpp
  ${botan_source_dir}/source/src/asn1/asn1_int.h
  ${botan_source_dir}/source/src/asn1/asn1_obj.h
  ${botan_source_dir}/source/src/asn1/asn1_oid.cpp
  ${botan_source_dir}/source/src/asn1/asn1_oid.h
  ${botan_source_dir}/source/src/asn1/asn1_str.cpp
  ${botan_source_dir}/source/src/asn1/asn1_tm.cpp
  ${botan_source_dir}/source/src/asn1/ber_dec.cpp
  ${botan_source_dir}/source/src/asn1/ber_dec.h
  ${botan_source_dir}/source/src/asn1/der_enc.cpp
  ${botan_source_dir}/source/src/asn1/der_enc.h
  ${botan_source_dir}/source/src/benchmark/benchmark.cpp
  ${botan_source_dir}/source/src/benchmark/benchmark.h
  ${botan_source_dir}/source/src/block/aes/aes.cpp
  ${botan_source_dir}/source/src/block/aes/aes.h
  ${botan_source_dir}/source/src/block/aes/aes_tab.cpp
  ${botan_source_dir}/source/src/block/block_cipher.h
  ${botan_source_dir}/source/src/block/blowfish/blfs_tab.cpp
  ${botan_source_dir}/source/src/block/blowfish/blowfish.cpp
  ${botan_source_dir}/source/src/block/blowfish/blowfish.h
  ${botan_source_dir}/source/src/block/cast/cast128.cpp
  ${botan_source_dir}/source/src/block/cast/cast128.h
  ${botan_source_dir}/source/src/block/cast/cast256.cpp
  ${botan_source_dir}/source/src/block/cast/cast256.h
  ${botan_source_dir}/source/src/block/cast/cast_tab.cpp
  ${botan_source_dir}/source/src/block/des/des.cpp
  ${botan_source_dir}/source/src/block/des/des.h
  ${botan_source_dir}/source/src/block/des/des_tab.cpp
  ${botan_source_dir}/source/src/block/des/desx.cpp
  ${botan_source_dir}/source/src/block/des/desx.h
  ${botan_source_dir}/source/src/block/gost_28147/gost_28147.cpp
  ${botan_source_dir}/source/src/block/gost_28147/gost_28147.h
  ${botan_source_dir}/source/src/block/idea/idea.cpp
  ${botan_source_dir}/source/src/block/idea/idea.h
  ${botan_source_dir}/source/src/block/kasumi/kasumi.cpp
  ${botan_source_dir}/source/src/block/kasumi/kasumi.h
  ${botan_source_dir}/source/src/block/lion/lion.cpp
  ${botan_source_dir}/source/src/block/lion/lion.h
  ${botan_source_dir}/source/src/block/lubyrack/lubyrack.cpp
  ${botan_source_dir}/source/src/block/lubyrack/lubyrack.h
  ${botan_source_dir}/source/src/block/mars/mars.cpp
  ${botan_source_dir}/source/src/block/mars/mars.h
  ${botan_source_dir}/source/src/block/mars/mars_tab.cpp
  ${botan_source_dir}/source/src/block/misty1/misty1.cpp
  ${botan_source_dir}/source/src/block/misty1/misty1.h
  ${botan_source_dir}/source/src/block/noekeon/noekeon.cpp
  ${botan_source_dir}/source/src/block/noekeon/noekeon.h
  ${botan_source_dir}/source/src/block/rc2/rc2.cpp
  ${botan_source_dir}/source/src/block/rc2/rc2.h
  ${botan_source_dir}/source/src/block/rc5/rc5.cpp
  ${botan_source_dir}/source/src/block/rc5/rc5.h
  ${botan_source_dir}/source/src/block/rc6/rc6.cpp
  ${botan_source_dir}/source/src/block/rc6/rc6.h
  ${botan_source_dir}/source/src/block/safer/safe_tab.cpp
  ${botan_source_dir}/source/src/block/safer/safer_sk.cpp
  ${botan_source_dir}/source/src/block/safer/safer_sk.h
  ${botan_source_dir}/source/src/block/seed/seed.cpp
  ${botan_source_dir}/source/src/block/seed/seed.h
  ${botan_source_dir}/source/src/block/seed/seed_tab.cpp
  ${botan_source_dir}/source/src/block/serpent/serpent.cpp
  ${botan_source_dir}/source/src/block/serpent/serpent.h
  ${botan_source_dir}/source/src/block/skipjack/skipjack.cpp
  ${botan_source_dir}/source/src/block/skipjack/skipjack.h
  ${botan_source_dir}/source/src/block/square/sqr_tab.cpp
  ${botan_source_dir}/source/src/block/square/square.cpp
  ${botan_source_dir}/source/src/block/square/square.h
  ${botan_source_dir}/source/src/block/tea/tea.cpp
  ${botan_source_dir}/source/src/block/tea/tea.h
  ${botan_source_dir}/source/src/block/twofish/two_tab.cpp
  ${botan_source_dir}/source/src/block/twofish/twofish.cpp
  ${botan_source_dir}/source/src/block/twofish/twofish.h
  ${botan_source_dir}/source/src/block/xtea/xtea.cpp
  ${botan_source_dir}/source/src/block/xtea/xtea.h
  ${botan_source_dir}/source/src/cert/x509/certstor.cpp
  ${botan_source_dir}/source/src/cert/x509/certstor.h
  ${botan_source_dir}/source/src/cert/x509/crl_ent.cpp
  ${botan_source_dir}/source/src/cert/x509/crl_ent.h
  ${botan_source_dir}/source/src/cert/x509/pkcs10.cpp
  ${botan_source_dir}/source/src/cert/x509/pkcs10.h
  ${botan_source_dir}/source/src/cert/x509/x509_ca.cpp
  ${botan_source_dir}/source/src/cert/x509/x509_ca.h
  ${botan_source_dir}/source/src/cert/x509/x509_crl.cpp
  ${botan_source_dir}/source/src/cert/x509/x509_crl.h
  ${botan_source_dir}/source/src/cert/x509/x509_ext.cpp
  ${botan_source_dir}/source/src/cert/x509/x509_ext.h
  ${botan_source_dir}/source/src/cert/x509/x509_obj.cpp
  ${botan_source_dir}/source/src/cert/x509/x509_obj.h
  ${botan_source_dir}/source/src/cert/x509/x509cert.cpp
  ${botan_source_dir}/source/src/cert/x509/x509cert.h
  ${botan_source_dir}/source/src/cert/x509/x509find.cpp
  ${botan_source_dir}/source/src/cert/x509/x509find.h
  ${botan_source_dir}/source/src/cert/x509/x509opt.cpp
  ${botan_source_dir}/source/src/cert/x509/x509self.cpp
  ${botan_source_dir}/source/src/cert/x509/x509self.h
  ${botan_source_dir}/source/src/cert/x509/x509stor.cpp
  ${botan_source_dir}/source/src/cert/x509/x509stor.h
  ${botan_source_dir}/source/src/checksum/adler32/adler32.cpp
  ${botan_source_dir}/source/src/checksum/adler32/adler32.h
  ${botan_source_dir}/source/src/checksum/crc24/crc24.cpp
  ${botan_source_dir}/source/src/checksum/crc24/crc24.h
  ${botan_source_dir}/source/src/checksum/crc32/crc32.cpp
  ${botan_source_dir}/source/src/checksum/crc32/crc32.h
  ${botan_source_dir}/source/src/cms/cms_algo.cpp
  ${botan_source_dir}/source/src/cms/cms_comp.cpp
  ${botan_source_dir}/source/src/cms/cms_dalg.cpp
  ${botan_source_dir}/source/src/cms/cms_dec.cpp
  ${botan_source_dir}/source/src/cms/cms_dec.h
  ${botan_source_dir}/source/src/cms/cms_ealg.cpp
  ${botan_source_dir}/source/src/cms/cms_enc.cpp
  ${botan_source_dir}/source/src/cms/cms_enc.h
  ${botan_source_dir}/source/src/codec/base64/b64_char.cpp
  ${botan_source_dir}/source/src/codec/base64/base64.cpp
  ${botan_source_dir}/source/src/codec/base64/base64.h
  ${botan_source_dir}/source/src/codec/hex/hex.cpp
  ${botan_source_dir}/source/src/codec/hex/hex.h
  ${botan_source_dir}/source/src/codec/hex/hex_char.cpp
  ${botan_source_dir}/source/src/codec/openpgp/openpgp.cpp
  ${botan_source_dir}/source/src/codec/openpgp/openpgp.h
  ${botan_source_dir}/source/src/codec/pem/pem.cpp
  ${botan_source_dir}/source/src/codec/pem/pem.h
  ${botan_source_dir}/source/src/cryptobox/cryptobox.cpp
  ${botan_source_dir}/source/src/cryptobox/cryptobox.h
  ${botan_source_dir}/source/src/engine/def_engine/def_eng.h
  ${botan_source_dir}/source/src/engine/def_engine/def_mode.cpp
  ${botan_source_dir}/source/src/engine/def_engine/def_pk_ops.cpp
  ${botan_source_dir}/source/src/engine/def_engine/def_powm.cpp
  ${botan_source_dir}/source/src/engine/def_engine/lookup_block.cpp
  ${botan_source_dir}/source/src/engine/def_engine/lookup_hash.cpp
  ${botan_source_dir}/source/src/engine/def_engine/lookup_mac.cpp
  ${botan_source_dir}/source/src/engine/def_engine/lookup_stream.cpp
  ${botan_source_dir}/source/src/engine/engine.h
  ${botan_source_dir}/source/src/entropy/cryptoapi_rng/es_capi.cpp
  ${botan_source_dir}/source/src/entropy/cryptoapi_rng/es_capi.h
  ${botan_source_dir}/source/src/entropy/entropy_src.h
  ${botan_source_dir}/source/src/entropy/win32_stats/es_win32.cpp
  ${botan_source_dir}/source/src/entropy/win32_stats/es_win32.h
  ${botan_source_dir}/source/src/filters/algo_filt.cpp
  ${botan_source_dir}/source/src/filters/basefilt.cpp
  ${botan_source_dir}/source/src/filters/basefilt.h
  ${botan_source_dir}/source/src/filters/buf_filt.cpp
  ${botan_source_dir}/source/src/filters/buf_filt.h
  ${botan_source_dir}/source/src/filters/data_snk.cpp
  ${botan_source_dir}/source/src/filters/data_snk.h
  ${botan_source_dir}/source/src/filters/data_src.cpp
  ${botan_source_dir}/source/src/filters/data_src.h
  ${botan_source_dir}/source/src/filters/filter.cpp
  ${botan_source_dir}/source/src/filters/filter.h
  ${botan_source_dir}/source/src/filters/filters.h
  ${botan_source_dir}/source/src/filters/out_buf.cpp
  ${botan_source_dir}/source/src/filters/out_buf.h
  ${botan_source_dir}/source/src/filters/pbe.h
  ${botan_source_dir}/source/src/filters/pipe.cpp
  ${botan_source_dir}/source/src/filters/pipe.h
  ${botan_source_dir}/source/src/filters/pipe_io.cpp
  ${botan_source_dir}/source/src/filters/pipe_rw.cpp
  ${botan_source_dir}/source/src/filters/secqueue.cpp
  ${botan_source_dir}/source/src/filters/secqueue.h
  ${botan_source_dir}/source/src/hash/fork256/fork256.cpp
  ${botan_source_dir}/source/src/hash/fork256/fork256.h
  ${botan_source_dir}/source/src/hash/gost_3411/gost_3411.cpp
  ${botan_source_dir}/source/src/hash/gost_3411/gost_3411.h
  ${botan_source_dir}/source/src/hash/has160/has160.cpp
  ${botan_source_dir}/source/src/hash/has160/has160.h
  ${botan_source_dir}/source/src/hash/hash.h
  ${botan_source_dir}/source/src/hash/md2/md2.cpp
  ${botan_source_dir}/source/src/hash/md2/md2.h
  ${botan_source_dir}/source/src/hash/md4/md4.cpp
  ${botan_source_dir}/source/src/hash/md4/md4.h
  ${botan_source_dir}/source/src/hash/md5/md5.cpp
  ${botan_source_dir}/source/src/hash/md5/md5.h
  ${botan_source_dir}/source/src/hash/mdx_hash/mdx_hash.cpp
  ${botan_source_dir}/source/src/hash/mdx_hash/mdx_hash.h
  ${botan_source_dir}/source/src/hash/par_hash/par_hash.cpp
  ${botan_source_dir}/source/src/hash/par_hash/par_hash.h
  ${botan_source_dir}/source/src/hash/rmd128/rmd128.cpp
  ${botan_source_dir}/source/src/hash/rmd128/rmd128.h
  ${botan_source_dir}/source/src/hash/rmd160/rmd160.cpp
  ${botan_source_dir}/source/src/hash/rmd160/rmd160.h
  ${botan_source_dir}/source/src/hash/sha1/sha160.cpp
  ${botan_source_dir}/source/src/hash/sha1/sha160.h
  ${botan_source_dir}/source/src/hash/sha2/sha2_32.cpp
  ${botan_source_dir}/source/src/hash/sha2/sha2_32.h
  ${botan_source_dir}/source/src/hash/sha2/sha2_64.cpp
  ${botan_source_dir}/source/src/hash/sha2/sha2_64.h
  ${botan_source_dir}/source/src/hash/skein/skein_512.cpp
  ${botan_source_dir}/source/src/hash/skein/skein_512.h
  ${botan_source_dir}/source/src/hash/tiger/tig_tab.cpp
  ${botan_source_dir}/source/src/hash/tiger/tiger.cpp
  ${botan_source_dir}/source/src/hash/tiger/tiger.h
  ${botan_source_dir}/source/src/hash/whirlpool/whrl_tab.cpp
  ${botan_source_dir}/source/src/hash/whirlpool/whrlpool.cpp
  ${botan_source_dir}/source/src/hash/whirlpool/whrlpool.h
  ${botan_source_dir}/source/src/kdf/kdf.cpp
  ${botan_source_dir}/source/src/kdf/kdf.h
  ${botan_source_dir}/source/src/kdf/kdf1/kdf1.cpp
  ${botan_source_dir}/source/src/kdf/kdf1/kdf1.h
  ${botan_source_dir}/source/src/kdf/kdf2/kdf2.cpp
  ${botan_source_dir}/source/src/kdf/kdf2/kdf2.h
  ${botan_source_dir}/source/src/kdf/mgf1/mgf1.cpp
  ${botan_source_dir}/source/src/kdf/mgf1/mgf1.h
  ${botan_source_dir}/source/src/kdf/ssl_prf/prf_ssl3.cpp
  ${botan_source_dir}/source/src/kdf/ssl_prf/prf_ssl3.h
  ${botan_source_dir}/source/src/kdf/tls_prf/prf_tls.cpp
  ${botan_source_dir}/source/src/kdf/tls_prf/prf_tls.h
  ${botan_source_dir}/source/src/kdf/x942_prf/prf_x942.cpp
  ${botan_source_dir}/source/src/kdf/x942_prf/prf_x942.h
  ${botan_source_dir}/source/src/libstate/botan.h
  ${botan_source_dir}/source/src/libstate/get_enc.cpp
  ${botan_source_dir}/source/src/libstate/init.cpp
  ${botan_source_dir}/source/src/libstate/init.h
  ${botan_source_dir}/source/src/libstate/libstate.cpp
  ${botan_source_dir}/source/src/libstate/libstate.h
  ${botan_source_dir}/source/src/libstate/look_pk.cpp
  ${botan_source_dir}/source/src/libstate/look_pk.h
  ${botan_source_dir}/source/src/libstate/lookup.cpp
  ${botan_source_dir}/source/src/libstate/lookup.h
  ${botan_source_dir}/source/src/libstate/oid_lookup/oids.cpp
  ${botan_source_dir}/source/src/libstate/oid_lookup/oids.h
  ${botan_source_dir}/source/src/libstate/pk_engine.cpp
  ${botan_source_dir}/source/src/libstate/pk_engine.h
  ${botan_source_dir}/source/src/libstate/policy.cpp
  ${botan_source_dir}/source/src/libstate/scan_name.cpp
  ${botan_source_dir}/source/src/libstate/scan_name.h
  ${botan_source_dir}/source/src/mac/cbc_mac/cbc_mac.cpp
  ${botan_source_dir}/source/src/mac/cbc_mac/cbc_mac.h
  ${botan_source_dir}/source/src/mac/cmac/cmac.cpp
  ${botan_source_dir}/source/src/mac/cmac/cmac.h
  ${botan_source_dir}/source/src/mac/hmac/hmac.cpp
  ${botan_source_dir}/source/src/mac/hmac/hmac.h
  ${botan_source_dir}/source/src/mac/mac.cpp
  ${botan_source_dir}/source/src/mac/mac.h
  ${botan_source_dir}/source/src/mac/ssl3mac/ssl3_mac.cpp
  ${botan_source_dir}/source/src/mac/ssl3mac/ssl3_mac.h
  ${botan_source_dir}/source/src/mac/x919_mac/x919_mac.cpp
  ${botan_source_dir}/source/src/mac/x919_mac/x919_mac.h
  ${botan_source_dir}/source/src/math/bigint/big_code.cpp
  ${botan_source_dir}/source/src/math/bigint/big_io.cpp
  ${botan_source_dir}/source/src/math/bigint/big_ops2.cpp
  ${botan_source_dir}/source/src/math/bigint/big_ops3.cpp
  ${botan_source_dir}/source/src/math/bigint/big_rand.cpp
  ${botan_source_dir}/source/src/math/bigint/bigint.cpp
  ${botan_source_dir}/source/src/math/bigint/bigint.h
  ${botan_source_dir}/source/src/math/bigint/divide.cpp
  ${botan_source_dir}/source/src/math/bigint/divide.h
  ${botan_source_dir}/source/src/math/bigint/monty_generic/mp_monty.cpp
  ${botan_source_dir}/source/src/math/bigint/mp_asm.cpp
  ${botan_source_dir}/source/src/math/bigint/mp_comba.cpp
  ${botan_source_dir}/source/src/math/bigint/mp_core.h
  ${botan_source_dir}/source/src/math/bigint/mp_generic/mp_asm.h
  ${botan_source_dir}/source/src/math/bigint/mp_generic/mp_asmi.h
  ${botan_source_dir}/source/src/math/bigint/mp_karat.cpp
  ${botan_source_dir}/source/src/math/bigint/mp_misc.cpp
  ${botan_source_dir}/source/src/math/bigint/mp_shift.cpp
  ${botan_source_dir}/source/src/math/bigint/mp_types.h
  ${botan_source_dir}/source/src/math/bigint/mulop_generic/mp_mulop.cpp
  ${botan_source_dir}/source/src/math/numbertheory/blinding.cpp
  ${botan_source_dir}/source/src/math/numbertheory/blinding.h
  ${botan_source_dir}/source/src/math/numbertheory/def_powm.h
  ${botan_source_dir}/source/src/math/numbertheory/dsa_gen.cpp
  ${botan_source_dir}/source/src/math/numbertheory/jacobi.cpp
  ${botan_source_dir}/source/src/math/numbertheory/make_prm.cpp
  ${botan_source_dir}/source/src/math/numbertheory/mp_numth.cpp
  ${botan_source_dir}/source/src/math/numbertheory/numthry.cpp
  ${botan_source_dir}/source/src/math/numbertheory/numthry.h
  ${botan_source_dir}/source/src/math/numbertheory/pow_mod.cpp
  ${botan_source_dir}/source/src/math/numbertheory/pow_mod.h
  ${botan_source_dir}/source/src/math/numbertheory/powm_fw.cpp
  ${botan_source_dir}/source/src/math/numbertheory/powm_mnt.cpp
  ${botan_source_dir}/source/src/math/numbertheory/primes.cpp
  ${botan_source_dir}/source/src/math/numbertheory/reducer.cpp
  ${botan_source_dir}/source/src/math/numbertheory/reducer.h
  ${botan_source_dir}/source/src/math/numbertheory/ressol.cpp
  ${botan_source_dir}/source/src/modes/cbc/cbc.cpp
  ${botan_source_dir}/source/src/modes/cbc/cbc.h
  ${botan_source_dir}/source/src/modes/cfb/cfb.cpp
  ${botan_source_dir}/source/src/modes/cfb/cfb.h
  ${botan_source_dir}/source/src/modes/ctr/ctr.cpp
  ${botan_source_dir}/source/src/modes/ctr/ctr.h
  ${botan_source_dir}/source/src/modes/cts/cts.cpp
  ${botan_source_dir}/source/src/modes/cts/cts.h
  ${botan_source_dir}/source/src/modes/eax/eax.cpp
  ${botan_source_dir}/source/src/modes/eax/eax.h
  ${botan_source_dir}/source/src/modes/eax/eax_dec.cpp
  ${botan_source_dir}/source/src/modes/ecb/ecb.cpp
  ${botan_source_dir}/source/src/modes/ecb/ecb.h
  ${botan_source_dir}/source/src/modes/mode_pad/mode_pad.cpp
  ${botan_source_dir}/source/src/modes/mode_pad/mode_pad.h
  ${botan_source_dir}/source/src/modes/modebase.cpp
  ${botan_source_dir}/source/src/modes/modebase.h
  ${botan_source_dir}/source/src/modes/ofb/ofb.cpp
  ${botan_source_dir}/source/src/modes/ofb/ofb.h
  ${botan_source_dir}/source/src/modes/xts/xts.cpp
  ${botan_source_dir}/source/src/modes/xts/xts.h
  ${botan_source_dir}/source/src/mutex/mutex.h
  ${botan_source_dir}/source/src/mutex/noop_mutex/mux_noop.cpp
  ${botan_source_dir}/source/src/mutex/noop_mutex/mux_noop.h
  ${botan_source_dir}/source/src/mutex/win32_crit_section/mux_win32.cpp
  ${botan_source_dir}/source/src/mutex/win32_crit_section/mux_win32.h
  ${botan_source_dir}/source/src/pbe/get_pbe.cpp
  ${botan_source_dir}/source/src/pbe/get_pbe.h
  ${botan_source_dir}/source/src/pbe/pbes1/pbes1.cpp
  ${botan_source_dir}/source/src/pbe/pbes1/pbes1.h
  ${botan_source_dir}/source/src/pbe/pbes2/pbes2.cpp
  ${botan_source_dir}/source/src/pbe/pbes2/pbes2.h
  ${botan_source_dir}/source/src/pk_pad/eme.cpp
  ${botan_source_dir}/source/src/pk_pad/eme.h
  ${botan_source_dir}/source/src/pk_pad/eme1/eme1.cpp
  ${botan_source_dir}/source/src/pk_pad/eme1/eme1.h
  ${botan_source_dir}/source/src/pk_pad/eme_pkcs/eme_pkcs.cpp
  ${botan_source_dir}/source/src/pk_pad/eme_pkcs/eme_pkcs.h
  ${botan_source_dir}/source/src/pk_pad/emsa.h
  ${botan_source_dir}/source/src/pk_pad/emsa1/emsa1.cpp
  ${botan_source_dir}/source/src/pk_pad/emsa1/emsa1.h
  ${botan_source_dir}/source/src/pk_pad/emsa1_bsi/emsa1_bsi.cpp
  ${botan_source_dir}/source/src/pk_pad/emsa1_bsi/emsa1_bsi.h
  ${botan_source_dir}/source/src/pk_pad/emsa2/emsa2.cpp
  ${botan_source_dir}/source/src/pk_pad/emsa2/emsa2.h
  ${botan_source_dir}/source/src/pk_pad/emsa3/emsa3.cpp
  ${botan_source_dir}/source/src/pk_pad/emsa3/emsa3.h
  ${botan_source_dir}/source/src/pk_pad/emsa4/emsa4.cpp
  ${botan_source_dir}/source/src/pk_pad/emsa4/emsa4.h
  ${botan_source_dir}/source/src/pk_pad/emsa_raw/emsa_raw.cpp
  ${botan_source_dir}/source/src/pk_pad/emsa_raw/emsa_raw.h
  ${botan_source_dir}/source/src/pk_pad/hash_id/hash_id.cpp
  ${botan_source_dir}/source/src/pk_pad/hash_id/hash_id.h
  ${botan_source_dir}/source/src/pubkey/dh/dh.cpp
  ${botan_source_dir}/source/src/pubkey/dh/dh.h
  ${botan_source_dir}/source/src/pubkey/dh/dh_core.cpp
  ${botan_source_dir}/source/src/pubkey/dh/dh_core.h
  ${botan_source_dir}/source/src/pubkey/dh/dh_op.h
  ${botan_source_dir}/source/src/pubkey/dl_algo/dl_algo.cpp
  ${botan_source_dir}/source/src/pubkey/dl_algo/dl_algo.h
  ${botan_source_dir}/source/src/pubkey/dl_group/dl_group.cpp
  ${botan_source_dir}/source/src/pubkey/dl_group/dl_group.h
  ${botan_source_dir}/source/src/pubkey/dlies/dlies.cpp
  ${botan_source_dir}/source/src/pubkey/dlies/dlies.h
  ${botan_source_dir}/source/src/pubkey/dsa/dsa.cpp
  ${botan_source_dir}/source/src/pubkey/dsa/dsa.h
  ${botan_source_dir}/source/src/pubkey/dsa/dsa_core.cpp
  ${botan_source_dir}/source/src/pubkey/dsa/dsa_core.h
  ${botan_source_dir}/source/src/pubkey/dsa/dsa_op.cpp
  ${botan_source_dir}/source/src/pubkey/dsa/dsa_op.h
  ${botan_source_dir}/source/src/pubkey/elgamal/elg_core.cpp
  ${botan_source_dir}/source/src/pubkey/elgamal/elg_core.h
  ${botan_source_dir}/source/src/pubkey/elgamal/elg_op.cpp
  ${botan_source_dir}/source/src/pubkey/elgamal/elg_op.h
  ${botan_source_dir}/source/src/pubkey/elgamal/elgamal.cpp
  ${botan_source_dir}/source/src/pubkey/elgamal/elgamal.h
  ${botan_source_dir}/source/src/pubkey/if_algo/if_algo.cpp
  ${botan_source_dir}/source/src/pubkey/if_algo/if_algo.h
  ${botan_source_dir}/source/src/pubkey/if_algo/if_core.cpp
  ${botan_source_dir}/source/src/pubkey/if_algo/if_core.h
  ${botan_source_dir}/source/src/pubkey/if_algo/if_op.cpp
  ${botan_source_dir}/source/src/pubkey/if_algo/if_op.h
  ${botan_source_dir}/source/src/pubkey/keypair/keypair.cpp
  ${botan_source_dir}/source/src/pubkey/keypair/keypair.h
  ${botan_source_dir}/source/src/pubkey/nr/nr.cpp
  ${botan_source_dir}/source/src/pubkey/nr/nr.h
  ${botan_source_dir}/source/src/pubkey/nr/nr_core.cpp
  ${botan_source_dir}/source/src/pubkey/nr/nr_core.h
  ${botan_source_dir}/source/src/pubkey/nr/nr_op.cpp
  ${botan_source_dir}/source/src/pubkey/nr/nr_op.h
  ${botan_source_dir}/source/src/pubkey/pk_algs.cpp
  ${botan_source_dir}/source/src/pubkey/pk_algs.h
  ${botan_source_dir}/source/src/pubkey/pk_codecs/pkcs8.cpp
  ${botan_source_dir}/source/src/pubkey/pk_codecs/pkcs8.h
  ${botan_source_dir}/source/src/pubkey/pk_codecs/x509_key.cpp
  ${botan_source_dir}/source/src/pubkey/pk_codecs/x509_key.h
  ${botan_source_dir}/source/src/pubkey/pk_filts.cpp
  ${botan_source_dir}/source/src/pubkey/pk_filts.h
  ${botan_source_dir}/source/src/pubkey/pk_keys.cpp
  ${botan_source_dir}/source/src/pubkey/pk_keys.h
  ${botan_source_dir}/source/src/pubkey/pubkey.cpp
  ${botan_source_dir}/source/src/pubkey/pubkey.h
  ${botan_source_dir}/source/src/pubkey/pubkey_enums.cpp
  ${botan_source_dir}/source/src/pubkey/pubkey_enums.h
  ${botan_source_dir}/source/src/pubkey/rsa/rsa.cpp
  ${botan_source_dir}/source/src/pubkey/rsa/rsa.h
  ${botan_source_dir}/source/src/pubkey/rw/rw.cpp
  ${botan_source_dir}/source/src/pubkey/rw/rw.h
  ${botan_source_dir}/source/src/rng/auto_rng/auto_rng.cpp
  ${botan_source_dir}/source/src/rng/auto_rng/auto_rng.h
  ${botan_source_dir}/source/src/rng/hmac_rng/hmac_rng.cpp
  ${botan_source_dir}/source/src/rng/hmac_rng/hmac_rng.h
  ${botan_source_dir}/source/src/rng/randpool/randpool.cpp
  ${botan_source_dir}/source/src/rng/randpool/randpool.h
  ${botan_source_dir}/source/src/rng/rng.cpp
  ${botan_source_dir}/source/src/rng/rng.h
  ${botan_source_dir}/source/src/rng/x931_rng/x931_rng.cpp
  ${botan_source_dir}/source/src/rng/x931_rng/x931_rng.h
  ${botan_source_dir}/source/src/s2k/pbkdf1/pbkdf1.cpp
  ${botan_source_dir}/source/src/s2k/pbkdf1/pbkdf1.h
  ${botan_source_dir}/source/src/s2k/pbkdf2/pbkdf2.cpp
  ${botan_source_dir}/source/src/s2k/pbkdf2/pbkdf2.h
  ${botan_source_dir}/source/src/s2k/pgps2k/pgp_s2k.cpp
  ${botan_source_dir}/source/src/s2k/pgps2k/pgp_s2k.h
  ${botan_source_dir}/source/src/s2k/s2k.cpp
  ${botan_source_dir}/source/src/s2k/s2k.h
  ${botan_source_dir}/source/src/selftest/selftest.cpp
  ${botan_source_dir}/source/src/selftest/selftest.h
  ${botan_source_dir}/source/src/stream/arc4/arc4.cpp
  ${botan_source_dir}/source/src/stream/arc4/arc4.h
  ${botan_source_dir}/source/src/stream/salsa20/salsa20.cpp
  ${botan_source_dir}/source/src/stream/salsa20/salsa20.h
  ${botan_source_dir}/source/src/stream/stream_cipher.cpp
  ${botan_source_dir}/source/src/stream/stream_cipher.h
  ${botan_source_dir}/source/src/stream/turing/tur_tab.cpp
  ${botan_source_dir}/source/src/stream/turing/turing.cpp
  ${botan_source_dir}/source/src/stream/turing/turing.h
  ${botan_source_dir}/source/src/stream/wid_wake/wid_wake.cpp
  ${botan_source_dir}/source/src/stream/wid_wake/wid_wake.h
  ${botan_source_dir}/source/src/sym_algo/sym_algo.h
  ${botan_source_dir}/source/src/sym_algo/symkey.cpp
  ${botan_source_dir}/source/src/sym_algo/symkey.h
  ${botan_source_dir}/source/src/timer/timer.cpp
  ${botan_source_dir}/source/src/timer/timer.h
  ${botan_source_dir}/source/src/timer/win32_query_perf_ctr/tm_win32.cpp
  ${botan_source_dir}/source/src/timer/win32_query_perf_ctr/tm_win32.h
  ${botan_source_dir}/source/src/utils/bit_ops.h
  ${botan_source_dir}/source/src/utils/bswap.h
  ${botan_source_dir}/source/src/utils/buf_comp/buf_comp.h
  ${botan_source_dir}/source/src/utils/charset.cpp
  ${botan_source_dir}/source/src/utils/charset.h
  ${botan_source_dir}/source/src/utils/datastor/datastor.cpp
  ${botan_source_dir}/source/src/utils/datastor/datastor.h
  ${botan_source_dir}/source/src/utils/exceptn.cpp
  ${botan_source_dir}/source/src/utils/exceptn.h
  ${botan_source_dir}/source/src/utils/loadstor.h
  ${botan_source_dir}/source/src/utils/mem_ops.h
  ${botan_source_dir}/source/src/utils/mlock.cpp
  ${botan_source_dir}/source/src/utils/parsing.cpp
  ${botan_source_dir}/source/src/utils/parsing.h
  ${botan_source_dir}/source/src/utils/rotate.h
  ${botan_source_dir}/source/src/utils/stl_util.h
  ${botan_source_dir}/source/src/utils/types.h
  ${botan_source_dir}/source/src/utils/ui.cpp
  ${botan_source_dir}/source/src/utils/ui.h
  ${botan_source_dir}/source/src/utils/util.cpp
  ${botan_source_dir}/source/src/utils/util.h
  ${botan_source_dir}/source/src/utils/version.cpp
  ${botan_source_dir}/source/src/utils/version.h
  ${botan_source_dir}/source/src/utils/xor_buf.h

  ${CMAKE_CURRENT_SOURCE_DIR}/botan-version.h
)

set(botan_check_sources
  ${botan_source_dir}/source/checks/algos.cpp
  ${botan_source_dir}/source/checks/bench.cpp
  ${botan_source_dir}/source/checks/bigint.cpp
  ${botan_source_dir}/source/checks/block.cpp
  ${botan_source_dir}/source/checks/check.cpp
  ${botan_source_dir}/source/checks/clock.cpp
  ${botan_source_dir}/source/checks/cvc_tests.cpp
  ${botan_source_dir}/source/checks/dolook.cpp
  ${botan_source_dir}/source/checks/dolook2.cpp
  ${botan_source_dir}/source/checks/ec_tests.cpp
  ${botan_source_dir}/source/checks/ecdsa.cpp
  ${botan_source_dir}/source/checks/eckaeg.cpp
  ${botan_source_dir}/source/checks/getopt.cpp
  ${botan_source_dir}/source/checks/gfpmath.cpp
  ${botan_source_dir}/source/checks/misc.cpp
  ${botan_source_dir}/source/checks/pk.cpp
  ${botan_source_dir}/source/checks/pk_bench.cpp
  ${botan_source_dir}/source/checks/timer.cpp
  ${botan_source_dir}/source/checks/validate.cpp
  ${botan_source_dir}/source/checks/x509.cpp
)
